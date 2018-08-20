
// mfc_osp_clientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mfc_osp_client.h"
#include "mfc_osp_clientDlg.h"
#include "afxdialogex.h"
#include "ospdemo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cmfc_osp_clientDlg dialog




Cmfc_osp_clientDlg::Cmfc_osp_clientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cmfc_osp_clientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cmfc_osp_clientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Cmfc_osp_clientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, &Cmfc_osp_clientDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON4, &Cmfc_osp_clientDlg::OnBnClickedButton4)
    ON_BN_CLICKED(IDC_BUTTON2, &Cmfc_osp_clientDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON3, &Cmfc_osp_clientDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// Cmfc_osp_clientDlg message handlers

BOOL Cmfc_osp_clientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void Cmfc_osp_clientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cmfc_osp_clientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cmfc_osp_clientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Cmfc_osp_clientDlg::OnBnClickedButton1()
{
    // TODO: Add your control notification handler code here

    // 获得IPADDRESS、EDIT
    CIPAddressCtrl *pBoxOne;
    CEdit *pBoxTwo;
    pBoxOne = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1);
    pBoxTwo = (CEdit*)GetDlgItem(IDC_EDIT2);
    // 赋值;
    // pBoxOne-> SetWindowText( _T("FOO ") );

    // 取值，获取IP地址、端口号;
    BYTE nf1,nf2,nf3,nf4;
    CString IpAddrStr;
    CString PortStr;
    pBoxOne->GetAddress(nf1,nf2,nf3,nf4);
    IpAddrStr.Format(_T("%u.%u.%u.%u"),nf1,nf2,nf3,nf4);
    pBoxTwo->GetWindowText(PortStr);

    // 字符串转换为整型;
    const int lenStr = IpAddrStr.GetLength();
    ASSERT(lenStr);
    char *pStr = new char[lenStr + 1];
    memset(pStr, 0, lenStr + 1);
    wcstombs(pStr, IpAddrStr, lenStr);
    u32 dwIpv4Addr = inet_addr(pStr);
    delete [] pStr;
    u16 wTcpPort = _ttoi(PortStr);

    // OSP初始化
    //BOOL32 OspInit(TRUE, 0, NULL);
    BOOL32 BOspInit = OspInit(TRUE, 2520);
    BOOL32 BIsOspInitd = IsOspInitd();
    CString IsOspInitdStr;
    IsOspInitdStr.Format(_T("%d"), BIsOspInitd);
    MessageBox(IsOspInitdStr, _T("osp初始化结果"), MB_OK);

    // 创建监听结点;

    // 连接外部结点;
    u32 dwLocalIP = 0;
    //OspPrintf(TRUE, FALSE, "ServerIP: %d\n", dwIpv4Addr);
    CPublic::g_uNodeNum = OspConnectTcpNode(dwIpv4Addr, wTcpPort, 10,
        3, 0, &dwLocalIP);
    //OspPrintf(TRUE, FALSE, "LocalIP: %u\n", dwLocalIP);
    if (INVALID_NODE == CPublic::g_uNodeNum)
    {
        MessageBox(_T("INVALID NODE"), _T("连接到服务器结果"), MB_OK);
    }
    else
    {
        MessageBox(_T("successful!!"), _T("连接到服务器结果"), MB_OK);
        CString nodeNumStr;
        nodeNumStr.Format(_T("%d"), CPublic::g_uNodeNum);
        MessageBox(nodeNumStr, _T("获得的节点号"), MB_OK);

        //创建APP
        int CrtRet = g_cDemoApp.CreateApp("DemoClient", 2, 100, 1000); //APPID = 2
        int ret3 = OspPost(MAKEIID(1, CInstance::DAEMON), EVENT_MSG_POST, "hello", 5, CPublic::g_uNodeNum, MAKEIID(2, 1), 0, 2000);

    }
    
    //CDialogEx::OnOK();
}

