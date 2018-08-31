// win32_osp_server.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "win32_osp_client.h"
#include "ClientInstance.h"
#include <Shlobj.h>
#include <shlwapi.h>

typedef zTemplate<CClientInstance, MAX_INS_NO> CDemoApp;
CDemoApp g_cDemoApp;

// 临时全局变量;
CFrameWindowWnd *pFrame = NULL;
extern vector<CClientInstance*> g_pvcFilePstInsNo;

extern TCHAR g_strFilePath[MAX_PATH];
extern TCHAR g_strFileName[MAX_FILE_NAME];

extern u16 g_wNodeNum;

// 自定义其他封装函数;
//////////////////////////////////////////////

// 判断IP字符串的合法性;
BOOL IsIpFormatRight(LPCTSTR pIpAddr)
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

// 根据InsNo, 找到列表对应的索引值;
u16 FindIndexByInsNo(u16 wInsNo)
{
    u16 wIndex = 0;
    // 寻找Insid对应的索引;
    for (wIndex = 0; wIndex < g_pvcFilePstInsNo.size(); wIndex++)
    {
        if (g_pvcFilePstInsNo[wIndex]->GetInsID() == wInsNo)
        {
            break;
        }
    }

    // 获取索引失败;
    if (wIndex == g_pvcFilePstInsNo.size())
    {
        OspPrintf(TRUE, FALSE, "Find Index Failed.\r\n");
        return MAX_INS_NO;
    }

    return wIndex;
}

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
    u32 dwTcpPort  = 0;
    u32 dwLocalIP  = 0;
    //s32 nNodeNum = 0;
	s8 achAppName[MAX_STR_LEN];

    // 获取Ipv4Addr及TcpPort;
    CEditUI *pcEditIPAddr = pFrame->m_pEditIPAddr;
    CEditUI *pcEditPort = pFrame->m_pEditPort;
    if (pcEditIPAddr == NULL || pcEditPort == NULL)
    {
		OspPrintf(TRUE, FALSE, "Get CEditUI Failed!!\r\n");
        return;
    }

    CDuiString cStrIPAddr = pcEditIPAddr->GetText();
    CDuiString cStrPort = pcEditPort->GetText();
    if (IsIpFormatRight(cStrIPAddr.GetData()) != TRUE)
    {
		OspPrintf(TRUE, FALSE, "INVALID IPADDRESS!!\r\n");
        ::MessageBox(NULL, _T("INVALID IPADDRESS"), _T("Config Result"), NULL);
        return;
    }

    // Ipv4Addr及TcpPort转换为整型;
    dwIpv4Addr = inet_addr((CW2A)cStrIPAddr.GetData());
    dwTcpPort = atoi((CW2A)cStrPort.GetData());

    // OSP初始化;
    OspInit(TRUE, 2520);

    // OSP初始化结果查询;
    if (IsOspInitd() != TRUE)
    {
        OspPrintf(TRUE, FALSE, "OSP Init Failed!!\r\n");
		::MessageBox(NULL, _T("OSP Init Failed!!"), _T("OSP Initialization Results"), NULL);
        return;
    }
    OspPrintf(TRUE, FALSE, "OSP Init OK!!\r\n");

    // 创建监听结点;

    // 连接外部结点;
    g_wNodeNum = OspConnectTcpNode(dwIpv4Addr, dwTcpPort, 10,
        3, 0, &dwLocalIP);

    if (INVALID_NODE == g_wNodeNum)
    {
        OspPrintf(TRUE, FALSE, "Connection Result:INVALID NODE\r\n");
        ::MessageBox(NULL, _T("INVALID NODE"), _T("Connect Result"), NULL);
        return;
    }
    else
    {
        OspPrintf(TRUE, FALSE, "Connect Result:SUCCESSFUL. Node Number:%d\r\n", g_wNodeNum);
        //::MessageBox(NULL, _T("SUCCESSFUL!!"), _T("Connect Result"), NULL);

		ZeroMemory(achAppName, MAX_STR_LEN);
		sprintf_s(achAppName, "DemoClient%d", g_wNodeNum);
        // 创建APP, APPID = 2
        g_cDemoApp.CreateApp(achAppName, DEMO_APP_CLIENT_NO, DEMO_APP_PRIO, DEMO_APP_QUE_SIZE);

        // 客户端默认分配instance 1, 即 INS_MSG_POST_NO, 负责消息互传流程;
        // 发送第一个空消息给服务端，让服务端获取Node ID;
        OspPost(MAKEIID(DEMO_APP_SERVER_NO, INS_MSG_POST_NO), EVENT_MSG_POST, NULL, 0,
            g_wNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), 0, DEMO_POST_TIMEOUT);
    }
    return;
}

