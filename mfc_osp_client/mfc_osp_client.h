
// mfc_osp_client.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cmfc_osp_clientApp:
// See mfc_osp_client.cpp for the implementation of this class
//

class Cmfc_osp_clientApp : public CWinApp
{
public:
	Cmfc_osp_clientApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Cmfc_osp_clientApp theApp;