// win32_osp_server.cpp : Defines the entry point for the application.
//
#pragma once

#include "stdafx.h"
#include "win32_osp_client.h"
#include "ClientInstance.h"
#include <Shlobj.h>
#include <shlwapi.h>
#include "print.h"

typedef zTemplate<CClientInstance, MAX_INS_NO> CDemoApp;
CDemoApp g_cDemoApp;

// 临时全局变量;
CFrameWindowWnd *g_pFrame = NULL;
extern vector<CClientInstance*> g_pvcFilePstInsNo;

extern TCHAR g_strFilePath[MAX_PATH];
extern TCHAR g_strFileName[MAX_FILE_NAME];

extern u32 g_dwNodeNum;

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
        PRINTMSG("Find Index Failed.\r\n");
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
	m_pEditMsg     = static_cast<CEditUI*>(m_pm.FindControl(_T("EditMsg")));
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
    m_pDoneList     = static_cast<CListUI*>(m_pm.FindControl(_T("DoneList")));
    m_pListHeader	= static_cast<CListHeaderUI*>(m_pm.FindControl(_T("LoadListHeader")));
	m_pDoneListHeader	= static_cast<CListHeaderUI*>(m_pm.FindControl(_T("DoneListHeader")));
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

// 配置消息处理按钮;
void OnBnClickedConnect()
{
    u32 dwIpv4Addr = 0;
    u32 dwTcpPort  = 0;
    u32 dwLocalIP  = 0;
    s32 nErroNo    = 0;
	s8 achAppName[MAX_STR_LEN];

    // 获取Ipv4Addr及TcpPort;
    CEditUI *pcEditIPAddr = g_pFrame->m_pEditIPAddr;
    CEditUI *pcEditPort = g_pFrame->m_pEditPort;
    if (pcEditIPAddr == NULL || pcEditPort == NULL)
    {
		PRINTMSG("Get CEditUI Failed!!\r\n");
        return;
    }

    CDuiString cStrIPAddr = pcEditIPAddr->GetText();
    CDuiString cStrPort = pcEditPort->GetText();
    if (IsIpFormatRight(cStrIPAddr.GetData()) != TRUE)
    {
		PRINTMSG("INVALID IPADDRESS!!\r\n");
        ::MessageBox(NULL, _T("INVALID IPADDRESS"), _T("Config Result"), NULL);
        return;
    }

    // Ipv4Addr及TcpPort转换为整型;
    dwIpv4Addr = inet_addr((CW2A)cStrIPAddr.GetData());
    dwTcpPort = atoi((CW2A)cStrPort.GetData());

    // OSP初始化;
    if (IsOspInitd() != TRUE)
    {
        if (!OspInit(TRUE, 2530))
        {
            PRINTMSG("OSP Initialized Failed!\r\n");
            OspSetPrompt(DEF_TELNET_NAME);
        }
    }

    // 创建监听结点;

    // 连接外部结点;
    g_dwNodeNum = OspConnectTcpNode(dwIpv4Addr, dwTcpPort, 10,
        3, 0, &dwLocalIP);

    if (INVALID_NODE == g_dwNodeNum)
    {
        PRINTMSG("Connection Result:INVALID NODE\r\n");
        ::MessageBox(NULL, _T("INVALID NODE"), _T("Connect Result"), NULL);
        return;
    }
    else
    {
        // 连接成功窗口信息提示;
        g_pFrame->m_pEditMsg->SetText(_T("Connect Successful!"));
        g_pFrame->m_pm.DoCase(_T("caseCnt"));

        PRINTMSG_TIME("Connect Result:SUCCESSFUL. Node Number:%d\r\n", g_dwNodeNum);

        // 设置断链检测参数;
        if (OspSetHBParam(g_dwNodeNum, HB_PERIOD, MAX_HB_NUM) != TRUE)
        {
            PRINTMSG("OspSetHBParam Failed!\r\n");
            ::MessageBox(NULL, _T("OspSetHBParam Failed!"), _T("OspSetHBParam Result"), NULL);
        }

        // 设置在node连接中断时需通知的appid和InstId;
        nErroNo = OspNodeDiscCBReg(g_dwNodeNum, DEMO_APP_CLIENT_NO, INS_MSG_POST_NO);
        if ( nErroNo != OSP_OK)
        {
            PRINTMSG("OspNodeDiscCBReg Failed, ErrorNo: %d!\r\n", nErroNo);
            ::MessageBox(NULL, _T("OspNodeDiscCBReg Failed!"), _T("OspNodeDiscCBReg Result"), NULL);
        }

		ZeroMemory(achAppName, MAX_STR_LEN);
		sprintf_s(achAppName, "DemoClient%d", g_dwNodeNum);
        // 创建APP, APPID = 2
        g_cDemoApp.CreateApp(achAppName, DEMO_APP_CLIENT_NO, DEMO_APP_PRIO, DEMO_APP_QUE_SIZE);

        // 客户端默认分配instance 1, 即 INS_MSG_POST_NO, 负责消息互传流程;
        // 发送第一个空消息给服务端，让服务端获取Node ID;
        OspPost(MAKEIID(DEMO_APP_SERVER_NO, INS_MSG_POST_NO), EVENT_MSG_POST, NULL, 0,
            g_dwNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), 0, DEMO_POST_TIMEOUT);
    }
    return;
}