// 发送消息处理按钮;
void OnBnClickedPost(CEditUI* m_pEditPost)
{
    CDuiString cstrMsg = m_pEditPost->GetText();
    s8 achMsgGet[MAX_POST_MSG_LEN + 1] = {0};

    // 获取窗口内容;
    ZeroMemory(achMsgGet, MAX_POST_MSG_LEN + 1);
    memcpy_s(achMsgGet, MAX_POST_MSG_LEN, (CW2A)cstrMsg.GetData(), strlen((CW2A)cstrMsg.GetData()));

    // 客户端发送消息内容到服务端;
    OspPrintf(TRUE, FALSE, "Client start to send a message to server: %s , length = %d\n", achMsgGet, strlen(achMsgGet));
	OspPost(MAKEIID(DEMO_APP_SERVER_NO, INS_MSG_POST_NO), EVENT_MSG_POST, achMsgGet, strlen(achMsgGet),
		g_wNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), 0, DEMO_POST_TIMEOUT);
}

// 文件选择处理按钮;
void OnBnClickedFileSel()
{
    u16 wIndex = 0;
    LPITEMIDLIST pil = NULL;
    INITCOMMONCONTROLSEX InitCtrls = {0};
    TCHAR szBuf[MAX_PATH] = {0};
    s8 achBuff[MAX_PATH + 1] = {0};
    BROWSEINFO bi = {0};
    //u16 uPos = 0;
	TFileInfo tFileInfo = {0};
	s8 achFileInfo[MAX_POST_MSG_LEN] = {0};

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

        // 获取Edit窗口并显示选择的文件路径信息;
        CEditUI* pcEditFileSel= pFrame->m_pEditSelFile;
        if (pcEditFileSel == NULL)
        {
            OspPrintf(TRUE, FALSE, "Get CEditUI Failed\r\n");
            return;
        }
        pcEditFileSel->SetText(szBuf);

        ZeroMemory(achBuff, MAX_PATH + 1);
        memcpy_s(achBuff, MAX_PATH, (CW2A)szBuf, strlen((CW2A)(szBuf)));
        OspPrintf(TRUE, FALSE, "Get FilePath: %s\r\n", achBuff);

        // 获取文件名称;
        ZeroMemory(g_strFileName, MAX_FILE_NAME);
        lstrcat(g_strFileName, PathFindFileName(szBuf));

        // 输出文件名;
        OspPrintf(TRUE, FALSE, "Get FileName: %s\r\n", (CW2A)g_strFileName);

		// 文件信息收集;
		HANDLE mFileR = CreateFile(g_strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (mFileR == INVALID_HANDLE_VALUE)
		{
			OspPrintf(TRUE, FALSE, "Client: CreateFile fail!\r\n");
			return;
		}

		// 获取文件大小和分包数;
		DWORD dwHigh = 0;
		DWORD dwSize = GetFileSize(mFileR, &dwHigh);
        CloseHandle(mFileR);
		//__int64 nFileSize = ((__int64)dwHigh << 32) + dwSize;//对于大文件需要将高32位和低32位拼接成64位整形;
		//g_pvcFilePstInsNo[]
		tFileInfo.fileLength = dwSize;
		tFileInfo.filePacketNum = dwSize/(MAX_FILE_PACKET - 4 - 2*sizeof(s32)- 3*sizeof(u16)) + 1;
		ZeroMemory(tFileInfo.strFileName, MAX_FILE_NAME + 1);
		memcpy_s(tFileInfo.strFileName, MAX_FILE_NAME, (CW2A)g_strFileName, strlen((CW2A)g_strFileName));

        // 让客户端分配一个空闲的instance，用于处理文件发送流程;
		OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_CLIENT_FILE_POST_INS_ALLOT, &tFileInfo,
			(strlen(tFileInfo.strFileName)+ 2*sizeof(u32)), 0, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), 0, DEMO_POST_TIMEOUT);

    }

    return;
}


