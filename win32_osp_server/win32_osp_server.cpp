// win32_osp_server.cpp : Defines the entry point for the application.
//


#include "stdafx.h"
#include "win32_osp_server.h"
#include "win32_osp_server_com.h"
#include <Shlobj.h>
#include <shlwapi.h>

typedef zTemplate<CDemoInstance, MAX_INS_NO> CDemoApp;
CDemoApp g_cDemoApp;

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
    m_pEditIPAddr    = static_cast<CEditUI*>(m_pm.FindControl(_T("EditIPAddr")));
    m_pEditPort      = static_cast<CEditUI*>(m_pm.FindControl(_T("EditPort")));
    m_pEditPost      = static_cast<CEditUI*>(m_pm.FindControl(_T("EditPost")));
    m_pEditRecv      = static_cast<CEditUI*>(m_pm.FindControl(_T("EditRecv")));
    m_pEditFolderSel = static_cast<CEditUI*>(m_pm.FindControl(_T("EditFolderSel")));
    // Button
    m_pBtnConfig     = static_cast<CButtonUI*>(m_pm.FindControl(_T("ConfigButton")));
    m_pBtnDisConnect = static_cast<CButtonUI*>(m_pm.FindControl(_T("DisCntButton")));
    m_pBtnRecPosSel  = static_cast<CButtonUI*>(m_pm.FindControl(_T("RecvPosButton")));
    m_pBtnFileRec    = static_cast<CButtonUI*>(m_pm.FindControl(_T("FileRecvButton")));
    m_pBtnFileRecStp = static_cast<CButtonUI*>(m_pm.FindControl(_T("FileRecvStpButton")));
    m_pBtnFileRecCcl = static_cast<CButtonUI*>(m_pm.FindControl(_T("FileRecvCclButton")));
    m_pBtnPost       = static_cast<CButtonUI*>(m_pm.FindControl(_T("PostButton")));
    m_pBtnClose      = static_cast<CButtonUI*>(m_pm.FindControl(_T("CloseButton")));
    // Progress
    //m_pProgress      = static_cast<CProgressUI*>(m_pm.FindControl(_T("ProgressDemo1"))); 
    // list
    m_pList          = static_cast<CListUI*>(m_pm.FindControl(_T("LoadList")));
	m_pListHeader	 = static_cast<CListHeaderUI*>(m_pm.FindControl(_T("LoadListHeader")));
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

// 消息处理按键;
///////////////////////////////////////////////////

// 发送按钮消息处理;
void OnBnClickedPost(CEditUI* m_pEditPost)
{
    CDuiString cstrMsg = m_pEditPost->GetText();
    s8 achMsgGet[MAX_POST_MSG_LEN + 1] = {0};

    // 获取窗口内容;
    ZeroMemory(achMsgGet, MAX_POST_MSG_LEN + 1);
    memcpy_s(achMsgGet, MAX_POST_MSG_LEN, (CW2A)cstrMsg.GetData(), strlen((CW2A)cstrMsg.GetData()));

    // 客户端分配到一个空闲的instance， 负责消息互传任务;
    OspPrintf(TRUE, FALSE, "Server start to send a message to client: %s, length = %d\n", achMsgGet, strlen(achMsgGet));
    OspPost(MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), EVENT_MSG_POST, achMsgGet, strlen(achMsgGet),
        g_wNodeNum, MAKEIID(DEMO_APP_SERVER_NO, INS_MSG_POST_NO), 0, DEMO_POST_TIMEOUT);
}

