// win32_osp_server.cpp : Defines the entry point for the application.
//


#include "stdafx.h"
#include "win32_osp_client.h"
#include "win32_osp_client_com.h"
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
    //m_pProgress       = static_cast<CProgressUI*>(m_pm.FindControl(_T("ProgressDemo1"))); 
    // List
    m_pList         = static_cast<CListUI*>(m_pm.FindControl(_T("LoadList")));
	m_pListHeader	= static_cast<CListHeaderUI*>(m_pm.FindControl(_T("LoadListHeader")));
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

// 配置消息处理按钮;
void OnBnClickedConnect()
{
    u32 dwIpv4Addr = 0;
    u32 dwTcpPort = 0;
    u32 dwLocalIP = 0;
    s32 nNodeNum = 0;

    // 获取Ipv4Addr及TcpPort;
    CEditUI *pcEditIPAddr = pFrame->m_pEditIPAddr;
    CEditUI *pcEditPort = pFrame->m_pEditPort;
    LPCTSTR pStrIPAddr = (pcEditIPAddr->GetText()).GetData();
    LPCTSTR pStrPort = (pcEditPort->GetText()).GetData();

    if (IsIpFormatRight(pStrIPAddr) != TRUE)
    {
        ::MessageBox(NULL, _T("INVALID IPADDRESS"), _T("Config Result"), NULL);
        return;
    }

    // Ipv4Addr及TcpPort转换为整型;
    dwIpv4Addr = inet_addr(CW2A(pStrIPAddr));
    dwTcpPort = atoi(CW2A(pStrPort));

    // OSP初始化;
    OspInit(TRUE, 2520);

    // OSP初始化结果查询;
    if (IsOspInitd() != TRUE)
    {
        OspPrintf(TRUE, FALSE, "OSP Init Failed!!\r\n");
        return;
    }
    OspPrintf(TRUE, FALSE, "OSP Init OK!!\r\n");

    // 创建监听结点;

    // 连接外部结点;
    nNodeNum = OspConnectTcpNode(dwIpv4Addr, dwTcpPort, 10,
        3, 0, &dwLocalIP);

    if (INVALID_NODE == nNodeNum)
    {
        OspPrintf(TRUE, FALSE, "Connection Result:INVALID NODE\r\n");
        ::MessageBox(NULL, _T("INVALID NODE"), _T("Connect Result"), NULL);
        return;
    }
    else
    {
        OspPrintf(TRUE, FALSE, "Connect Result:SUCCESSFUL. Node Number:%d\r\n", nNodeNum);
        //::MessageBox(NULL, _T("SUCCESSFUL!!"), _T("Connect Result"), NULL);

        // 创建APP
        s32 nCrtRet = g_cDemoApp.CreateApp("DemoClient", DEMO_APP_CLIENT_NO, DEMO_APP_PRIO, DEMO_APP_QUE_SIZE); //APPID = 2

        // 客户端默认分配instance 1，负责消息互传流程;

        // 服务端分配到一个空闲的instance， 负责消息互传任务;
        s32 nPostRet = OspPost(MAKEIID(DEMO_APP_SERVER_NO, CInstance::DAEMON), EVENT_SERVER_MSG_POST_INS_ALLOT,
            NULL, 0, nNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), 0, DEMO_POST_TIMEOUT);
    }
    return;
}

// 发送消息处理按钮;
void OnBnClickedPost(CEditUI* m_pEditPost)
{
    // 获取窗口内容;
    USES_CONVERSION;
    char *strMsg = W2A((m_pEditPost->GetText()).GetData());

    // 发送消息到server端;
    OspPrintf(TRUE, FALSE, "client start to send a message to server: %s , length = %d\n", strMsg, strlen(strMsg));
	s32 nPostRet = OspPost(MAKEIID(DEMO_APP_SERVER_NO, CPublic::g_uInsNum), EVENT_MSG_POST, strMsg, strlen(strMsg), CPublic::g_uNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO));
}

