// win32_osp_server.cpp : Defines the entry point for the application.
//


#include "stdafx.h"
#include "win32_osp_client.h"
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

// ������Ϣ����ť;
void OnBnClickedPost(CEditUI* m_pEditPost)
{
    // ��ȡ��������;
    USES_CONVERSION;
    char *strMsg = W2A((m_pEditPost->GetText()).GetData());

    // ������Ϣ��server��;
    OspPrintf(TRUE, FALSE, "client start to send a message to server: %s , length = %d\n", strMsg, strlen(strMsg));
	s32 nPostRet = OspPost(MAKEIID(DEMO_APP_SERVER_NO, CPublic::g_uInsNum), EVENT_MSG_POST, strMsg, strlen(strMsg), CPublic::g_uNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO));
}

// ������Ϣ����ť;
void OnBnClickedConnect()
{
    u32 dwIpv4Addr = 0;
    u32 dwTcpPort = 0;
    u32 dwLocalIP = 0;

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
    dwIpv4Addr = inet_addr(W2A(pStrIPAddr));
    dwTcpPort = atoi(W2A(pStrPort));

    // OSP��ʼ��;
    OspInit(TRUE, 2520);
    // OSP��ʼ�������ѯ;
    if (IsOspInitd() == TRUE)
    {
        OspPrintf(TRUE, FALSE, "OSP Init OK!!\r\n");
    }

    // �����������;

    // �����ⲿ���;
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
        OspPrintf(TRUE, FALSE, "Connect Result:SUCCESSFUL. Node Number:%d\r\n", CPublic::g_uNodeNum);
        ::MessageBox(NULL, _T("SUCCESSFUL!!"), _T("Connect Result"), NULL);

        //����APP
        s32 nCrtRet = g_cDemoApp.CreateApp("DemoClient", DEMO_APP_CLIENT_NO, DEMO_APP_PRIO, DEMO_APP_QUE_SIZE); //APPID = 2

        // �÷���˷��䵽һ�����е�instance�� ������Ϣ��������;
        s32 nPostRet = OspPost(MAKEIID(DEMO_APP_SERVER_NO, CInstance::DAEMON), EVENT_SERVER_INS_ALLOT,
            NULL, 0, CPublic::g_uNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), 0, DEMO_POST_TIMEOUT);
    }
	return;
}

// �ļ�ѡ����ť;
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
    bi.lpszTitle = _T("��ѡ���ļ�·��");  
    bi.pszDisplayName = szBuf;  
    bi.ulFlags = BIF_BROWSEINCLUDEFILES;  

    // �ڵ��ú���SHBrowseForFolder֮ǰ��Ҫ���øú�����ʼ����ػ���;
    InitCommonControlsEx(&InitCtrls);

    pil = SHBrowseForFolder(&bi);
    // ������ִ�гɹ��������û�ѡ���ʼ�·�������ȷ��;
    if (NULL != pil)
    {
        // ��ȡ�û�ѡ����ļ�·��;
        SHGetPathFromIDList(pil, szBuf);
        ZeroMemory(g_strFilePath, MAX_PATH);
        lstrcat(g_strFilePath, szBuf);

        USES_CONVERSION;
        char *buff = W2A(szBuf);
        OspPrintf(TRUE, FALSE, "Get FilePath: %s\r\n", buff);

        // ��ȡ�ļ�����;
        TCHAR *szFileName = PathFindFileName(szBuf);
        ZeroMemory(g_strFileName, MAX_FILE_NAME);
        lstrcat(g_strFileName, szBuf);

        // ����ļ���;
        char *Buff = W2A(szFileName);
        OspPrintf(TRUE, FALSE, "Get FileName: %s\r\n", Buff);
        
        // �����ļ��Ļ������Ե������;
        HANDLE mFileR = CreateFile(g_strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (mFileR == INVALID_HANDLE_VALUE)
        {
            //OutputDebugString(TEXT("CreateFile fail!\r\n"));
            OspPrintf(TRUE, FALSE, "Client: CreateFile fail!\r\n");
            return;
        }

        // ��ȡ�ļ���С�ͷְ���;
        DWORD dwHigh = 0;
        DWORD dwSize = GetFileSize(mFileR, &dwHigh);
        //__int64 nFileSize = ((__int64)dwHigh << 32) + dwSize;//���ڴ��ļ���Ҫ����32λ�͵�32λƴ�ӳ�64λ����
        m_fileInfo.fileLength = dwSize;
        m_fileInfo.filePacketNum = m_fileInfo.fileLength/(MAX_FILE_PACKET - 4 - 2*sizeof(int)) + 1;
        m_length = m_fileInfo.fileLength;
        CloseHandle(mFileR);

        // ���ͻ����ļ���Ϣ��server�ˣ����ļ������ļ���С;
        ZeroMemory(m_fileInfo.strFileName, MAX_FILE_NAME + 1);
        memcpy_s(m_fileInfo.strFileName, MAX_FILE_NAME, Buff, strlen(Buff));
        //wcstombs(m_fileInfo.strFileName, szFileName, strlen(szFileName));
        OspPrintf(TRUE, FALSE, "Start to send fileInfo, name is : %s, length : %d\n", m_fileInfo.strFileName, m_length);

        // �ļ������͵������;
        OspPost(MAKEIID(DEMO_APP_SERVER_NO, CPublic::g_uInsNum), EVENT_FILE_ATR_POST, m_fileInfo.strFileName,
            strlen(m_fileInfo.strFileName), CPublic::g_uNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO));
        ZeroMemory(strFileLen, 10);
        sprintf(strFileLen, "%d", m_fileInfo.fileLength);

        // �ļ����ȷ��͵������;
        OspPost(MAKEIID(DEMO_APP_SERVER_NO, CPublic::g_uInsNum), EVENT_FILE_LEN_POST, strFileLen,
            strlen(strFileLen), CPublic::g_uNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO));

        // �ÿͻ��˷���һ�����е�instance�����ڴ����ļ���������;
        OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_CLIENT_INS_ALLOT, NULL, 0,
            0, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), 0, DEMO_POST_TIMEOUT);

        m_fileInfo.fileStart = 0;
        m_fileInfo.fileSize = 0;
        m_fileInfo.lastStart = 0;
        m_fileInfo.lastSize = 0;
        m_fileInfo.filePacketIndex = 0;

    }
    return;
}

