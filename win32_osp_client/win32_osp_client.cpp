// win32_osp_server.cpp : Defines the entry point for the application.
//


#include "stdafx.h"
#include "win32_osp_client.h"
#include "../include/w32ospdemo.h"
#include <Shlobj.h>
#include <shlwapi.h>

//主程序中要引用atlbase.h 和 tchar.h;
CFrameWindowWnd::CFrameWindowWnd(void)
{
}

CFrameWindowWnd::~CFrameWindowWnd(void)
{
}

void CFrameWindowWnd::Init()
{
    // Edit
    m_pEditIPAddr  = static_cast<CEditUI*>(m_pm.FindControl(_T("EditIPAddr")));
    m_pEditPort    = static_cast<CEditUI*>(m_pm.FindControl(_T("EditPort")));
    m_pEditPost    = static_cast<CEditUI*>(m_pm.FindControl(_T("EditPost")));
    m_pEditRecv    = static_cast<CEditUI*>(m_pm.FindControl(_T("EditRecv")));
    m_pEditSelFile = static_cast<CEditUI*>(m_pm.FindControl(_T("EditFileSel")));
    // Button
    m_pBtnConnect     = static_cast<CButtonUI*>(m_pm.FindControl(_T("ConnectButton")));
    m_pBtnDisConnect  = static_cast<CButtonUI*>(m_pm.FindControl(_T("DisCntButton")));
    m_pBtnSelFile2Pst = static_cast<CButtonUI*>(m_pm.FindControl(_T("FileSelButton")));
    m_pBtnFilePost    = static_cast<CButtonUI*>(m_pm.FindControl(_T("FilePstButton")));
    m_pBtnFilePostStp = static_cast<CButtonUI*>(m_pm.FindControl(_T("FileStpButton")));
    m_pBtnFilePostCcl = static_cast<CButtonUI*>(m_pm.FindControl(_T("FileCclButton")));
    m_pBtnPost        = static_cast<CButtonUI*>(m_pm.FindControl(_T("PostButton")));
    m_pBtnClose       = static_cast<CButtonUI*>(m_pm.FindControl(_T("CloseButton")));
    // Progress
    m_pProgress       = static_cast<CProgressUI*>(m_pm.FindControl(_T("ProgressDemo1")));    
}

LPCTSTR CFrameWindowWnd::GetWindowClassName() const
{
    return _T("UIMainFrame");
}

UINT CFrameWindowWnd::GetClassStyle() const 
{
    return UI_CLASSSTYLE_FRAME | CS_DBLCLKS; 
}

void CFrameWindowWnd::OnFinalMessage(HWND /*hWnd*/)
{ 
    delete this; 
}

// 发送消息处理按钮;
void OnBnClickedPost(CEditUI* m_pEditPost)
{
    // 获取窗口内容;
    USES_CONVERSION;
    char *strMsg = W2A((m_pEditPost->GetText()).GetData());

    // 发送消息到server端;
    OspPrintf(TRUE, FALSE, "client start to send a message to server: %s , length = %d\n", strMsg, strlen(strMsg));
	s32 ret3 = OspPost(MAKEIID(1, CPublic::g_uInsNum), EVENT_MSG_POST, strMsg, strlen(strMsg), CPublic::g_uNodeNum, MAKEIID(2, 1));
}

// 判断IP字符串的合法性;
BOOL isIpFormatRight(LPCTSTR pIpAddr)
{
    s32 dwCount = 0;
    s32 i = 0;
    s32 dwA, dwB, dwC, dwD;

    // 检查是否只包含点和数字;
    for(i = 0; pIpAddr[i] != '\0'; i++)
    {
        if(!isdigit(pIpAddr[i]) && pIpAddr[i] != '.')
            return FALSE;
    }

    // 检查形式是否为X.X.X.X;
    for (i = 0; pIpAddr[i+1] != '\0'; i++)
    {
        if (isdigit(pIpAddr[i]) && pIpAddr[i+1] == '.')
        {
           dwCount++;
        }
    }    
    if (dwCount != 3)
    {
        return FALSE;
    }

    // 检查区间的合法性;
    if ((swscanf(pIpAddr, L"%d.%d.%d.%d", &dwA, &dwB, &dwC, &dwD) == 4)
        &&(dwA >= 0 && dwA <= 255)
        &&(dwB >= 0 && dwB <= 255)
        &&(dwC >= 0 && dwC <= 255)
        &&(dwD >= 0 && dwD <= 255))
    {
        return TRUE;
    }
    return FALSE;
}