// 断开连接处理按键;
void OnBnClickedDisConnect()
{
    if (OspDisconnectTcpNode(g_dwNodeNum))
    {
        PRINTMSG_TIME("Disconnect TCP Node Successful!\r\n");
        g_pFrame->m_pEditMsg->SetText(_T("DisConnect!!"));
        g_pFrame->m_pm.DoCase(_T("caseDisCnt"));
    }
}

// 发送消息处理按钮;
void OnBnClickedPost(CEditUI* m_pEditPost)
{
    s8 achMsgGet[MAX_POST_MSG_LEN + 1] = {0};

    // 空消息内容不发送;
    CDuiString cstrMsg = m_pEditPost->GetText();
    if (cstrMsg == _T(""))
    {
        return;
    }

    // 获取窗口内容;
    ZeroMemory(achMsgGet, MAX_POST_MSG_LEN + 1);
    memcpy_s(achMsgGet, MAX_POST_MSG_LEN, (CW2A)cstrMsg.GetData(), strlen((CW2A)cstrMsg.GetData()));

    // 客户端发送消息内容到服务端;
    if (CPrint::IsPrintCommunion())
    {
        PRINTMSG_TIME("Client start to send a message to server: %s , length = %d\n", achMsgGet, strlen(achMsgGet));
    }

	OspPost(MAKEIID(DEMO_APP_SERVER_NO, INS_MSG_POST_NO), EVENT_MSG_POST, achMsgGet, strlen(achMsgGet),
		g_dwNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), 0, DEMO_POST_TIMEOUT);
}

// 文件选择处理按钮;
void OnBnClickedFileSel()
{
    u16 wIndex = 0;
    LPITEMIDLIST pil = NULL;
    INITCOMMONCONTROLSEX InitCtrls = {0};
    TCHAR szBuf[MAX_PATH] = {0};
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
        CEditUI* pcEditFileSel = g_pFrame->m_pEditSelFile;
        if (pcEditFileSel)
        {
            pcEditFileSel->SetText(szBuf);
        }

        PRINTMSG("Get FilePath: %s\r\n", (CW2A)szBuf);

        // 获取文件名称;
        ZeroMemory(g_strFileName, MAX_FILE_NAME);
        lstrcat(g_strFileName, PathFindFileName(szBuf));

		// 文件信息收集;
		HANDLE mFileR = CreateFile(g_strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (mFileR == INVALID_HANDLE_VALUE)
		{
			PRINTMSG("Client: CreateFile fail!\r\n");
			return;
		}

		// 获取文件大小和分包数;
		DWORD dwHigh = 0;
		DWORD dwSize = GetFileSize(mFileR, &dwHigh);
        CloseHandle(mFileR);
		//__int64 nFileSize = ((__int64)dwHigh << 32) + dwSize;//对于大文件需要将高32位和低32位拼接成64位整形;
		//g_pvcFilePstInsNo[]
		tFileInfo.fileLength = dwSize;
		tFileInfo.filePacketNum = dwSize/(FILE_PACKET_LEN - 4 - 2*sizeof(s32)) + 1;
		ZeroMemory(tFileInfo.strFileName, MAX_FILE_NAME + 1);
		memcpy_s(tFileInfo.strFileName, MAX_FILE_NAME, (CW2A)g_strFileName, strlen((CW2A)g_strFileName));

        // 让客户端分配一个空闲的instance，用于处理文件发送流程;
		OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_CLIENT_FILE_POST_INS_ALLOT, &tFileInfo,
			(strlen(tFileInfo.strFileName)+ 2*sizeof(u32)), 0, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), 0, DEMO_POST_TIMEOUT);

    }

    return;
}


