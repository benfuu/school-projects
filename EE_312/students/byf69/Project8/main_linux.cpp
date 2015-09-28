

#include <cstdint>
#include <chrono>
#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>

#include "Bugs.h"

// CBugsSolutionApp construction

const Fl_Color RED{FL_RED};
const Fl_Color GREEN{FL_GREEN};
const Fl_Color WHITE{FL_WHITE};

const double tick_period = 0.020; // 20 milliseconds between updates -- 50Hz

/* Global Variables for the simulation */
int world[WORLD_SIZE][WORLD_SIZE];
std::vector<Bug> bug_list;

Fl_Color bugColor(Bug* b) {
	int straight = (b->genes[0] * 255) / GENE_TOTAL;
	int right = (b->genes[1] + b->genes[2] + b->genes[3]) * 255 / GENE_TOTAL;
	int left = (b->genes[5] + b->genes[6] + b->genes[7]) * 255 / GENE_TOTAL;
	return fl_rgb_color(straight, right, left);
}


class BugFrame : public Fl_Box {
public:
	BugFrame(int X,int Y,int W,int H,const char*L=0) : Fl_Box(X,Y,W,H,L) {
		/* print title: EE312 BUGS! */
        Fl::add_timeout(tick_period, Timer_Interrupt, (void*)this);
	}

    static void Timer_Interrupt(void* param) {
        BugFrame* me = (BugFrame*) param;
    	for (int k = 0; k < SPEED_UP; k += 1) {
    		timeStep(); // update the world
    		if (time_step > SLOW_TIME) { break; }
    	}
        me->redraw();
        Fl::repeat_timeout(tick_period, Timer_Interrupt, me);
    }

    void draw(void) {
    	/* erase the world (draw white over everything) */
    	fl_rectf(this->x(), this->y(), this->w(), this->h(), WHITE);

    	/* fill in the food and bugs */
    	for (int i = 0; i < WORLD_SIZE; i += 1) {
    		for (int j = 0; j < WORLD_SIZE; j += 1) {
    			switch(world[i][j]) {
    			case EMPTY: // draw nothing
    				break;
    			case FOOD: // draw food
    				fl_rectf(this->x() + i * 3, this->y() + j * 3, 3, 3, GREEN);
    				break;
    			default: // draw bug
    				if (world[i][j] == 0) {
    					fl_rectf(this->x() + i * 3 - 4, this->y() + j * 3 - 4, 9, 9,
    							bugColor(&bug_list[world[i][j]]));
    				} else {
    					fl_rectf(this->x() + i * 3, this->y() + j * 3, 3, 3,
    							bugColor(&bug_list[world[i][j]]));
    				}
    			}
    		}
    	}

		if (time_step % 10 == 0) {
			/* update the population graph */
			/* NOTE: I don't have this working in FLTK yet */
			/* scroll the population graph up one pixel */
//    		// dc.BitBlt(3*WORLD_SIZE, 0, 200, 3 * WORLD_SIZE, &dc, 3 * WORLD_SIZE, 1, SRCCOPY);
//
//    		int32_t pop_point = bug_list.size() * 100 / (NUM_FOOD * 20) - 25;
//    		int32_t stable_point = 75;
//    		dc.FillSolidRect(3*WORLD_SIZE, 3*WORLD_SIZE, 200, 1, RGB(255, 255, 255));

			/* the text should br displayed in the window, but that's not working, I'll worry about that later */
			/* for now, I can at least printf it to the console! */
    		char buff[128];

    		sprintf(buff, "time: %8d, #bugs %4d, avg age %4d, avg gen %6d,"
    			" straight %2d%%, left %2d%%, right %2d%%, back %2d%%",
    			time_step, bug_list.size(), average_age, average_generation,
    			percent_straight, percent_left, percent_right, percent_back);
    		fl_draw(buff, this->x() + 3 * WORLD_SIZE, this->y());
    		printf("%s\n", buff);

    		//    		dc.SetPixel(3*WORLD_SIZE + pop_point, 3*WORLD_SIZE, RGB(0, 0, 0));
//    		dc.SetPixel(3*WORLD_SIZE + stable_point, 3*WORLD_SIZE, RGB(255, 0, 0));


    	}
    }

};

int main(void) {
	createWorld();

	Fl_Double_Window root_window(800, 800);
    BugFrame canvas(10, 10, root_window.w()-20, root_window.h()-20);

	root_window.end();
	root_window.show();


	return Fl::run();

}