// 文件选择处理按钮;
void OnBnClickedFileSel()
{
    u16 wIndex = 0;
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
        
        // 让客户端分配一个空闲的instance，用于处理文件发送流程;
        OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_CLIENT_FILE_POST_INS_ALLOT, NULL, 0,
            0, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), 0, DEMO_POST_TIMEOUT);

        CPublic::m_swTmpNum = 0;  // 便于判断是否获得服务端的回复消息;
        // 让服务端分配一个空闲的instance，用于处理文件接收流程;
        OspPost(MAKEIID(DEMO_APP_SERVER_NO, CInstance::DAEMON), EVENT_SERVER_FILE_POST_INS_ALLOT,
            NULL, 0, CPublic::g_uNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), 0, DEMO_POST_TIMEOUT);

        // 发送文件的基本属性到服务端;
        HANDLE mFileR = CreateFile(g_strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (mFileR == INVALID_HANDLE_VALUE)
        {
            OspPrintf(TRUE, FALSE, "Client: CreateFile fail!\r\n");
            return;
        }

        Sleep(1000);

        // 获取客户端申请得到的instance;
        for (wIndex = 0; wIndex < g_tInsNo.size(); wIndex++)
        {
            if (g_tInsNo[wIndex].uInsNum != 0 && g_tInsNo[wIndex].nUsedFlag == 0)
            {
                // 获取文件大小和分包数;
                DWORD dwHigh = 0;
                DWORD dwSize = GetFileSize(mFileR, &dwHigh);
                //__int64 nFileSize = ((__int64)dwHigh << 32) + dwSize;//对于大文件需要将高32位和低32位拼接成64位整形
                g_tInsNo[wIndex].m_tFileInfo.fileLength = dwSize;
                g_tInsNo[wIndex].m_tFileInfo.filePacketNum = g_tInsNo[wIndex].m_tFileInfo.fileLength/(MAX_FILE_PACKET - 4 - 2*sizeof(s32)- 3*sizeof(u16)) + 1;

                // 发送基本文件信息到server端，及文件名、文件大小;
                ZeroMemory(g_tInsNo[wIndex].m_tFileInfo.strFileName, MAX_FILE_NAME + 1);
                memcpy_s(g_tInsNo[wIndex].m_tFileInfo.strFileName, MAX_FILE_NAME, Buff, strlen(Buff));

                OspPrintf(TRUE, FALSE, "Start to send fileInfo, name is : %s, length : %d\n",
					g_tInsNo[wIndex].m_tFileInfo.strFileName, g_tInsNo[wIndex].m_tFileInfo.fileLength);

                // 文件名发送到服务端;
                OspPost(MAKEIID(DEMO_APP_SERVER_NO, CPublic::g_uInsNum), EVENT_FILE_ATR_POST, g_tInsNo[wIndex].m_tFileInfo.strFileName,
                    strlen(g_tInsNo[wIndex].m_tFileInfo.strFileName), CPublic::g_uNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO));

                ZeroMemory(strFileLen, 16);
                sprintf(strFileLen, "%d", g_tInsNo[wIndex].m_tFileInfo.fileLength);

                // 文件长度发送到服务端;
                OspPost(MAKEIID(DEMO_APP_SERVER_NO, CPublic::g_uInsNum), EVENT_FILE_LEN_POST, strFileLen,
                    strlen(strFileLen), CPublic::g_uNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO));

                // 其余成员数据初始化为0;
				g_tInsNo[wIndex].m_nPuase = 0;
				g_tInsNo[wIndex].m_nCancel = 0;
                g_tInsNo[wIndex].m_tFileInfo.fileStart = 0;
                g_tInsNo[wIndex].m_tFileInfo.fileSize = 0;
                g_tInsNo[wIndex].m_tFileInfo.lastStart = 0;
                g_tInsNo[wIndex].m_tFileInfo.lastSize = 0;
                g_tInsNo[wIndex].m_tFileInfo.filePacketIndex = 0;
                g_tInsNo[wIndex].m_tFileInfo.nFilePacketBuff = 0;
                break;
            }
        }
        CloseHandle(mFileR);

        if (wIndex == g_tInsNo.size())
        {
            OspPrintf(TRUE, FALSE, "Client: OnBnClickedFileSel, Get instance index error!!\r\n");
        }
    }
    return;
}

