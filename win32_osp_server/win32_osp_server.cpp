// win32_osp_server.cpp : Defines the entry point for the application.
//


#include "stdafx.h"
#include "win32_osp_server.h"
#include "ServerInstance.h"
#include <Shlobj.h>
#include <shlwapi.h>

typedef zTemplate<CServerInstance, MAX_INS_NO> CDemoApp;
CDemoApp g_cDemoApp;

// ��ʱȫ�ֱ���;
CFrameWindowWnd *g_pFrame = NULL;
//extern vector<CServerInstance*> g_pvcFilePstInsNo;

extern TCHAR g_strFolderPath[MAX_PATH];

extern u32 g_dwNodeNum;

//��������Ҫ����atlbase.h �� tchar.h;
CFrameWindowWnd::CFrameWindowWnd(void)
{
}

CFrameWindowWnd::~CFrameWindowWnd(void)
{
}

void CFrameWindowWnd::Init()
{
    // Edit
	m_pEditMsg       = static_cast<CEditUI*>(m_pm.FindControl(_T("EditMsg")));
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

// ��Ϣ������;
///////////////////////////////////////////////////

// ���Ͱ�ť��Ϣ����;
void OnBnClickedPost(CEditUI* m_pEditPost)
{
    s8 achMsgGet[MAX_POST_MSG_LEN + 1] = {0};

    CDuiString cstrMsg = m_pEditPost->GetText();
    if (cstrMsg == _T(""))
    {
        return;
    }

    // ��ȡ��������;
    ZeroMemory(achMsgGet, MAX_POST_MSG_LEN + 1);
    memcpy_s(achMsgGet, MAX_POST_MSG_LEN, (CW2A)cstrMsg.GetData(), strlen((CW2A)cstrMsg.GetData()));

    // ����˷�����Ϣ���ݵ��ͻ���;
    OspPrintf(TRUE, FALSE, "Server start to send a message to client: %s, length = %d\n", achMsgGet, strlen(achMsgGet));
    OspPost(MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), EVENT_MSG_POST, achMsgGet, strlen(achMsgGet),
        g_dwNodeNum, MAKEIID(DEMO_APP_SERVER_NO, INS_MSG_POST_NO), 0, DEMO_POST_TIMEOUT);
}

// ���ð�ť��Ϣ����;
void OnBnClickedConfig()
{
    WSADATA wsaData;
    u32 dwIpv4Addr = inet_addr("127.0.0.1");
    s8 achHost[MAX_POST_MSG_LEN + 1] = {0};
    u16 wTcpPort = 0;
    in_addr tAddr = {0};
    s32 nIndex = 0;

    // ��ȡIpv4Addr��TcpPort;
    // ����Winsock��;
    if (::WSAStartup( MAKEWORD(2,0), &wsaData ))
    {
        OspPrintf(TRUE, FALSE, "WSAStartup Failed!!\r\n");
        return;
    }

    ::gethostname(achHost, MAX_POST_MSG_LEN);
    hostent *pHost = ::gethostbyname(achHost);
    if (pHost == NULL)
    {
        OspPrintf(TRUE, FALSE, "Get Host Content Failed!!\r\n");
        return;
    }

    // ��ȡ������һ��IP��ַ;
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
    ::WSACleanup();

    // ��ȡ�û�ָ����TCP �˿ں�;
    CEditUI *pcEditPort = g_pFrame->m_pEditPort;
    if (pcEditPort)
    {
        CDuiString cStrPort = pcEditPort->GetText();
        wTcpPort = atoi((CW2A)cStrPort.GetData());
    }

    // OSP��ʼ��
    OspInit(TRUE, 2510);
    // OSP��ʼ�������ѯ
    if (IsOspInitd() == FALSE)
    {
        OspPrintf(TRUE, FALSE, "OSP Init Failed!!");
    }

    // ����һ��TCP��� //���ؼ������;
    s32 sfd = OspCreateTcpNode(dwIpv4Addr, wTcpPort);
    if (INVALID_SOCKET == sfd)
    {
        OspPrintf(TRUE, FALSE, "INVALID SOCKET\r\n");
        ::MessageBox(NULL, _T("INVALID SOCKET!!"), _T("���������ý��"), NULL);
        return;
    }
    else
    {
		g_pFrame->m_pEditMsg->SetText(_T("OSP Init OK!"));
        OspPrintf(TRUE, FALSE, "���������ý����SUCCESSFUL!! Socket Number:%d\r\n", sfd);
        //����APP
        s32 nGreateRlt = g_cDemoApp.CreateApp("DemoServer", DEMO_APP_SERVER_NO, DEMO_APP_PRIO, DEMO_APP_QUE_SIZE); //APPID = 1
    }
}

// �Ͽ����Ӵ�����;
void OnBnClickedDisConnect()
{
	if (OspDisconnectTcpNode(g_dwNodeNum) == true)
	{
		OspPrintf(TRUE, FALSE, "Disconnect TCP Node Successful!\r\n");
        g_pFrame->m_pEditMsg->SetText(_T("DisConnect!!"));
	}
}

// �ļ��洢·��ѡ��;
void OnBnClickedFilePos()
{
    TCHAR szFolderPath[MAX_PATH] = {0};
    u16 uPos = 0;
    u16 wIndex = 0;

    BROWSEINFO sInfo;
    ZeroMemory(&sInfo, sizeof(BROWSEINFO));
    sInfo.pidlRoot   = 0;
    sInfo.lpszTitle  = _T("��ѡ��һ���ļ��У�");
    sInfo.ulFlags    = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
    sInfo.lpfn       = NULL;

    // ��ʾ�ļ���ѡ��Ի���;
    LPITEMIDLIST lpidlBrowse = SHBrowseForFolder(&sInfo);

    if (lpidlBrowse != NULL)
    {
        // ȡ���ļ�����;
        if (SHGetPathFromIDList(lpidlBrowse, szFolderPath))    
        {
            // ��ȡ��Ҫд����ļ�·��;
            ZeroMemory(g_strFolderPath, MAX_PATH);
            lstrcat(g_strFolderPath, szFolderPath);
        }

        // ���ڷ���ѡ�е��ļ�·����Ϣ;
        CEditUI *pcEditPort = g_pFrame->m_pEditFolderSel;
        if (pcEditPort)
        {
            pcEditPort->SetText(g_strFolderPath);
        }
    }

    if (lpidlBrowse != NULL)
    {
        CoTaskMemFree(lpidlBrowse);
    }

    return;
}

// ������֪ͨ��Ϣ����Ӧ�û�����;
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
		if (msg.pSender->GetName() == _T("DisCntButton"))
		{
			OnBnClickedDisConnect();
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

        // ����XML����̬��������Ԫ�ز����ֽ���Ԫ��
        CDialogBuilder builder;
        CControlUI* pRoot = builder.Create(_T("OspDemoServer.xml"), (UINT)0, NULL, &m_pm);
        ASSERT(pRoot && "Failed to parse XML");
        m_pm.AttachDialog(pRoot);

        // ��ʼ���ؼ�ָ��;
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
 
// ������ڼ�Duilib��ʼ������
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());
 
    g_pFrame = new CFrameWindowWnd();
    if(g_pFrame == NULL)
    {
        return 0;
    }

    g_pFrame->Create(NULL, _T("OspDemoServer"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    g_pFrame->CenterWindow();
    g_pFrame->ShowWindow(true);
    CPaintManagerUI::MessageLoop();

    return 0;
}