// 文件一键发送处理按钮;
void OnBnClickedFilePst()
{
    u16 wIndex = 0;
    TFileInfo tFileInfo = {0};
    u16 wLength = 0;

    for (wIndex = 0; wIndex < g_pvcFilePstInsNo.size(); wIndex++)
    {
        //if () //根据标志位，判断是否已传送;
        memset((s8*)&tFileInfo, 0, MAX_FILE_NAME+1+2*sizeof(u32));
        wLength = 0;
        tFileInfo.fileLength =  g_pvcFilePstInsNo[wIndex]->m_tFileInfo.fileLength;
        tFileInfo.filePacketNum =  g_pvcFilePstInsNo[wIndex]->m_tFileInfo.filePacketNum;
        ZeroMemory(tFileInfo.strFileName, MAX_FILE_NAME + 1);
        wLength = strlen(g_pvcFilePstInsNo[wIndex]->m_tFileInfo.strFileName);
        memcpy_s(tFileInfo.strFileName, MAX_FILE_NAME, g_pvcFilePstInsNo[wIndex]->m_tFileInfo.strFileName, wLength);

        // 让服务端分配一个空闲的instance，用于处理文件接收流程;
        OspPost(MAKEIID(DEMO_APP_SERVER_NO, CInstance::DAEMON), EVENT_SERVER_FILE_POST_INS_ALLOT,
            &tFileInfo, wLength + 2*sizeof(u32), g_wNodeNum, MAKEIID(DEMO_APP_CLIENT_NO,  g_pvcFilePstInsNo[wIndex]->m_instId), 0, DEMO_POST_TIMEOUT);
    }

    // 使发送按钮无效;
    pFrame->m_pBtnFilePost->SetName(_T("None"));
    return;
}

// 文件开始发送;
void OnBnClickedFileStt(u16 wInsNo)
{
	u16 wIndex = 0;
	TFileInfo tFileInfo = {0};
	u16 wLength = 0;

	wIndex = FindIndexByInsNo(wInsNo);
	if (wIndex == MAX_INS_NO)
	{
		OspPrintf(TRUE, FALSE, "Index Error.\r\n");
		return;
	}

    // 获取待发送文件信息;
	tFileInfo.fileLength =  g_pvcFilePstInsNo[wIndex]->m_tFileInfo.fileLength;
	tFileInfo.filePacketNum =  g_pvcFilePstInsNo[wIndex]->m_tFileInfo.filePacketNum;
	ZeroMemory(tFileInfo.strFileName, MAX_FILE_NAME + 1);
	wLength = strlen(g_pvcFilePstInsNo[wIndex]->m_tFileInfo.strFileName);
	memcpy_s(tFileInfo.strFileName, MAX_FILE_NAME, g_pvcFilePstInsNo[wIndex]->m_tFileInfo.strFileName, wLength);
	
	// 让服务端分配一个空闲的instance，用于处理文件接收流程;
	OspPost(MAKEIID(DEMO_APP_SERVER_NO, CInstance::DAEMON), EVENT_SERVER_FILE_POST_INS_ALLOT,
		&tFileInfo, wLength + 2*sizeof(u32), g_wNodeNum, MAKEIID(DEMO_APP_CLIENT_NO,  g_pvcFilePstInsNo[wIndex]->m_instId), 0, DEMO_POST_TIMEOUT);

	return;
}

// 文件暂停发送;
void OnBnClickedFileStp(u16 wInsNo)
{
	u16 wIndex = 0;

	wIndex = FindIndexByInsNo(wInsNo);
	if (wIndex == MAX_INS_NO)
	{
		OspPrintf(TRUE, FALSE, "Index Error.\r\n");
        return;
    }

    g_pvcFilePstInsNo[wIndex]->m_nPuase = !g_pvcFilePstInsNo[wIndex]->m_nPuase;

    return;
}