// �ļ����ʹ���ť;
void OnBnClickedFilePst()
{
    u16 wInsNum = 0;
    u16 wIndex = 0;
    u16 wCliPostInsNo = 0;
    u16 wSerPostInsNo = 0;

    OspPrintf(TRUE, FALSE, "start to call sendFileInfo\n");

    // ��ȡ�ͻ�������õ���instance;
    for (wIndex; wIndex < MAX_FILE_POST_INS; wIndex++)
    {
        if (g_uInsNo[wIndex].uCliInsNum != 0 && g_uInsNo[wIndex].nFlag == 0)
        {
            wCliPostInsNo = g_uInsNo[wIndex].uCliInsNum;
            g_uInsNo[wIndex].nFlag = 1;     //ʹ����1;

            // ��ʼ��;
            g_uInsNo[wIndex].nLastStart  = 0;
            g_uInsNo[wIndex].nLastSize   = 0;
            g_uInsNo[wIndex].nPktIndex   = 0;
            g_uInsNo[wIndex].nErrorIndex = 0;

			USES_CONVERSION;
			ZeroMemory(g_uInsNo[wIndex].strFilePath, MAX_PATH);
			memcpy_s(g_uInsNo[wIndex].strFilePath, MAX_PATH, g_strFilePath, strlen(W2A(g_strFilePath)));
            break;
        }
    }

    if (wIndex == MAX_FILE_POST_INS)
    {
        OspPrintf(TRUE, FALSE, "Client:Get instance index error!!\r\n");
    }

    // �������������ˣ��÷��������һ�����е�instance�������ļ��Ľ���;
    s32 nPostRet = OspPost(MAKEIID(DEMO_APP_SERVER_NO, CInstance::DAEMON), EVENT_SERVER_INS_ALLOT,
            NULL, 0, CPublic::g_uNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), 0, DEMO_POST_TIMEOUT);
	wSerPostInsNo = wCliPostInsNo;

	// ���͵�һ����;
    sendFileInfo(0, 0, "0", wCliPostInsNo, wSerPostInsNo, wIndex);

    // ��ʼ����ͣ���λ;
    g_PauseFlag = 0;
    return;
}

// �ļ���ͣ����;
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

// �ļ�����ȡ��;
void OnBnClickedFileCcl()
{
    ResumeThread(g_hTask);
    return;
}

// ������֪ͨ��Ϣ����Ӧ�û�����;
void CFrameWindowWnd::Notify(TNotifyUI& msg)
{
    char szBuff[128] = {0};
    if (msg.sType == _T("click")) 
    {
        // �˳�����;
        if (msg.pSender->GetName() == _T("CloseButton"))
        {
            Close();
        }
        // �������Ӱ���;
        if (msg.pSender->GetName() == _T("ConnectButton"))
        {
            OnBnClickedConnect();
        }
        // ��Ϣ���Ͱ���;
        if (msg.pSender->GetName() == _T("PostButton"))
        {
            OnBnClickedPost(m_pEditPost);
        }
        // �ļ�ѡ�񰴼�;
        if (msg.pSender->GetName() == _T("FileSelButton"))
        {
            OnBnClickedFileSel();
        }
        // �ļ����Ͱ���;
        if (msg.pSender->GetName() == _T("FilePstButton"))
        {
            OnBnClickedFilePst();
        }
        // �ļ�����ֹͣ;
        if (msg.pSender->GetName() == _T("FileStpButton"))
        {
            OnBnClickedFileStp();
        }
        // �ļ�����ȡ������;
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

        // ����XML����̬��������Ԫ�ز����ֽ���Ԫ��
        CDialogBuilder builder;
        CControlUI* pRoot = builder.Create(_T("OspDemoClient.xml"), (UINT)0, NULL, &m_pm);
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

    pFrame->Create(NULL, _T("OspDemoClient"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    pFrame->CenterWindow();
    pFrame->ShowWindow(true);
    CPaintManagerUI::MessageLoop();

    //delete pFrame;
    return 0;
}