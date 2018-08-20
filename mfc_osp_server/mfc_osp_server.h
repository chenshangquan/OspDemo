
// mfc_osp_server.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cmfc_osp_serverApp:
// See mfc_osp_server.cpp for the implementation of this class
//

class Cmfc_osp_serverApp : public CWinApp
{
public:
	Cmfc_osp_serverApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Cmfc_osp_serverApp theApp;