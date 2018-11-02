#include "stdafx.h"
#include "print.h"
#include "semaphoreclass.h"
#include "direct.h"


static CSemaphoreClass g_tSemaphore;
static CString g_strLogFile[2];
static CString g_moduleFullPath = _T("F:\\Projects_Git\\OspDemo\\Debug\\");

#ifdef _DEBUG
	u8  CPrint::m_byPrintLevel = PRT_TYPE_NORMOL;
#else
	u8  CPrint::m_byPrintLevel = PRT_TYPE_OFF;
#endif


CPrint::CPrint()
{
	
}

CPrint::~CPrint()
{

}


void CPrint::Print( u8 byPrintLevel )
{
	m_byPrintLevel = byPrintLevel;
}

u8 CPrint::GetPrintLevel()
{
	return m_byPrintLevel;
}

bool CPrint::IsPrintCommunion()
{
    if ( m_byPrintLevel == PRT_TYPE_COMMUNION || m_byPrintLevel == PRT_TYPE_OPEN_ALL )
    {
        return true;
    }
    return false;
}

bool CPrint::IsPrintFileTR()
{
    if ( m_byPrintLevel == PRT_TYPE_FILE_TR || m_byPrintLevel == PRT_TYPE_OPEN_ALL )
    {
        return true;
    }
    return false;
}

CString GetModuleFullPath( void )
{
    TCHAR achDllName[32768] = _T("");
    CString csPath = _T("");

    MEMORY_BASIC_INFORMATION mbi;   
    static int dummy; 
    u32 dwLength = sizeof(mbi); 
    if ( dwLength == VirtualQuery( &dummy, &mbi, dwLength ) )
    {
        GetModuleFileName( reinterpret_cast<HMODULE>(mbi.AllocationBase), achDllName, _countof(achDllName) ); 
        size_t nBuffLen = GetLongPathName( achDllName, 0, 0 );
        if ( nBuffLen == _tcslen( achDllName ) )
        {
            csPath = achDllName;
        }
        else
        {
            TCHAR* pChar = new TCHAR[nBuffLen + 1];
            ZeroMemory( pChar, sizeof(TCHAR) * ( nBuffLen + 1 ) );
            GetLongPathName( achDllName, pChar, nBuffLen );
            csPath = pChar;
            delete[] pChar;
        }		
    }
    
    csPath = csPath.Left( csPath.ReverseFind(_T('\\')) + 1 );

    // 创建temp文件夹，存储日志文件;
    WIN32_FIND_DATA wfd;
    CString csTempPath = csPath + _T("temp\\");
    HANDLE hFound = FindFirstFile(csTempPath, &wfd);
    if (hFound == INVALID_HANDLE_VALUE)
    {
        _mkdir((CW2A)csTempPath);
    }

    return csPath;
}

void CPrint::WriteLog( const char *pTemp )
{
	if (g_strLogFile[0].IsEmpty())
	{
		g_strLogFile[0] = GetModuleFullPath() + LOG_FILE0;
		g_strLogFile[1] = GetModuleFullPath() + LOG_FILE1;
	}

	CAutoLock cAuto(&g_tSemaphore);
	CFile cfile;

	//打开;
	static int nFileIndex = 0;
	if (!PathFileExists(g_strLogFile[nFileIndex]))
	{
		BOOL bFlag = cfile.Open(g_strLogFile[nFileIndex], CFile::modeCreate | CFile::modeWrite);
		if (!bFlag)
		{
			return;
		}
	}
	else
	{
		CFileStatus fileStaus;
		if (CFile::GetStatus(g_strLogFile[nFileIndex], fileStaus))  
		{  
			UINT nOpenFlag = 0;
			if (fileStaus.m_size > LOG_SIZE)
			{
				//文件0满，写文件1;文件1满，写文件
				if ( 1 == nFileIndex )
				{
					nFileIndex = 0;
				}
				else
				{
					nFileIndex = 1;
				}
				
				if (!PathFileExists(g_strLogFile[nFileIndex]))
				{
					nOpenFlag = CFile::modeCreate | CFile::modeWrite;
				}
				else
				{
					if (CFile::GetStatus(g_strLogFile[nFileIndex], fileStaus))  
					{
						//满就清空，不满就继续写;
						if (fileStaus.m_size > LOG_SIZE)
						{
							nOpenFlag = CFile::modeCreate | CFile::modeWrite;
						}
						else
						{
							nOpenFlag = CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate;
						}
					}
				}
			}
			else
			{
				nOpenFlag = CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate;
			}
			
			BOOL bFlag = cfile.Open(g_strLogFile[nFileIndex], nOpenFlag);
			if (!bFlag)
			{
				return;
			}
		}  
	}

	//
	if (cfile.m_hFile != CFile::hFileNull)
	{
		cfile.SeekToEnd();
		cfile.Write(pTemp, strlen(pTemp)+1);
		cfile.Close();
	}
}

void CPrint::PrintMsg( char * format,... )
{
    va_list arg_ptr;  
    char szBuffer[BUF_SIZE] = { 0 };

    va_start(arg_ptr, format);  

    _vsnprintf ( szBuffer, sizeof (szBuffer) / sizeof (TCHAR),format, arg_ptr) ; 

    va_end(arg_ptr); 

    int n = strlen(szBuffer);

    szBuffer[n] = '\0';

    if ( PRT_TYPE_OFF != m_byPrintLevel )
    {
        OspPrintf(TRUE,FALSE,szBuffer); 
    }

    //不把通讯、文件传输的单打印写入日志;
    if (PRT_TYPE_COMMUNION != m_byPrintLevel
        && PRT_TYPE_FILE_TR != m_byPrintLevel)
    {
        WriteLog( szBuffer );
    }
}

void CPrint::PrintTimeMsg( char * format,... )
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	char achTemp[MAX_PATH] = {0};
	sprintf(achTemp, "[%d-%d-%d %d:%d:%d]", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	
	va_list arg_ptr;  
	char szBuffer[BUF_SIZE] = { 0 };

	va_start(arg_ptr, format);  

	_vsnprintf ( szBuffer, sizeof (szBuffer) / sizeof (TCHAR),format, arg_ptr) ; 

	va_end(arg_ptr); 

	int n = strlen(szBuffer);

	szBuffer[n] = '\0';

	string strTmp = achTemp;
	strTmp += szBuffer;

	if ( PRT_TYPE_OFF != m_byPrintLevel )
	{
		OspPrintf(TRUE,FALSE,strTmp.c_str()); 
	}

	//不把通讯、文件传输的单打印写入日志;
	if (PRT_TYPE_COMMUNION != m_byPrintLevel
		&& PRT_TYPE_FILE_TR != m_byPrintLevel)
	{
		WriteLog( strTmp.c_str() );
	}
}

void CPrint::PrintMsgNolog( char * format,... )
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	char achTemp[MAX_PATH] = {0};
	sprintf(achTemp, "[%d-%d-%d %d:%d:%d]", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	va_list arg_ptr;  
	char szBuffer[BUF_SIZE] = { 0 };

	va_start(arg_ptr, format);  

	_vsnprintf ( szBuffer, sizeof (szBuffer) / sizeof (TCHAR),format, arg_ptr) ; 

	va_end(arg_ptr); 

	int n = strlen(szBuffer);

	szBuffer[n] = '\0';

	string strTmp = achTemp;
	strTmp += szBuffer;

	if ( PRT_TYPE_OFF != m_byPrintLevel )
	{
		OspPrintf(TRUE,FALSE,strTmp.c_str()); 
	}
}