// 文件一键发送处理按钮;
void OnBnClickedFilePST()
{
    u16 wIndex = 0;
    TFileInfo tFileInfo = {0};
    u16 wLength = 0;

    for (wIndex = 0; wIndex < g_pvcFilePstInsNo.size(); wIndex++)
    {
		// 该控件在文件已处于传输时，不生效;
		if (g_pvcFilePstInsNo[wIndex]->m_nUsedFlag == 1)
		{
			continue;
		}

		g_pvcFilePstInsNo[wIndex]->m_nUsedFlag = 1;
        memset((s8*)&tFileInfo, 0, MAX_FILE_NAME+1+2*sizeof(u32));
        wLength = 0;
        tFileInfo.fileLength =  g_pvcFilePstInsNo[wIndex]->m_tFileInfo.fileLength;
        tFileInfo.filePacketNum =  g_pvcFilePstInsNo[wIndex]->m_tFileInfo.filePacketNum;
        ZeroMemory(tFileInfo.strFileName, MAX_FILE_NAME + 1);
        wLength = strlen(g_pvcFilePstInsNo[wIndex]->m_tFileInfo.strFileName);
        memcpy_s(tFileInfo.strFileName, MAX_FILE_NAME, g_pvcFilePstInsNo[wIndex]->m_tFileInfo.strFileName, wLength);

        // 让服务端分配一个空闲的instance，用于处理文件接收流程;
        OspPost(MAKEIID(DEMO_APP_SERVER_NO, CInstance::DAEMON), EVENT_SERVER_FILE_POST_INS_ALLOT,
            &tFileInfo, wLength + 2*sizeof(u32), g_dwNodeNum, MAKEIID(DEMO_APP_CLIENT_NO,  g_pvcFilePstInsNo[wIndex]->m_instId), 0, DEMO_POST_TIMEOUT);
    }

    // 使发送按钮无效;
    g_pFrame->m_pBtnFilePost->SetName(_T("None"));
    return;
}

// 文件一键暂停处理按钮;
void OnBnClickedFileSTP()
{
	u16 wIndex = 0;

	for (wIndex = 0; wIndex < g_pvcFilePstInsNo.size(); wIndex++)
	{
		// 文件未开始传输前，暂停按键无效;
		if (g_pvcFilePstInsNo[wIndex]->m_nUsedFlag == 0)
		{
			return;
		}

		g_pvcFilePstInsNo[wIndex]->m_nPuase = !g_pvcFilePstInsNo[wIndex]->m_nPuase;

		// 暂停取消，继续传文件;
		if (g_pvcFilePstInsNo[wIndex]->m_nPuase == 0
			&& g_pvcFilePstInsNo[wIndex]->m_dwPktIndex < g_pvcFilePstInsNo[wIndex]->m_tFileInfo.filePacketNum)
		{
			g_pvcFilePstInsNo[wIndex]->SendFileInfo(g_pvcFilePstInsNo[wIndex]->m_nLastStart,
				g_pvcFilePstInsNo[wIndex]->m_nLastSize, "OK!");
		}
	}

}