// 配置按钮消息处理;
void OnBnClickedConfig()
{
    u32 dwIpv4Addr = inet_addr("127.0.0.1");
    s8 achHost[MAX_POST_MSG_LEN + 1] = {0};
    u16 wTcpPort = 0;
    in_addr tAddr = {0};
    s32 nIndex = 0;

    // 获取Ipv4Addr及TcpPort;
#if 0
    ::gethostname(achHost, MAX_POST_MSG_LEN);
    hostent *pHost = ::gethostbyname(achHost);
    /*if (pHost == NULL)
    {
        OspPrintf(TRUE, FALSE, "Get Host Content Failed!!\r\n");
        return;
    }*/

    // 获取本机第一个IP地址;
    for (nIndex = 0;;nIndex++)
    {
        s8 *pAddr = pHost->h_addr_list[nIndex];
        if (pAddr != NULL)
        {
            memcpy(&tAddr.S_un.S_addr, pAddr, pHost->h_length);
            break;
        }
    }
    
    dwIpv4Addr = inet_addr(::inet_ntoa(tAddr));
#endif
    CEditUI *pcEditPort = pFrame->m_pEditPort;
    if (pcEditPort == NULL)
    {
		OspPrintf(TRUE, FALSE, "Get CEditUI Failed!!\r\n");
        return;
    }

    CDuiString cStrPort = pcEditPort->GetText();
    wTcpPort = atoi((CW2A)cStrPort.GetData());

    // OSP初始化
    OspInit(TRUE, 2510);
    // OSP初始化结果查询
    if (IsOspInitd() == TRUE)
    {
        OspPrintf(TRUE, FALSE, "OSP Init OK!!");
    }

    // 创建一个TCP结点 //本地监听结点;
    s32 sfd = OspCreateTcpNode(dwIpv4Addr, wTcpPort);
    if (INVALID_SOCKET == sfd)
    {
        OspPrintf(TRUE, FALSE, "INVALID SOCKET\r\n");
        ::MessageBox(NULL, _T("INVALID SOCKET!!"), _T("服务器配置结果"), NULL);
        return;
    }
    else
    {
        OspPrintf(TRUE, FALSE, "服务器配置结果：SUCCESSFUL!! Socket Number:%d\r\n", sfd);
        //创建APP
        s32 nGreateRlt = g_cDemoApp.CreateApp("DemoServer", DEMO_APP_SERVER_NO, DEMO_APP_PRIO, DEMO_APP_QUE_SIZE); //APPID = 1
    }
}

// 文件存储路径选择;
void OnBnClickedFilePos()
{
#if 0
    TCHAR szFolderPath[MAX_PATH] = {0};
    u16 uPos = 0;
    u16 wIndex = 0;

    BROWSEINFO sInfo;
    ZeroMemory(&sInfo, sizeof(BROWSEINFO));
    sInfo.pidlRoot   = 0;
    sInfo.lpszTitle  = _T("请选择一个文件夹：");
    sInfo.ulFlags    = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
    sInfo.lpfn       = NULL;

    // 显示文件夹选择对话框;
    LPITEMIDLIST lpidlBrowse = SHBrowseForFolder(&sInfo);

    if (lpidlBrowse != NULL)
    {
        // 取得文件夹名;
        if (SHGetPathFromIDList(lpidlBrowse, szFolderPath))    
        {
            // 将信息写入指定instance结点;
            for (wIndex; wIndex < g_tInsNo.size(); wIndex++)
            {
                if (g_tInsNo[wIndex].uInsNum != 0 && g_tInsNo[wIndex].nUsedFlag == 0)
                {
                    USES_CONVERSION;
                    char *buff = W2A(szFolderPath);
                    OspPrintf(TRUE, FALSE, "Get FilePosPath: %s\r\n", buff);
                    OspPrintf(TRUE, FALSE, "Get FileName: %s\r\n", g_tInsNo[wIndex].m_tFileInfo.strFileName);

                    // 获取需要写入的文件路径;
                    ZeroMemory(g_strFolderPath, MAX_PATH);
                    ZeroMemory(g_strFilePath, MAX_PATH);

                    lstrcat(g_strFolderPath, szFolderPath);
                    lstrcat(g_strFilePath, szFolderPath);
                    lstrcat(g_strFilePath, L"\\");
                    lstrcat(g_strFilePath, A2W(g_tInsNo[wIndex].m_tFileInfo.strFileName));
                    char *BUFF = W2A(g_strFilePath);
                    OspPrintf(TRUE, FALSE, "Get FilePosPath & name: %s\r\n", BUFF);
                }
            }
        }
    }

    if (lpidlBrowse != NULL)
    {
        CoTaskMemFree(lpidlBrowse);
    }
#endif
    return;
}

// 处理窗口通知消息，相应用户输入;
void CFrameWindowWnd::Notify(TNotifyUI& msg)
{
    char szBuff[128] = {0};
    if (msg.sType == _T("click"))
    {
        //if(msg.pSender->GetName() == _T("closebtn"))
        if (msg.pSender->GetName() == _T("CloseButton"))
        {
            Close();
        }
        if (msg.pSender->GetName() == _T("ConfigButton"))
        {
            OnBnClickedConfig();
        }
        if (msg.pSender->GetName() == _T("PostButton"))
        {
            OnBnClickedPost(m_pEditPost);
        }
        if (msg.pSender->GetName() == _T("RecvPosButton"))
        {
            OnBnClickedFilePos();
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
        CControlUI* pRoot = builder.Create(_T("OspDemoServer.xml"), (UINT)0, NULL, &m_pm);
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

    pFrame->Create(NULL, _T("OspDemoServer"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    pFrame->CenterWindow();
    pFrame->ShowWindow(true);
    CPaintManagerUI::MessageLoop();

    return 0;
}