// 文件取消发送;
void OnBnClickedFileCcl(u16 wInsNo)
{
	u16 wIndex = 0;

	// 清空文件发送列表;
	/*if (wInsNo == MAX_INS_NO+1)
	{
		g_pvcFilePstInsNo.clear();
		ListUI2Paint();
		return;
	}*/

	wIndex = FindIndexByInsNo(wInsNo);
	if (wIndex == MAX_INS_NO)
	{
		OspPrintf(TRUE, FALSE, "Index Error.\r\n");
		return;
	}
	
    g_pvcFilePstInsNo[wIndex]->m_nCancel = 1;
    Sleep(1000);

    vector<CClientInstance*>::iterator itIndex;
    for (itIndex = g_pvcFilePstInsNo.begin(); itIndex != g_pvcFilePstInsNo.end(); itIndex++)
    {
        if ((*itIndex)->GetInsID() == wInsNo)
        {
            // 释放instance资源;
            (*itIndex)->m_curState = IDLE_STATE;
            (*itIndex)->m_lastState = STATE_WORK;
            g_pvcFilePstInsNo.erase(itIndex);
            // 列表重绘;
            OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_LIST_UI_PAINT, NULL,
                0, 0, MAKEIID(DEMO_APP_CLIENT_NO, wInsNo), 0, DEMO_POST_TIMEOUT);
            return;
        }
    }
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
        // 文件全部发送按键;
        if (msg.pSender->GetName() == _T("FilePstButton"))
        {
            OnBnClickedFilePst();
        }


		// 文件发送开始按键;
		/*if (msg.pSender->GetName() == _T("FileSttButton"))
		{
			OnBnClickedFileStt(MAX_INS_NO+1);
		}*/
		
		if (msg.pSender->GetName() == _T("FileSttButton2"))
		{
			OnBnClickedFileStt(2);
		}
		if (msg.pSender->GetName() == _T("FileSttButton3"))
		{
			OnBnClickedFileStt(3);
		}
		if (msg.pSender->GetName() == _T("FileSttButton4"))
		{
			OnBnClickedFileStt(4);
		}
		if (msg.pSender->GetName() == _T("FileSttButton5"))
		{
			OnBnClickedFileStt(5);
		}
		if (msg.pSender->GetName() == _T("FileSttButton6"))
		{
			OnBnClickedFileStt(6);
		}
		if (msg.pSender->GetName() == _T("FileSttButton7"))
		{
			OnBnClickedFileStt(7);
		}
		if (msg.pSender->GetName() == _T("FileSttButton8"))
		{
			OnBnClickedFileStt(8);
		}
		if (msg.pSender->GetName() == _T("FileSttButton9"))
		{
			OnBnClickedFileStt(9);
		}
		if (msg.pSender->GetName() == _T("FileSttButton10"))
		{
			OnBnClickedFileStt(10);
		}

        // 文件发送停止按键;
        /*if (msg.pSender->GetName() == _T("FileStpButton"))
        {
            OnBnClickedFileStp(MAX_INS_NO+1);
        }*/

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
		if (msg.pSender->GetName() == _T("FileStpButton5"))
		{
			OnBnClickedFileStp(5);
		}
		if (msg.pSender->GetName() == _T("FileStpButton6"))
		{
			OnBnClickedFileStp(6);
		}
		if (msg.pSender->GetName() == _T("FileStpButton7"))
		{
			OnBnClickedFileStp(7);
		}
		if (msg.pSender->GetName() == _T("FileStpButton8"))
		{
			OnBnClickedFileStp(8);
		}
		if (msg.pSender->GetName() == _T("FileStpButton9"))
		{
			OnBnClickedFileStp(9);
		}
		if (msg.pSender->GetName() == _T("FileStpButton10"))
		{
			OnBnClickedFileStp(10);
		}

        // 文件发送取消按键;
		/*if (msg.pSender->GetName() == _T("FileCclButton"))
		{
			OnBnClickedFileCcl(MAX_INS_NO+1);
		}*/

		if (msg.pSender->GetName() == _T("FileCclButton2"))
		{
			OnBnClickedFileCcl(2);
		}
		if (msg.pSender->GetName() == _T("FileCclButton3"))
		{
			OnBnClickedFileCcl(3);
		}
		if (msg.pSender->GetName() == _T("FileCclButton4"))
		{
			OnBnClickedFileCcl(4);
		}
		if (msg.pSender->GetName() == _T("FileCclButton5"))
		{
			OnBnClickedFileCcl(5);
		}
		if (msg.pSender->GetName() == _T("FileCclButton6"))
		{
			OnBnClickedFileCcl(6);
		}
		if (msg.pSender->GetName() == _T("FileCclButton7"))
		{
			OnBnClickedFileCcl(7);
		}
		if (msg.pSender->GetName() == _T("FileCclButton8"))
		{
			OnBnClickedFileCcl(8);
		}
		if (msg.pSender->GetName() == _T("FileCclButton9"))
		{
			OnBnClickedFileCcl(9);
		}
		if (msg.pSender->GetName() == _T("FileCclButton10"))
		{
			OnBnClickedFileCcl(10);
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