// 配置消息处理按钮;
void OnBnClickedConnect()
{
    //u32 dwIpv4Addr = inet_addr("127.0.0.1");
    //u32 wTcpPort = 6682;

    // 获取Ipv4Addr及TcpPort;
    CEditUI *pEditIPAddr = pFrame->m_pEditIPAddr;
    CEditUI *pEditPort = pFrame->m_pEditPort;
    LPCTSTR pStrIPAddr = (pEditIPAddr->GetText()).GetData();
    LPCTSTR pStrPort = (pEditPort->GetText()).GetData();
    if (isIpFormatRight(pStrIPAddr) != TRUE)
    {
        ::MessageBox(NULL, _T("INVALID IPADDRESS"), _T("Config Result"), NULL);
        return;
    }
    USES_CONVERSION;
    u32 dwIpv4Addr = inet_addr(W2A(pStrIPAddr));
    u32 dwTcpPort = atoi(W2A(pStrPort));

    // OSP初始化
    BOOL32 BOspInit = OspInit(TRUE, 2520);
    BOOL32 BIsOspInitd = IsOspInitd();
    OspPrintf(TRUE, FALSE, "Socket Init OK!!\r\n");

    // 创建监听结点;

    // 连接外部结点;
    u32 dwLocalIP = 0;
    //OspPrintf(TRUE, FALSE, "ServerIP: %d\n", dwIpv4Addr);
    CPublic::g_uNodeNum = OspConnectTcpNode(dwIpv4Addr, dwTcpPort, 10,
        3, 0, &dwLocalIP);

    if (INVALID_NODE == CPublic::g_uNodeNum)
    {
        OspPrintf(TRUE, FALSE, "Connection Result:INVALID NODE\r\n");
        ::MessageBox(NULL, _T("INVALID NODE"), _T("Connect Result"), NULL);
        //MessageBox(_T("INVALID NODE"), _T("Connect Result"), MB_OK);
    }
    else
    {
        OspPrintf(TRUE, FALSE, "Connect Result:SUCCESSFUL. 节点号：%d\r\n", CPublic::g_uNodeNum);
        ::MessageBox(NULL, _T("SUCCESSFUL!!"), _T("Connect Result"), NULL);
        //MessageBox(nodeNumStr, _T("获得的节点号"), MB_OK);

        //创建APP
        int CrtRet = g_cDemoApp.CreateApp("DemoClient", 2, 100, 1000); //APPID = 2
        int ret3 = OspPost(MAKEIID(1, CInstance::DAEMON), EVENT_MSG_POST, "hello", 5, CPublic::g_uNodeNum, MAKEIID(2, 1), 0, 2000);

    }

    // 初始化文件传输相关全局变量;
    //ZeroMemory(CPublic::m_lastStart, MAX_THREADNO);
    //ZeroMemory(CPublic::m_lastSize, MAX_THREADNO);
    //ZeroMemory(CPublic::m_errorIndex, MAX_THREADNO);

}

