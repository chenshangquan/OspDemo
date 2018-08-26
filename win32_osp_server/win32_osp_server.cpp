// win32_osp_server.cpp : Defines the entry point for the application.
//


#include "stdafx.h"
#include "win32_osp_server.h"
#include "../include/w32ospdemo.h"
#include <Shlobj.h>
#include <shlwapi.h>

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
    // ���ʹ�������;
    //char strMsg[MAX_POST_MSG_LEN + 1];
    //ZeroMemory(strMsg, MAX_POST_MSG_LEN + 1);
    USES_CONVERSION;
    char *strMsg = W2A((m_pEditPost->GetText()).GetData());

    // ��ȡ��������;
    OspPrintf(TRUE, FALSE, "server start to send a message to client: %s, length = %d\n", strMsg, strlen(strMsg));
    OspPost(MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), EVENT_MSG_POST, strMsg, strlen(strMsg), CPublic::g_uNodeNum, MAKEIID(DEMO_APP_SERVER_NO, CPublic::g_uInsNum));

}

// ���ð�ť��Ϣ����;
void OnBnClickedConfig()
{
    u32 dwIpv4Addr = inet_addr("127.0.0.1");
    u16 wTcpPort = 6682;
#if 0 //ֱ�ӱ���IP��ַ;
    // ��ȡIpv4Addr��TcpPort;
    CEditUI *pEditIPAddr = pFrame->m_pEditIPAddr;
    CEditUI *pEditPort = pFrame->m_pEditPort;
    LPCTSTR pStrIPAddr = (pEditIPAddr->GetText()).GetData();
    LPCTSTR pStrPort = (pEditPort->GetText()).GetData();

    if (IsIpFormatRight(pStrIPAddr) != TRUE)
    {
        ::MessageBox(NULL, _T("INVALID IPADDRESS"), _T("Config Result"), NULL);
        return;
    }
    USES_CONVERSION;
    u32 uIpv4Addr = inet_addr(W2A(pStrIPAddr));
    u32 uTcpPort = atoi(W2A(pStrPort));
#endif
    // OSP��ʼ��
    OspInit(TRUE, 2510);
    // OSP��ʼ�������ѯ
    if (IsOspInitd() == TRUE)
    {
        OspPrintf(TRUE, FALSE, "OSP Init OK!!");
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
        OspPrintf(TRUE, FALSE, "���������ý����SUCCESSFUL!! Socket Number:%d\r\n", sfd);
        ::MessageBox(NULL, _T("SUCCESSFUL!!"), _T("���������ý��"), NULL);
        //����APP
        s32 nGreateRlt = g_cDemoApp.CreateApp("DemoServer", DEMO_APP_SERVER_NO, DEMO_APP_PRIO, DEMO_APP_QUE_SIZE); //APPID = 1
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
            // ����Ϣд��ָ��instance���;
            for (wIndex; wIndex < g_tInsNo.size(); wIndex++)
            {
                if (g_tInsNo[wIndex].uInsNum != 0 && g_tInsNo[wIndex].nUsedFlag == 0)
                {
                    USES_CONVERSION;
                    char *buff = W2A(szFolderPath);
                    OspPrintf(TRUE, FALSE, "Get FilePosPath: %s\r\n", buff);
                    OspPrintf(TRUE, FALSE, "Get FileName: %s\r\n", g_tInsNo[wIndex].m_tFileInfo.strFileName);

                    // ��ȡ��Ҫд����ļ�·��;
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