// 文件发送处理按钮;
void OnBnClickedFilePst()
{
    u16 wIndex = 0;
    u16 wCliPostInsNo = 0;
    u16 wSerPostInsNo = 0;

    // 获取客户端申请得到的instance, 初始化文件传输控制信息;
    for (wIndex; wIndex < g_tInsNo.size(); wIndex++)
    {
        if (g_tInsNo[wIndex].uInsNum != 0 && g_tInsNo[wIndex].nUsedFlag == 0)
        {
            wCliPostInsNo = g_tInsNo[wIndex].uInsNum;
            g_tInsNo[wIndex].nUsedFlag = 1;     //使用置1;

            // 初始化;
            g_tInsNo[wIndex].nLastStart  = 0;
            g_tInsNo[wIndex].nLastSize   = 0;
            g_tInsNo[wIndex].nPktIndex   = 0;
            g_tInsNo[wIndex].dnProgValve = 0;
            g_tInsNo[wIndex].nErrorIndex = 0;

			USES_CONVERSION;
			ZeroMemory(g_tInsNo[wIndex].strFilePath, MAX_PATH);
			lstrcpy(g_tInsNo[wIndex].strFilePath, g_strFilePath);
            OspPrintf(TRUE, FALSE, "Instance(%d) -> path is: %s\r\n", wIndex, W2A(g_tInsNo[wIndex].strFilePath));
            break;
        }
    }

    if (wIndex == g_tInsNo.size())
    {
        OspPrintf(TRUE, FALSE, "Client:OnBnClickedFilePst, Get instance index error!!\r\n");
        return;
    }

    // 等待ack;
    if (CPublic::m_swTmpNum == 0)
    {
        Sleep(1000);
    }

    if (CPublic::m_swTmpNum == 0)
    {
        OspPrintf(TRUE, FALSE, "Client: Get server instance ID failed!!\r\n", wCliPostInsNo, wSerPostInsNo, wIndex);
    }
    wSerPostInsNo = CPublic::m_swTmpNum;

	OspPrintf(TRUE, FALSE, "wCliPostInsNo:%d, wSerPostInsNo:%d, wIndex:%d\r\n", wCliPostInsNo, wSerPostInsNo, wIndex);
	// 发送第一个包;
    sendFileInfo(0, 0, "0", wCliPostInsNo, wSerPostInsNo, wIndex);

    return;
}

// 文件暂停传输;
void OnBnClickedFileStp(u16 wIndex)
{
    OspPrintf(TRUE, FALSE, "SuspendThread.\r\n\r\n\r\n");
	if (wIndex == MAX_FILE_POST_INS)
	{
		// 暂停所有;
		return;
	}

	g_tInsNo[wIndex].m_nPuase = !g_tInsNo[wIndex].m_nPuase;
    Sleep(1);
    return;
}

// 文件发送取消;
void OnBnClickedFileCcl()
{
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
        // 文件发送停止按键;
        if (msg.pSender->GetName() == _T("FileStpButton"))
        {
            OnBnClickedFileStp(MAX_FILE_POST_INS);
        }
		if (msg.pSender->GetName() == _T("FileStpButton0"))
		{
			OnBnClickedFileStp(0);
		}
		if (msg.pSender->GetName() == _T("FileStpButton1"))
		{
			OnBnClickedFileStp(1);
		}
		if (msg.pSender->GetName() == _T("FileStpButton2"))
		{
			OnBnClickedFileStp(2);
		}
		if (msg.pSender->GetName() == _T("FileStpButton3"))
		{
			OnBnClickedFileStp(3);
		}
		if (msg.pSender->GetName() == _T("FileStpButton4"))
		{
			OnBnClickedFileStp(4);
		}
        // 文件发送取消按键;
        if (msg.pSender->GetName() == _T("FileCclButton"))
        {
            OnBnClickedFileCcl();
        }
		// 
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