// 文件选择处理按钮;
void OnBnClickedFileSel()
{
    LPITEMIDLIST pil = NULL;
    INITCOMMONCONTROLSEX InitCtrls = {0};
    TCHAR szBuf[MAX_PATH] = {0};
    BROWSEINFO bi = {0};
    u16 uPos = 0;

    bi.hwndOwner = NULL;  
    bi.iImage    = 0;  
    bi.lParam    = NULL;  
    bi.lpfn      = NULL;  
    bi.lpszTitle = _T("请选择文件路径");  
    bi.pszDisplayName = szBuf;  
    bi.ulFlags = BIF_BROWSEINCLUDEFILES;  

    // 在调用函数SHBrowseForFolder之前需要调用该函数初始化相关环境;
    InitCommonControlsEx(&InitCtrls);

    pil = SHBrowseForFolder(&bi);
    // 若函数执行成功，并且用户选择问件路径并点击确定;
    if (NULL != pil)
    {
        // 获取用户选择的文件路径;
        SHGetPathFromIDList(pil, szBuf);
        ZeroMemory(g_strFilePath, MAX_PATH);
        lstrcat(g_strFilePath, szBuf);

        USES_CONVERSION;
        char *buff = W2A(szBuf);
        OspPrintf(TRUE, FALSE, "Get FilePath: %s\r\n", buff);

        // 获取文件名称;
        TCHAR *szFileName = PathFindFileName(szBuf);
        ZeroMemory(g_strFileName, MAX_FILE_NAME);
        lstrcat(g_strFileName, szBuf);

        // 输出文件名;
        char *Buff = W2A(szFileName);
        OspPrintf(TRUE, FALSE, "Get FileName: %s\r\n", Buff);
        
        // 发送文件的基本属性到服务端;
        HANDLE mFileR = CreateFile(g_strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (mFileR == INVALID_HANDLE_VALUE)
        {
            //OutputDebugString(TEXT("CreateFile fail!\r\n"));
            OspPrintf(TRUE, FALSE, "Client: CreateFile fail!\r\n");
            return;
        }

        // 获取文件大小和分包数;
        DWORD dwHigh = 0;
        DWORD dwSize = GetFileSize(mFileR, &dwHigh);
        //__int64 nFileSize = ((__int64)dwHigh << 32) + dwSize;//对于大文件需要将高32位和低32位拼接成64位整形
        m_fileInfo.fileLength = dwSize;
        m_fileInfo.filePacketNum = m_fileInfo.fileLength/(MAX_FILE_PACKET - 4 - 2*sizeof(int)) + 1;
        m_length = m_fileInfo.fileLength;
        CloseHandle(mFileR);

        // 发送基本文件信息到server端，及文件名、文件大小;
        ZeroMemory(m_fileInfo.strFileName, MAX_FILE_NAME + 1);
        memcpy_s(m_fileInfo.strFileName, MAX_FILE_NAME, Buff, strlen(Buff));
        //wcstombs(m_fileInfo.strFileName, szFileName, strlen(szFileName));
        OspPrintf(TRUE, FALSE, "Start to send fileInfo, name is : %s, length : %d\n", m_fileInfo.strFileName, m_length);
        OspPost(MAKEIID(1, CPublic::g_uInsNum), EVENT_FILE_ATR_POST, m_fileInfo.strFileName, strlen(m_fileInfo.strFileName), CPublic::g_uNodeNum, MAKEIID(2, 1));
        ZeroMemory(strFileLen, 10);
        sprintf(strFileLen, "%d", m_fileInfo.fileLength);
        OspPost(MAKEIID(1, CPublic::g_uInsNum), EVENT_FILE_LEN_POST, strFileLen, strlen(strFileLen), CPublic::g_uNodeNum, MAKEIID(2, 1));
        CPublic::m_PktIndex = 0;
        m_fileInfo.fileStart = 0;
        m_fileInfo.fileSize = 0;
        m_fileInfo.lastStart = 0;
        m_fileInfo.lastSize = 0;
        m_fileInfo.filePacketIndex = 0;

    }
}

DWORD WINAPI TaskFunc(PVOID pvParam)
{
    sendFileInfo(0, 0, "0");
    OspTaskExit();
    return 0;
}

// 文件发送处理按钮;
void OnBnClickedFilePst()
{
    OspPrintf(TRUE, FALSE, "start to call sendFileInfo\n");
    // 初始化发送流程的状态信息;
    CPublic::m_lastStart = 0;
    CPublic::m_lastSize  = 0;
    CPublic::m_errorIndex = 0;

    u32 uTaskID;
    //func pvTaskEntry;
    //pvTaskEntry = sendFileInfo;
    //pvTaskEntry = TaskFunc;

    g_hTask = OspTaskCreate(TaskFunc, "OspSendTask", 100,
        200<<10, 0, CREATE_SUSPENDED, &uTaskID);

    g_PauseFlag = 0;
    //WaitForSingleObject(g_hTask, INFINITE);
    //CloseHandle(g_hTask);
}

// 文件暂停传输;
void OnBnClickedFileStp()
{
    BOOL32 BRet = 0;
    OspPrintf(TRUE, FALSE, "SuspendThread.\r\n\r\n\r\n");
    g_PauseFlag = !g_PauseFlag;
    Sleep(1);
#if 0
    if (g_PauseFlag != 0)
    {
        if (OspTaskSuspend(g_hTask) == -1)
        {
            OspPrintf(TRUE, FALSE, "OspTaskSuspend Failed.\r\n\r\n\r\n");
        }
        Sleep(1);
    }
    else
    {
        if (OspTaskResume(g_hTask) == -1)
        {
            OspPrintf(TRUE, FALSE, "OspTaskResume Failed.\r\n\r\n\r\n");
        }
        Sleep(1);
    }
#endif
    return;
}

// 文件发送取消;
void OnBnClickedFileCcl()
{
    ResumeThread(g_hTask);
    return;
}

// 处理窗口通知消息，相应用户输入;
void CFrameWindowWnd::Notify(TNotifyUI& msg)
{
    char szBuff[128] = {0};
    if (msg.sType == _T("click")) 
    {
        // 退出按键;
        if (msg.pSender->GetName() == _T("CloseButton"))
        {
            Close();
        }
        // 发起连接按键;
        if (msg.pSender->GetName() == _T("ConnectButton"))
        {
            OnBnClickedConnect();
        }
        // 消息发送按键;
        if (msg.pSender->GetName() == _T("PostButton"))
        {
            OnBnClickedPost(m_pEditPost);
        }
        // 文件选择按键;
        if (msg.pSender->GetName() == _T("FileSelButton"))
        {
            OnBnClickedFileSel();
        }
        // 文件发送按键;
        if (msg.pSender->GetName() == _T("FilePstButton"))
        {
            OnBnClickedFilePst();
        }
        // 文件发送停止;
        if (msg.pSender->GetName() == _T("FileStpButton"))
        {
            OnBnClickedFileStp();
        }
        // 文件发送取消按键;
        if (msg.pSender->GetName() == _T("FileCclButton"))
        {
            OnBnClickedFileCcl();
        }
    }
    if (msg.sType == _T("return"))
    {
        if (msg.pSender->GetName() == _T("EditPost"))
        {
            OnBnClickedPost(m_pEditPost);
            //m_pEditRecv->SetText((m_pEditPost->GetText()).GetData());
        }
    }
}

LRESULT CFrameWindowWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if ( uMsg == WM_CREATE ) {
        m_pm.Init(m_hWnd);

        // 加载XML，动态创建界面元素并布局界面元素
        CDialogBuilder builder;
        CControlUI* pRoot = builder.Create(_T("OspDemoClient.xml"), (UINT)0, NULL, &m_pm);
        ASSERT(pRoot && "Failed to parse XML");
        m_pm.AttachDialog(pRoot);

        // 初始化控件指针;
        Init();
        m_pm.AddNotifier(this);
        return 0;
    }
    else if ( uMsg == WM_DESTROY ) {
        ::PostQuitMessage(0);
    }

    LRESULT lRes = 0;
    if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes))
    {
        return lRes;
    }

    return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}
 
// 程序入口及Duilib初始化部分
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
 
    pFrame = new CFrameWindowWnd();
    if(pFrame == NULL)
    {
        return 0;
    }

    pFrame->Create(NULL, _T("OspDemoClient"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    pFrame->CenterWindow();
    pFrame->ShowWindow(true);
    CPaintManagerUI::MessageLoop();

    //delete pFrame;
    return 0;
}