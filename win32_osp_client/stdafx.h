// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma comment(lib,"../osplib/lib/debug/OspLib.lib") 
#pragma comment(lib,"../10-common/lib/debug/win32_2010/DuiLib_u.lib") 
//#pragma comment(lib,"../DuiLib_Redrain/bin/DuiLib_d.lib")

#include "targetver.h"

#define DEF_TELNET_NAME  "win32_osp_client"  //telnetĬ����ʾ����;

#define WIN32_LEAN_AND_MEAN                  // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
//#include <windows.h>

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN                    // Exclude rarely-used stuff from Windows headers
#endif

#include <afx.h>
#include <afxwin.h>                     // MFC core and standard components
#include <afxext.h>                     // MFC extensions
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>                   // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <atlbase.h>  // important;

// TODO: reference additional headers your program requires here

#include "../duilib/UIlib.h"
using namespace DuiLib;
