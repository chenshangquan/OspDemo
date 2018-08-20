// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma comment(lib,"../osplib/lib/debug/OspLib.lib") 
#pragma comment(lib,"../10-common/lib/debug/win32_2010/DuiLib_u.lib") 

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <atlbase.h>  // 重点：一定要加上，不然编译不过;

// TODO: reference additional headers your program requires here


#include "../duilib/UIlib.h"
using namespace DuiLib;