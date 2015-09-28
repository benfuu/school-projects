// Bugs Solution.h : main header file for the Bugs Solution application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

// CBugsSolutionApp:
// See Bugs Solution.cpp for the implementation of this class
//

class CBugsSolutionApp : public CWinApp
{
public:
	CBugsSolutionApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	//afx_msg void OnAppAbout();
	//DECLARE_MESSAGE_MAP()
};

extern CBugsSolutionApp theApp;