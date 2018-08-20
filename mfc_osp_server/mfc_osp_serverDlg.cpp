
// mfc_osp_serverDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mfc_osp_server.h"
#include "mfc_osp_serverDlg.h"
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


// Cmfc_osp_serverDlg dialog




Cmfc_osp_serverDlg::Cmfc_osp_serverDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cmfc_osp_serverDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cmfc_osp_serverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Cmfc_osp_serverDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, &Cmfc_osp_serverDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON4, &Cmfc_osp_serverDlg::OnBnClickedButton4)
    ON_BN_CLICKED(IDC_BUTTON2, &Cmfc_osp_serverDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// Cmfc_osp_serverDlg message handlers

BOOL Cmfc_osp_serverDlg::OnInitDialog()
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

void Cmfc_osp_serverDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Cmfc_osp_serverDlg::OnPaint()
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
HCURSOR Cmfc_osp_serverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Cmfc_osp_serverDlg::OnBnClickedButton1()
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
    IpAddrStr.Format(_T("%d.%d.%d.%d"),nf1,nf2,nf3,nf4);
    pBoxTwo->GetWindowText(PortStr);
    

    //字符串转换为整型;
    const int lenStr = IpAddrStr.GetLength();
    ASSERT(lenStr);
    char *pStr = new char[lenStr + 1];
    memset(pStr, 0, lenStr + 1);
    wcstombs(pStr, IpAddrStr, lenStr);
    
    u32 dwIpAddr = inet_addr(pStr);
    delete [] pStr;
    u16 wPort = _ttoi(PortStr);

    //IpAddrStr.ReleaseBuffer();
    //PortStr.ReleaseBuffer();

    // OSP初始化
    BOOL32 BOspInit = OspInit(TRUE, 2510);
    // OSP初始化结果查询
    BOOL32 BIsOspInitd = IsOspInitd();
    CString IsOspInitdStr;
    IsOspInitdStr.Format(_T("%d"), BIsOspInitd);
    MessageBox(IsOspInitdStr, _T("osp初始化结果"), MB_OK);

    //OspPrintf(TRUE, FALSE, "test message for print.");

    // 创建一个TCP结点 //本地监听结点;
    // int sfd = OspCreateTcpNode(dwIpAddr, wPort, FALSE);
    int sfd = OspCreateTcpNode(dwIpAddr, wPort);
    if (INVALID_SOCKET == sfd)
    {
        MessageBox(_T("INVALID SOCKET"), _T("服务器配置结果"), MB_OK);
        return;
    }
    else
    {
        MessageBox(_T("successful!!"), _T("服务器配置结果"), MB_OK);
        CString sfdStr;
        sfdStr.Format(_T("%d"), sfd);
        MessageBox(sfdStr, _T("创建的socket号"), MB_OK);

        //创建APP
        int CrtRet = g_cDemoApp.CreateApp("DemoServer", 1, 100, 1000); //APPID = 1
    }
    
    //CDialogEx::OnOK();
}


void Cmfc_osp_serverDlg::OnBnClickedButton4()
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

    //CDemoInstance cDemoInst;l

    // 发送消息到client端;
    OspPrintf(TRUE, FALSE, "server start to send a message to client: %s, length = %d\n", cPostMsg, strlen(cPostMsg));
    //int ret1 = OspPost(MAKEIID(2, 2), EVENT_START_WORK, START_CMD, strlen(START_CMD));
    //int ret2 = OspPost(MAKEIID(2, 20), EVENT_TIMEOUT, START_CMD, strlen(START_CMD));
	int ret3 = OspPost(MAKEIID(2, 1), EVENT_MSG_POST, cPostMsg, strlen(cPostMsg), CPublic::g_uNodeNum, MAKEIID(1, 2), 0, 2000);
    
}

void Cmfc_osp_serverDlg::OnBnClickedButton2()
{
    // TODO: Add your control notification handler code here

    // 获得EDIT
    CEdit *pBoxOne;
    pBoxOne = (CEdit*)GetDlgItem(IDC_EDIT3);

#if 1 // 选择目录路径;
    // TODO: Add your control notification handler code here
    //char szPath[MAX_PATH];     //存放选择的目录路径;
    CString str;
    CString str2;
    WCHAR szPath[MAX_PATH-MAX_FILE_NAME];

    ZeroMemory(szPath, sizeof(szPath));     

    BROWSEINFO bi;     
    bi.hwndOwner = m_hWnd;     
    bi.pidlRoot = NULL;     
    bi.pszDisplayName = szPath;     
    bi.lpszTitle = _T("请选择需要打包的目录：");
    bi.ulFlags = 0;     
    bi.lpfn = NULL;     
    bi.lParam = 0;     
    bi.iImage = 0;

    // 弹出选择目录对话框;
    LPITEMIDLIST lp = SHBrowseForFolder(&bi);     

    if (lp && SHGetPathFromIDList(lp, szPath))     
    {
        //str.Format(_T("选择的目录为 %s"), szPath);
        str.Format(_T("%s\\"), szPath);
        MessageBox(str);

        OspPrintf(TRUE, FALSE, "server get file attribute , fileName: %s\n", m_fileInfo.strFileName);
        str2.Format(_T("%s"), CStringW(m_fileInfo.strFileName));
        strFilePath2W = str + str2;
        //pBoxOne->SetWindowText(str);
        pBoxOne->SetWindowText(strFilePath2W);
        strFilePath2W.Replace(L"\\",L"\\\\");

    } 
    else
        MessageBox(_T("无效的目录，请重新选择"));
#endif
}
