// Bugs Solution.cpp : Defines the class behaviors for the application.

//
/* Special thanks to EE312 student Steven Prickett for adding double buffering Spring 2013 */
//
#include "stdafx.h"
#include "WinMFCStuff.h"
#include "Bugs.h"

// CBugsSolutionApp construction

CBugsSolutionApp::CBugsSolutionApp()
{
}
CBugsSolutionApp theApp;


class CMyFrame : public CFrameWnd
{
	bool on;
	COLORREF color;
	public:
		CMyFrame(COLORREF);
	protected:
		afx_msg void OnPaint();
		afx_msg void OnTimer(UINT);
		DECLARE_MESSAGE_MAP()
};

const COLORREF RED = RGB(255, 0, 0);
const COLORREF GREEN = RGB(0, 255, 0);
const COLORREF WHITE = RGB(255, 255, 255);



CMyFrame::CMyFrame(COLORREF c) : color(c)
{
	Create( NULL, CString("EE312 BUGS!"));
}


// CBugsSolutionApp initialization
BOOL CBugsSolutionApp::InitInstance()
{
	// initialize the bug world
	createWorld();

	m_pMainWnd = new CMyFrame(RED);

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// start a timer for the simulation
	m_pMainWnd->SetTimer(1, 10, 0);

	return TRUE;
}

/* Global Variables for the simulation */
int world[WORLD_SIZE][WORLD_SIZE]; 
std::vector<Bug> bug_list;

BEGIN_MESSAGE_MAP( CMyFrame, CFrameWnd )
ON_WM_PAINT()
ON_WM_TIMER()
END_MESSAGE_MAP()


COLORREF bugColor(Bug* b) {
	int straight = (b->genes[0] * 255) / GENE_TOTAL;
	int right = (b->genes[1] + b->genes[2] + b->genes[3]) * 255 / GENE_TOTAL;
	int left = (b->genes[5] + b->genes[6] + b->genes[7]) * 255 / GENE_TOTAL;
	return RGB(straight, right, left);
}

void CMyFrame::OnPaint()
{
	CPaintDC dc( this );

	// setup bitmap buffers
	CBitmap bitmap;
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap(&dc, 3 * WORLD_SIZE, 3 * WORLD_SIZE);

	dcMem.SelectObject(&bitmap); 

	// use dcMem instead of dc for drawing the world bitmap (i.e., double buffering)
	dcMem.BitBlt(0, 0, 3 * WORLD_SIZE, 3 * WORLD_SIZE, &dc, 0, 0, WHITENESS);

	dcMem.BitBlt(0, 0, 3 * WORLD_SIZE, 3 * WORLD_SIZE, &dc, 0, 0, WHITENESS);

	for (int i = 0; i < WORLD_SIZE; i += 1) {
		for (int j = 0; j < WORLD_SIZE; j += 1) {
			switch(world[i][j]) {
			case EMPTY: // draw nothing
				break;
			case FOOD: // draw food
				dcMem.FillSolidRect(i * 3, j * 3, 3, 3, GREEN);
				break;
			default: // draw bug
				if (world[i][j] == 0) {
					dcMem.FillSolidRect(i * 3 - 4, j * 3 - 4, 9, 9, bugColor(&bug_list[world[i][j]]));
				} else {
					dcMem.FillSolidRect(i * 3, j * 3, 3, 3, bugColor(&bug_list[world[i][j]]));
				}
			}
		}
	}

	// draw buffered world
	dc.BitBlt(0, 0, 3 * WORLD_SIZE, 3 * WORLD_SIZE, &dcMem, 0, 0, SRCCOPY);

	
	if (time_step % 10 == 0) {
		dc.BitBlt(3*WORLD_SIZE, 0, 200, 3 * WORLD_SIZE, &dc, 3 * WORLD_SIZE, 1, SRCCOPY);

		int pop_point = bug_list.size() * 100 / (NUM_FOOD * 20) - 25;
		int stable_point = 75;
		dc.FillSolidRect(3*WORLD_SIZE, 3*WORLD_SIZE, 200, 1, RGB(255, 255, 255));
		char buff[128];	
		sprintf(buff, "time: %8d, #bugs %4d, avg age %4d, avg gen %6d,"
			" straight %2d%%, left %2d%%, right %2d%%, back %2d%%",
			time_step, bug_list.size(), average_age, average_generation,
			percent_straight, percent_left, percent_right, percent_back);
		CString cs(buff);
		dc.TextOut(0, 3*WORLD_SIZE, cs);
		dc.SetPixel(3*WORLD_SIZE + pop_point, 3*WORLD_SIZE, RGB(0, 0, 0));
		dc.SetPixel(3*WORLD_SIZE + stable_point, 3*WORLD_SIZE, RGB(255, 0, 0));


	}
}


void CMyFrame::OnTimer(UINT) { // the parameter is not used
	for (int k = 0; k < SPEED_UP; k += 1) {
		timeStep(); // update the world
		if (time_step > SLOW_TIME) { break; }
	}
	Invalidate(FALSE); // redraw the window on the screen
}