// 文件一键取消处理按钮;
void OnBnClickedFileCCL()
{
	u16 wIndex = 0;

	for (wIndex = 0; wIndex < g_pvcFilePstInsNo.size(); wIndex++)
	{
		if (g_pvcFilePstInsNo[wIndex]->m_nUsedFlag == 0)
		{
			// 文件未传输前，释放客户端用于处理文件发送流程的instance;
			OspPost(MAKEIID(DEMO_APP_CLIENT_NO, g_pvcFilePstInsNo[wIndex]->GetInsID()), EVENT_CLIENT_FILE_POST_INS_RELEASE_BF, NULL,
				0, 0, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), 0, DEMO_POST_TIMEOUT);
			return;
		}

		g_pvcFilePstInsNo[wIndex]->m_nCancel = 1;

		// 处于暂停时，点击删除;
		if (g_pvcFilePstInsNo[wIndex]->m_nPuase != 0)
		{
			g_pvcFilePstInsNo[wIndex]->m_nPuase = !g_pvcFilePstInsNo[wIndex]->m_nPuase;
			// 继续包的发送流程，触发删除包的接受并处理用户的删除操作;
			g_pvcFilePstInsNo[wIndex]->SendFileInfo(g_pvcFilePstInsNo[wIndex]->m_nLastStart,
				g_pvcFilePstInsNo[wIndex]->m_nLastSize, "OK!");
		}
	}
}

// 单文件开始发送;
void OnBnClickedFileStt(u16 wInsNo)
{
	u16 wIndex = 0;
	TFileInfo tFileInfo = {0};
	u16 wLength = 0;

	wIndex = FindIndexByInsNo(wInsNo);
	if (wIndex == MAX_INS_NO)
	{
		PRINTMSG("Index Error.\r\n");
		return;
	}

	// 该控件在文件已处于传输时，不生效;
	if (g_pvcFilePstInsNo[wIndex]->m_nUsedFlag == 1)
	{
		return;
	}
	// 文件开始传输，使用标志位置1;
	g_pvcFilePstInsNo[wIndex]->m_nUsedFlag = 1;

    // 获取待发送文件信息;
	tFileInfo.fileLength =  g_pvcFilePstInsNo[wIndex]->m_tFileInfo.fileLength;
	tFileInfo.filePacketNum =  g_pvcFilePstInsNo[wIndex]->m_tFileInfo.filePacketNum;
	ZeroMemory(tFileInfo.strFileName, MAX_FILE_NAME + 1);
	wLength = strlen(g_pvcFilePstInsNo[wIndex]->m_tFileInfo.strFileName);
	memcpy_s(tFileInfo.strFileName, MAX_FILE_NAME, g_pvcFilePstInsNo[wIndex]->m_tFileInfo.strFileName, wLength);
	
	// 让服务端分配一个空闲的instance，用于处理文件接收流程;
	OspPost(MAKEIID(DEMO_APP_SERVER_NO, CInstance::DAEMON), EVENT_SERVER_FILE_POST_INS_ALLOT,
		&tFileInfo, wLength + 2*sizeof(u32), g_dwNodeNum, MAKEIID(DEMO_APP_CLIENT_NO,  g_pvcFilePstInsNo[wIndex]->m_instId), 0, DEMO_POST_TIMEOUT);

	return;
}

// 单文件暂停发送;
void OnBnClickedFileStp(u16 wInsNo)
{
	u16 wIndex = 0;

	wIndex = FindIndexByInsNo(wInsNo);
	if (wIndex == MAX_INS_NO)
	{
		PRINTMSG("Index Error.\r\n");
        return;
    }

	// 文件未开始传输前，暂停按键无效;
	if (g_pvcFilePstInsNo[wIndex]->m_nUsedFlag == 0)
	{
		return;
	}

    g_pvcFilePstInsNo[wIndex]->m_nPuase = !g_pvcFilePstInsNo[wIndex]->m_nPuase;

	// 暂停取消，继续传文件;
	if (g_pvcFilePstInsNo[wIndex]->m_nPuase == 0
		&& g_pvcFilePstInsNo[wIndex]->m_dwPktIndex < g_pvcFilePstInsNo[wIndex]->m_tFileInfo.filePacketNum)
	{
		g_pvcFilePstInsNo[wIndex]->SendFileInfo(g_pvcFilePstInsNo[wIndex]->m_nLastStart,
			g_pvcFilePstInsNo[wIndex]->m_nLastSize, "OK!");
	}

    return;
}