void Cmfc_osp_clientDlg::OnBnClickedButton4()
{
    // TODO: Add your control notification handler code here

    // 获得EDIT
    CEdit *pBoxOne;
    pBoxOne = (CEdit*)GetDlgItem(IDC_EDIT1);

    // 取值，获取message;
    CString cPostMsgStr;
	char cPostMsg[MAX_POST_MSG_LEN + 1] = {0};
	cPostMsgStr.Empty();
	pBoxOne->GetWindowText(cPostMsgStr);
	//cPostMsgStr.ReleaseBuffer();
	ZeroMemory(cPostMsg, MAX_POST_MSG_LEN + 1);
    wcstombs(cPostMsg, cPostMsgStr, MAX_POST_MSG_LEN);
    
    // 发送消息到server端;
    OspPrintf(TRUE, FALSE, "client start to send a message to server: %s , length = %d\n", cPostMsg, strlen(cPostMsg));
	int ret3 = OspPost(MAKEIID(1, CPublic::g_uInsNum), EVENT_MSG_POST, cPostMsg, strlen(cPostMsg), CPublic::g_uNodeNum, MAKEIID(2, 1));

    //发送消息
    //int OspPost(u32 dstid, u16 event, const void* content=0, u16 length=0, u32 dstnode=0, 
      //  u32 srciid=MAKEIID(INVALID_APP, INVALID_INS), u32 srcnode=INVALID_NODE, int timeout=2000);
    //int post(u32 dstiid, u16 event, const void *content=0, u16 length=0, u32 dstnode=0);
}


void Cmfc_osp_clientDlg::OnBnClickedButton2()
{
    // 获得EDIT
    CEdit *pBoxOne;
    pBoxOne = (CEdit*)GetDlgItem(IDC_EDIT3);

#if 1 // 选择文件对话框;
    //CString strFile = _T("");  
    //CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files (*.cfg)|*.cfg|All Files (*.*)|*.*||"), NULL);  
    CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("All Files (*.*)|*.*||"), NULL);  
  
    if (dlgFile.DoModal())
    {
        g_strFilePath = dlgFile.GetPathName();
        g_strFileName = dlgFile.GetFileName();

        MessageBox(g_strFileName);
        pBoxOne->SetWindowText(g_strFilePath);
        g_strFilePath.Replace(L"\\",L"\\\\");
         
        // 发送文件的基本属性到服务端
        CFile mFile(g_strFilePath, CFile::modeRead);
        m_fileInfo.fileLength = mFile.GetLength();
        m_length = m_fileInfo.fileLength;
        mFile.Close();

        ZeroMemory(m_fileInfo.strFileName, MAX_FILE_NAME + 1);
        wcstombs(m_fileInfo.strFileName, g_strFileName, g_strFileName.GetLength());
        OspPrintf(TRUE, FALSE, "start to send file, name is : %s, length : %d\n", m_fileInfo.strFileName, m_length);
        OspPost(MAKEIID(1, CPublic::g_uInsNum), EVENT_FILE_ATR_POST, m_fileInfo.strFileName, strlen(m_fileInfo.strFileName), CPublic::g_uNodeNum, MAKEIID(2, 1));
        ZeroMemory(strFileLen, 10);
        sprintf(strFileLen, "%d", m_fileInfo.fileLength);
        OspPost(MAKEIID(1, CPublic::g_uInsNum), EVENT_FILE_LEN_POST, strFileLen, strlen(strFileLen), CPublic::g_uNodeNum, MAKEIID(2, 1));
        m_fileInfo.fileStart = 0;
        m_fileInfo.fileSize = 0;
        m_fileInfo.lastStart = 0;
        m_fileInfo.lastSize = 0;
        m_fileInfo.filePacketIndex = 0;
    }
  
#endif

}


void Cmfc_osp_clientDlg::OnBnClickedButton3()
{
    // TODO: Add your control notification handler code here
#if 0
    CString strFileTitle;
    CFileFind finder;
    BOOL bWorking = finder.FindFile(_T("C://windows//sysbkup//*.cab"));
    while (bWorking)
    {
        bWorking=finder.FindNextFile();
        strFileTitle=finder.GetFileTitle();
    }
#endif

#if 0
    char sRead[128 + 1] = {0};
    ZeroMemory(sRead, 128 + 1);
    CFile mFile(g_strFilePath, CFile::modeRead);
    g_ustrLen = mFile.GetLength();
    if (g_ustrLen >= 128)
        return;
    mFile.Read(sRead, 128);
    mFile.Close();

    CFile mFileW(_T("F:\\2\\tmp.txt"), CFile::modeWrite | CFile::modeCreate);
    mFileW.Write(sRead, g_ustrLen);
    mFileW.Flush();
    mFileW.Close();
#endif
    OspPrintf(TRUE, FALSE, "start to call sendFileInfo\n");
    CPublic::m_lastStart = 0;
    CPublic::m_lastSize  = 0;
    CPublic::m_errorIndex = 0;
    sendFileInfo(0, 0, "0");
    //OspPost(MAKEIID(1, CPublic::g_uInsNum), EVENT_FILE_POST2S, m_fileInfo.strFileName, strlen(m_fileInfo.strFileName), CPublic::g_uNodeNum, MAKEIID(2, 1));
    
}