// 单文件取消发送;
void OnBnClickedFileCcl(u16 wInsNo)
{
	u16 wIndex = 0;

	wIndex = FindIndexByInsNo(wInsNo);
	if (wIndex == MAX_INS_NO)
	{
		PRINTMSG("Index Error.\r\n");
		return;
	}
	
	if (g_pvcFilePstInsNo[wIndex]->m_nUsedFlag == 0)
	{
		// 文件未传输前，释放客户端用于处理文件发送流程的instance;
		OspPost(MAKEIID(DEMO_APP_CLIENT_NO, g_pvcFilePstInsNo[wIndex]->GetInsID()), EVENT_CLIENT_FILE_POST_INS_RELEASE_BF, NULL,
			0, 0, MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), 0, DEMO_POST_TIMEOUT);
		return;
	}

	g_pvcFilePstInsNo[wIndex]->m_nCancel = 1;

	// 处于暂停时，点击删除;
	if (g_pvcFilePstInsNo[wIndex]->m_nPuase != 0)
	{
		g_pvcFilePstInsNo[wIndex]->m_nPuase = !g_pvcFilePstInsNo[wIndex]->m_nPuase;
		// 继续包的发送流程，触发删除包的接受并处理用户的删除操作;
		g_pvcFilePstInsNo[wIndex]->SendFileInfo(g_pvcFilePstInsNo[wIndex]->m_nLastStart,
			g_pvcFilePstInsNo[wIndex]->m_nLastSize, "OK!");
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
        // 断开连接请求;
        if (msg.pSender->GetName() == _T("DisCntButton"))
        {
            OnBnClickedDisConnect();
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
        // 文件一键发送按键;
        if (msg.pSender->GetName() == _T("FilePstButton"))
        {
            OnBnClickedFilePST();
        }
		// 文件一键暂停按键;
		if (msg.pSender->GetName() == _T("FileStpButton"))
		{
			OnBnClickedFileSTP();
		}
		// 文件一键取消按键;
		if (msg.pSender->GetName() == _T("FileCclButton"))
		{
			OnBnClickedFileCCL();
		}

		// 文件发送开始按键;
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

    if(msg.sType == _T("selectchanged"))
    {
        CDuiString    strName     = msg.pSender->GetName();
        CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("tabTest")));

        if(strName == _T("OptionDemo1"))
            pControl->SelectItem(0);
        else if(strName == _T("OptionDemo2"))
            pControl->SelectItem(1);
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
 
    g_pFrame = new CFrameWindowWnd();
    if(g_pFrame == NULL)
    {
        return 0;
    }

    g_pFrame->Create(NULL, _T("OspDemoClient"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    g_pFrame->CenterWindow();
    g_pFrame->ShowWindow(true);
    CPaintManagerUI::MessageLoop();

    //delete pFrame;
    return 0;
}

API void help()
{
    OspPrintf(TRUE,FALSE,"\nprt nLevel:打印级别为nLevel的消息；消息级别 0-4，\n \
                         0为关闭打印\n \
                         1为打印一般提示消息，默认输入prt即为关闭打印\n \
                         2为打印通讯消息\n \
                         3为打印文件发送消息\n \
                         4为打印所有消息");
    OspPrintf(TRUE,FALSE,"\nshowdev:显示hid设备信息");
    OspPrintf(TRUE,FALSE,"\nshowsize:显示Datalist大小");
    OspPrintf(TRUE,FALSE,"\nshowver:显示版本号");
    OspPrintf(TRUE,FALSE,"\nmdver:显示媒控库版本号");
    OspPrintf(TRUE,FALSE,"\ndatastatus:显示数据是否阻塞");
    OspPrintf(TRUE,FALSE,"\ncpuadjust:是否启用CPU动态调整");
    OspPrintf(TRUE,FALSE,"\nscreen byScreen:选择屏幕,从1开始\n");
}

API void prt( u8 byLevel )
{
    CPrint::Print( byLevel );
    OspPrintf(TRUE,FALSE,"\nbyLevel:%d", byLevel);
}