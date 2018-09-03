#include "StdAfx.h"
#include "ClientInstance.h"
#include "win32_osp_client.h"

/****************************************************
 *
 * CClientInstance 函数定义;
 *
 ****************************************************/
vector<CClientInstance*> g_pvcFilePstInsNo;

TCHAR g_strFilePath[MAX_PATH] = _T("");
TCHAR g_strFileName[MAX_FILE_NAME] = _T("");
TCHAR g_strFolderPath[MAX_PATH] = _T("E:\\2");

u32 g_dwNodeNum;

extern CFrameWindowWnd *pFrame;
extern BOOL IsIpFormatRight(LPCTSTR pIpAddr);
extern u16 FindIndexByInsNo(u16 wInsNo);

CClientInstance::CClientInstance(void)
{
}

CClientInstance::~CClientInstance(void)
{
}

// InstanceEntry:event 消息处理函数定义;
////////////////////////////////////////

// 包发送处理函数;
void CClientInstance::SendFileInfo(s32 fStart,s32 fSize,char *fHead)
{
    BYTE m_sendBuffer[MAX_FILE_PACKET + 1] = {0};   // 待传送数据buffer;
	DWORD nByte;

    // 文件传输进度数值;
    m_dnProgValve = 100 * m_dwPktIndex / m_tFileInfo.filePacketNum;

    u32 dwFilePacketBuff = MAX_FILE_PACKET - 4 - 2*sizeof(s32);

    TFileMessage strFileMsg;
    // 定义用于发送文件数据的文件包，fileHead定义为“FFF”;
    strFileMsg.fileHead[0] = 'F';
    strFileMsg.fileHead[1] = 'F';
    strFileMsg.fileHead[2] = 'F';
    strFileMsg.fileHead[3] = '\0';

    // 初始化;
    strFileMsg.fileStart   = 0;
    strFileMsg.fileSize    = 0;
	ZeroMemory(m_sendBuffer, sizeof(BYTE)*(MAX_FILE_PACKET) + 1);
	ZeroMemory(strFileMsg.filePacket, sizeof(BYTE)*(MAX_FILE_PACKET) - 4 - 2*sizeof(s32));

	// 暂停发送;  // 保留当前发包位置;
	if (m_nPuase != 0)
	{
		return;
	}

    // 是否基于上次分包的偏移位置;
    if (fStart == m_nLastStart && fSize == m_nLastSize)
    {
        // 最后一个包发送完成;
        if (!strcmp(fHead, "END"))
        {
            OspPrintf(TRUE, FALSE, "Enter END.\r\n");
			//FindClose(m_hFile);
			CloseHandle(m_hFile);

			// 释放资源并重新绘图;
			CliFilePostInsRelease(m_instId);

			// 传送完毕时，给自己发送消息并进行进度条的绘制;
			/*OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_LAST_PROGRESS_UI_PAINT, NULL,
				0, 0, MAKEIID(DEMO_APP_CLIENT_NO, m_instId), 0, DEMO_POST_TIMEOUT);*/

            return;
        }
        // 发送第一个包;
        else if (fStart == 0 && fSize == 0 && !strcmp(fHead,"0"))
        {
            // 传输出错标志置0、包索引置1;
            m_nErrorPktNum = 0;
            m_dwPktIndex   = 1;

            // 考虑只有一个包的情况;
            if ( m_tFileInfo.fileLength <= dwFilePacketBuff)
            {
                strFileMsg.fileStart = 0;
                strFileMsg.fileSize  = m_tFileInfo.fileLength;
                m_nLastStart = 0;
                m_nLastSize  = m_tFileInfo.fileLength;
                OspPrintf(TRUE, FALSE, "Only one packet. Index(%d)\r\n", m_dwPktIndex);
            }
            else
            {
                strFileMsg.fileStart = 0;
                strFileMsg.fileSize  = dwFilePacketBuff;
                m_nLastStart = 0;
                m_nLastSize  = dwFilePacketBuff;
                OspPrintf(TRUE, FALSE, "First packet. Index(%d)\r\n", m_dwPktIndex);
            }

			// 发送第一个包时，给自己发送消息并进行进度条的绘制;
			OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_FIRST_PROGRESS_UI_PAINT, NULL,
				0, 0, MAKEIID(DEMO_APP_CLIENT_NO, m_instId), 0, DEMO_POST_TIMEOUT);
        }
        // 接收到server端回复的正常确认包;
        else if (!strcmp(fHead,"OK!"))
        {
            // 传输出错标志置0;
            m_nErrorPktNum = 0;
            // 包索引进行累加;
            m_dwPktIndex++;

            // 当要发送最后一个包的时候;
            if ((m_tFileInfo.fileLength - fStart - dwFilePacketBuff) <= dwFilePacketBuff)
            {
                strFileMsg.fileStart = fStart + dwFilePacketBuff;
                //strFileMsg.fileSize  = g_pvcFilePstInsNo[wIndex].m_tFileInfo.fileLength % nFilePacketBuff;
                strFileMsg.fileSize  = m_tFileInfo.fileLength - strFileMsg.fileStart;
                m_nLastStart = strFileMsg.fileStart;
                m_nLastSize  = strFileMsg.fileSize;
                OspPrintf(TRUE, FALSE, "Last packet. Index(%d)\r\n",  m_dwPktIndex);
            }
            else
            {
                strFileMsg.fileStart = fStart + dwFilePacketBuff;
                strFileMsg.fileSize  = dwFilePacketBuff;
                m_nLastStart = strFileMsg.fileStart;
                m_nLastSize  = strFileMsg.fileSize;
                OspPrintf(TRUE, FALSE, "Next packet. Index(%d)\r\n",  m_dwPktIndex);
            }

			// 后续正常包传送时，给自己发送消息并进行进度条的绘制;
			OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_NORMAL_PROGRESS_UI_PAINT, NULL,
				0, 0, MAKEIID(DEMO_APP_CLIENT_NO, m_instId), 0, DEMO_POST_TIMEOUT);
        }
        // 确认包表示接收到的包有错误，则重新发送上一个包;
        else if (!strcmp(fHead, "ERR"))   
        {
            OspPrintf(TRUE, FALSE, "Error packet. Index(%d)\r\n",  m_dwPktIndex);
            m_nErrorPktNum++;
            strFileMsg.fileStart = m_nLastStart;
            strFileMsg.fileSize  = m_nLastSize;
        }
        // 确认包表示用户取消了文件传输;
        else if (!strcmp(fHead, "CCL"))
        {
            OspPrintf(TRUE, FALSE, "Cancel the packet send.\r\n");
			// 释放句柄;
			//FindClose(m_hFile);
			CloseHandle(m_hFile);
            
			// 释放资源并重新绘图;
			CliFilePostInsRelease(m_instId);

            return;
        }
        // 暂停包表示用户停止文件传送;
        else if (!strcmp(fHead, "STP"))
        {
            OspPrintf(TRUE, FALSE, "Stop the packet send.\r\n\r\n\r\n");
            Sleep(1);
        }
        // 接收到的信息不正常时默认出错，重新发送上一个包;
        else
        {
            OspPrintf(TRUE, FALSE, "Repeat the previous packet. Index(%d)\r\n",  m_dwPktIndex);
            m_nErrorPktNum++;
            strFileMsg.fileStart = m_nLastStart;
            strFileMsg.fileSize  = m_nLastSize;
        }
    }
    // 接收到的信息不正常时默认出错，重新发送上一个包;
    else 
    {
        OspPrintf(TRUE, FALSE, "Other error, Repeat the previous packet. Index(%d)\r\n", m_dwPktIndex);
        m_nErrorPktNum++;
        strFileMsg.fileStart = m_nLastStart;
        strFileMsg.fileSize  = m_nLastSize;
    }

    // 这里可以进行出错处理;
    if (m_nErrorPktNum > 10)       
    {
        strFileMsg.fileHead[0] = 'E';
        strFileMsg.fileHead[1] = 'R';
        strFileMsg.fileHead[2] = 'R';
        strFileMsg.fileHead[3] = '\0';
    }

	// 取消包发送;  // 作下一包发送;
	if (m_nCancel == 1)
	{
		strFileMsg.fileHead[0] = 'C';
		strFileMsg.fileHead[1] = 'C';
		strFileMsg.fileHead[2] = 'L';
		strFileMsg.fileHead[3] = '\0';

		//BYTE* filePacket = (BYTE*)&strFileMsg;
		// 将待发送包的内容填充至发送缓存区;
		memcpy(m_sendBuffer, (BYTE*)&strFileMsg, strFileMsg.fileSize + 4 + 2*sizeof(s32));

		// 发送取消包;
		OspPost(MAKEIID(DEMO_APP_SERVER_NO, m_wSerInsNum), EVENT_FILE_POST2S, m_sendBuffer,
			(strFileMsg.fileSize + 4 + 2*sizeof(s32)), g_dwNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, m_instId));
	}

    // 读取一个包的内容;
    SetFilePointer(m_hFile, strFileMsg.fileStart, NULL, FILE_BEGIN);
    ReadFile(m_hFile, strFileMsg.filePacket, strFileMsg.fileSize, &nByte, NULL);

    BYTE* filePacket = (BYTE*)&strFileMsg;
    // 将待发送包的内容填充至发送缓存区;
    memcpy(m_sendBuffer, filePacket, strFileMsg.fileSize + 4 + 2*sizeof(s32));

    OspPrintf(TRUE, FALSE, "Start to post the file message to server. SerInsNo:%d\r\n", m_wSerInsNum);

    // 发送包到服务端;
    OspPost(MAKEIID(DEMO_APP_SERVER_NO, m_wSerInsNum), EVENT_FILE_POST2S, m_sendBuffer,
        strFileMsg.fileSize + 4 + 2*sizeof(s32), g_dwNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, m_instId));
}

// 消息接收处理函数;
void MsgPostFunc(CMessage *const pMsg)
{
    u16 wMsgLen = 0;
    s8 *pchMsgGet = NULL;

    // 消息内容提取;
    wMsgLen = pMsg->length;

    pchMsgGet = new char[wMsgLen + 1];
    ZeroMemory(pchMsgGet, wMsgLen + 1);
    memcpy_s(pchMsgGet, wMsgLen, pMsg->content, wMsgLen);
    OspPrintf(TRUE, FALSE, "message content is: %s, length is: %d.\n", pchMsgGet, wMsgLen);

	if (pMsg->event == OSP_DISCONNECT)
	{
		return;
	}

    // 窗口赋值;
    pFrame->m_pEditRecv->SetText(CA2W(pchMsgGet));

    // 释放空间;
    delete [] pchMsgGet;
    pchMsgGet = NULL;
}

// 断链消息处理;
void OspDisconnect(CMessage *const pMsg)
{
    // 窗口赋值;
    OspPrintf(TRUE, FALSE, "DisConnect by app!!\r\n");
    pFrame->m_pEditMsg->SetText(_T("DisConnect!!"));
}

// 申请服务端分配文件发送instance回复;
void CClientInstance::ClientFilePostInsAllotAck(CMessage *const pMsg)
{

	m_wSerInsNum = GETINS(pMsg->srcid);

    OspPrintf(TRUE, FALSE, "Client: wCliPostInsNo: %d, wSerPostInsNo: %d.\r\n", GETINS(pMsg->dstid), GETINS(pMsg->srcid));

    // 发送第一个包;
	// 创建文件句柄，打开文件;
	m_hFile = CreateFile(m_strFilePath,	GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		DWORD dw = GetLastError();
		OspPrintf(TRUE, FALSE, "Client: CreateFile Failed!, Error: %d.\r\n", dw);
		return;
	}
	
	// 发送第一个包;
    SendFileInfo(0, 0, "0");
    return;
}

// 处理的server端的回复包，决定下一次的文件发送;
void CClientInstance::OnClientReceive(CMessage *const pMsg)
{
	OspPrintf(TRUE, FALSE, "Client: Receive the response package\r\n");

    // 获取收到的消息内容;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];
    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                

    // 解析包数据;
    s8 achFileHead[4];
    TFileMessage *strFileMsg = (TFileMessage*)strMsgGet;
    achFileHead[0] = strFileMsg->fileHead[0];
    achFileHead[1] = strFileMsg->fileHead[1];                                                                                                                                                                                                                                                                     
    achFileHead[2] = strFileMsg->fileHead[2];
    achFileHead[3] = '\0';
    s32 nFileStart = strFileMsg->fileStart;
    s32 nFileSize  = strFileMsg->fileSize;

    // 提取消息发送处理;
    SendFileInfo(nFileStart, nFileSize, achFileHead);

    delete [] strMsgGet;
    strMsgGet = NULL;
}

// InstanceEntry消息类型分发处理入口;
void CClientInstance::InstanceEntry(CMessage *const pMsg)
{
    /*得到当前消息的类型;*/
    u16 wCurEvent = pMsg->event;

    // 根据不同的消息类型进行处理;
    switch (wCurEvent)
    {
    case OSP_DISCONNECT:
        OspDisconnect(pMsg);
        //NextState(IDLE_STATE);
    case EVENT_MSG_POST:
        MsgPostFunc(pMsg);
        //NextState(STATE_WORK);
        //SetTimer(EVENT_TIMEOUT, 1000);
        break;
	case EVENT_SERVER_FILE_POST_INS_ALLOT_ACK:
		ClientFilePostInsAllotAck(pMsg);
        //NextState(STATE_WORK);
		break;
    case EVENT_FILE_POST2C:
        OnClientReceive(pMsg);
        //NextState(STATE_WORK);
        break;
    default:
        
        break;
    }

    return;
}


// DaemonInstanceEntry:event 消息处理函数定义;
//////////////////////////////////////////////

// 获取空闲的instance指针;
CClientInstance* GetIdleInsID(CApp* pcApp)
{
    u16 wIndex = 0;
    CClientInstance *pCInst = NULL;

    // 遍历所有instance，寻找到一个空闲的instance并返回;
    for (wIndex = 2; wIndex <= MAX_INS_NO; wIndex++)
    {
        pCInst = (CClientInstance *)pcApp->GetInstance(wIndex);
        if (pCInst->CurState() == IDLE_STATE)
        {
            pCInst->m_curState = STATE_WORK;
            pCInst->m_lastState = IDLE_STATE;
            break;
        }
    }

    // 当前没有空闲的instance;
    if (wIndex > MAX_INS_NO)
    {
        OspPrintf(TRUE, FALSE, "Current have none idle instance.\r\n");
        return NULL;
    }

    return pCInst;
}

// 客户端文件发送instance分配;
void CClientInstance::ClientFilePostInsAllot(CMessage *const pcMsg, CApp* pcApp)
{
	u16 wMsgLen = 0;

	// 消息内容提取;
	wMsgLen = pcMsg->length;
	s8 *pchMsgGet = new char[wMsgLen + 1];
	ZeroMemory(pchMsgGet, wMsgLen + 1);
	memcpy_s(pchMsgGet, wMsgLen, pcMsg->content, wMsgLen);
	//OspPrintf(TRUE, FALSE, "message content is: %s, length is: %d.\n", pchMsgGet, wMsgLen);

    // 获取空闲的instance指针;
    CClientInstance *pcIns = GetIdleInsID(pcApp);
    if (pcIns == 0)
    {
        OspPrintf(TRUE, FALSE, "Client:have none idle instance.\r\n");
        return;
    }

	TFileInfo *tFileInfo = (TFileInfo *)pchMsgGet;

	// 数据获取;
	pcIns->m_tFileInfo.fileLength = tFileInfo->fileLength;
	pcIns->m_tFileInfo.filePacketNum = tFileInfo->filePacketNum;
	ZeroMemory(pcIns->m_tFileInfo.strFileName, MAX_FILE_NAME + 1);
	memcpy_s(pcIns->m_tFileInfo.strFileName, MAX_FILE_NAME, tFileInfo->strFileName, (wMsgLen - 2*sizeof(u32)));
    
    // 文件读取路径信息处理;
    ZeroMemory(pcIns->m_strFilePath, MAX_PATH);
    lstrcpy(pcIns->m_strFilePath, g_strFilePath);

    // 其余成员信息初始化;
	pcIns->m_wSerInsNum = 0;
	pcIns->m_hFile = INVALID_HANDLE_VALUE;
    pcIns->m_nUsedFlag = 0;

    pcIns->m_nLastStart = 0;
    pcIns->m_nLastSize = 0;
    pcIns->m_dwPktIndex = 0;
    pcIns->m_dnProgValve = 0;
    pcIns->m_nErrorPktNum = 0;

    pcIns->m_nStart = 0;
    pcIns->m_nPuase = 0;
    pcIns->m_nCancel = 0;

    // 按钮无效;
    pFrame->m_pBtnFilePost->SetName(_T("FilePstButton"));
    
    g_pvcFilePstInsNo.push_back(pcIns);
    OspPrintf(TRUE, FALSE, "Client: Get a idle instance, ID: %d, FileName: %s\r\n", pcIns->m_instId, pcIns->m_tFileInfo.strFileName);

	// 文件信息获取后，给自己发送消息并进行列表图形绘制;
	OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_LIST_UI_PAINT, NULL,
		0, 0, MAKEIID(DEMO_APP_CLIENT_NO, pcIns->m_instId), 0, DEMO_POST_TIMEOUT);

	// 释放空间;
	delete [] pchMsgGet;
	pchMsgGet = NULL;

    return;
}

// 客户端文件发送instance释放;
void CClientInstance::CliFilePostInsRelease(u32 dwInsNo)
{
    // 释放全局变量;
    vector< CClientInstance* >::iterator itIndex;
    for (itIndex = g_pvcFilePstInsNo.begin(); itIndex != g_pvcFilePstInsNo.end();)
    {
        if ((*itIndex)->GetInsID() == dwInsNo)
        {
            itIndex = g_pvcFilePstInsNo.erase(itIndex);
			break;
        }
		else
		{
			itIndex++;
		}
    }
    // 列表信息重绘;
    OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_LIST_UI_PAINT, NULL,
        0, 0, MAKEIID(DEMO_APP_CLIENT_NO, dwInsNo), 0, DEMO_POST_TIMEOUT);

	// 释放instance资源;
	m_curState = IDLE_STATE;
	m_lastState = STATE_WORK;
}

// 列表信息绘制;
void CClientInstance::ListUI2Paint()
{
	// 绘图;
	u16 wListIndex = 0;
	s8 achProgress[MAX_FILE_NAME] = {0};
	s8 achBtnName[MAX_STR_LEN]  = {0};
	s8 achPrgName[MAX_STR_LEN]  = {0};
	//s8 achTmpName[MAX_FILE_NAME]  = {0};
	CListUI* pcLoadList = pFrame->m_pList;
	if (pcLoadList == NULL)
	{
		return;
	}

	pcLoadList->RemoveAll();    // 清除重绘;
	for (wListIndex = 0; wListIndex < g_pvcFilePstInsNo.size(); wListIndex++)
	{
		CDemoListContainerElementUI* pcListContainer = new CDemoListContainerElementUI;
		pcListContainer->m_pHeader = pFrame->m_pListHeader;
		CProgressUI* pcProgress = new CProgressUI;
		CHorizontalLayoutUI* pcHorizontalLayout = new CHorizontalLayoutUI;
		CButtonUI* pcButtonStt = new CButtonUI;
		CButtonUI* pcButtonStp = new CButtonUI;
		CButtonUI* pcButtonCcl = new CButtonUI;

		pcListContainer->ApplyAttributeList(_T("height=\"25\" align=\"right\""));

		// 进度条控件添加;
		ZeroMemory(achPrgName, MAX_STR_LEN);
		sprintf(achPrgName, "DemoProgress%u", g_pvcFilePstInsNo[wListIndex]->m_instId);
		USES_CONVERSION;
		pcProgress->ApplyAttributeList(_T("width=\"200\" height=\"20\" foreimage=\"OspDemoSkins\\progress_fore.png\"\
										  min=\"0\" max=\"100\" hor=\"true\" align=\"center\""));
		pcProgress->SetName(A2W(achPrgName));
		ZeroMemory(achProgress, MAX_FILE_NAME);
		sprintf(achProgress, "%s(%ld%%)", g_pvcFilePstInsNo[wListIndex]->m_tFileInfo.strFileName, g_pvcFilePstInsNo[wListIndex]->m_dnProgValve);
		pcProgress->SetValue((s32)g_pvcFilePstInsNo[wListIndex]->m_dnProgValve);
		pcProgress->SetText(A2W(achProgress));
		pcListContainer->Add(pcProgress);

		//pcHorizontalLayout->ApplyAttributeList(_T(""));

		// 开始按键控件添加;
		ZeroMemory(achBtnName, MAX_STR_LEN);
		sprintf(achBtnName, "FileSttButton%u", g_pvcFilePstInsNo[wListIndex]->m_instId);
		pcButtonStt->ApplyAttributeList(_T("width=\"20\" height=\"20\" normalimage=\"file=\'OspDemoSkins\\start.png\'\" pushedimage=\"file=\'OspDemoSkins\\start2.png\'\""));
		pcButtonStt->SetName(A2W(achBtnName));
		pcHorizontalLayout->Add(pcButtonStt);

		// 暂停按键控件添加;
		ZeroMemory(achBtnName, MAX_STR_LEN);
		sprintf(achBtnName, "FileStpButton%u", g_pvcFilePstInsNo[wListIndex]->m_instId);
		pcButtonStp->ApplyAttributeList(_T("width=\"20\" height=\"20\" normalimage=\"file=\'OspDemoSkins\\pause.png\'\" pushedimage=\"file=\'OspDemoSkins\\pause2.png\'\""));
		pcButtonStp->SetName(A2W(achBtnName));
		pcHorizontalLayout->Add(pcButtonStp);

		// 取消按键控件添加;
		ZeroMemory(achBtnName, MAX_STR_LEN);
		sprintf(achBtnName, "FileCclButton%u", g_pvcFilePstInsNo[wListIndex]->m_instId);
		pcButtonCcl->ApplyAttributeList(_T("width=\"20\" height=\"20\" normalimage=\"file=\'OspDemoSkins\\del.png\'\" pushedimage=\"file=\'OspDemoSkins\\del.png\'\""));
		pcButtonCcl->SetName(A2W(achBtnName));
		pcHorizontalLayout->Add(pcButtonCcl);

		pcListContainer->Add(pcHorizontalLayout);
		pcLoadList->Add(pcListContainer);

	}
	return;
}

// 第一包图形绘制;
void FirstProgressUI2Paint(CMessage *const pcMsg)
{
	u16 wInsNo = GETINS(pcMsg->srcid);
	u16 wIndex = 0;
	s8 achProgress[MAX_FILE_NAME] = {0};
	s8 achPrgName[MAX_STR_LEN]  = {0};

	wIndex = FindIndexByInsNo(wInsNo);
	if (wIndex == MAX_INS_NO)
	{
		OspPrintf(TRUE, FALSE, "Index Error.\r\n");
		return;
	}
	
	// 预留;
	//sprintf(achPrgName, "DemoProgress%u", wInsNo);


	return;
}

// 后续正常包绘制;
void NormalProgressUI2Paint(CMessage *const pcMsg)
{
	u16 wInsNo = GETINS(pcMsg->srcid);
	u16 wIndex = 0;
	s8 achProgress[MAX_FILE_NAME] = {0};
	s8 achPrgName[MAX_STR_LEN]  = {0};

	wIndex = FindIndexByInsNo(wInsNo);
	if (wIndex == MAX_INS_NO)
	{
		OspPrintf(TRUE, FALSE, "Index Error.\r\n");
		return;
	}

	sprintf(achPrgName, "DemoProgress%u", wInsNo);

	USES_CONVERSION;
	CProgressUI* pcPrg = static_cast<CProgressUI*>(pFrame->m_pm.FindControl(A2W(achPrgName)));
	if (pcPrg == NULL)
	{
		OspPrintf(TRUE, FALSE, "Can't find the progress control\r\n");
	}

	// 进度条赋值;
	ZeroMemory(achProgress, MAX_FILE_NAME);
	sprintf(achProgress, "%s(%ld%%)", g_pvcFilePstInsNo[wIndex]->m_tFileInfo.strFileName, g_pvcFilePstInsNo[wIndex]->m_dnProgValve);
	pcPrg->SetValue((s32)g_pvcFilePstInsNo[wIndex]->m_dnProgValve);
	pcPrg->SetText(A2W(achProgress));
	
	return;
}

// 传送完毕列表信息绘制;
void LastProgressUI2Paint(CMessage *const pcMsg)
{
	u16 wInsNo = GETINS(pcMsg->srcid);
	u16 wIndex = 0;
	s8 achProgress[MAX_FILE_NAME] = {0};
	s8 achPrgName[MAX_STR_LEN]  = {0};
	s8 achBtnNameA[MAX_STR_LEN]  = {0};
	s8 achBtnNameB[MAX_STR_LEN]  = {0};

	wIndex = FindIndexByInsNo(wInsNo);
	if (wIndex == MAX_INS_NO)
	{
		OspPrintf(TRUE, FALSE, "Index Error.\r\n");
		return;
	}

	sprintf(achPrgName, "DemoProgress%u", wInsNo);
	sprintf(achBtnNameA, "DemoProgress%u", wInsNo);
	sprintf(achBtnNameB, "FileCclButton%u", wInsNo);

	USES_CONVERSION;
	CProgressUI* pcPrg = static_cast<CProgressUI*>(pFrame->m_pm.FindControl(A2W(achPrgName)));
	if (pcPrg == NULL)
	{
		OspPrintf(TRUE, FALSE, "Can't find the progress control\r\n");
	}

	// 进度条赋值;
	ZeroMemory(achProgress, MAX_FILE_NAME);
	sprintf(achProgress, "%s(%ld%%)", g_pvcFilePstInsNo[wIndex]->m_tFileInfo.strFileName, 100);
	pcPrg->SetValue(100);
	pcPrg->SetText(A2W(achProgress));

	// 删除指定控件;
	CListUI* pcLoadList = pFrame->m_pList;
	if (pcLoadList == NULL)
	{
		return;
	}

	CButtonUI* pcBtnCcl = static_cast<CButtonUI*>(pFrame->m_pm.FindControl(A2W(achBtnNameB)));
	pcLoadList->Remove(pcBtnCcl);
	return;
}

// DaemonInstanceEntry消息分发处理入口;
void CClientInstance::DaemonInstanceEntry(CMessage *const pcMsg, CApp* pcApp)
{
    /*得到当前消息的类型;*/
    u16 wCurEvent = pcMsg->event;

    switch (wCurEvent)
    {
    case EVENT_CLIENT_FILE_POST_INS_ALLOT:
        ClientFilePostInsAllot(pcMsg, pcApp);
        //NextState(STATE_WORK);
        break;
	case EVENT_CLIENT_FILE_POST_INS_RELEASE:
		//CliFilePostInsRelease(pcMsg, pcApp);
		//NextState(STATE_WORK);
		break;
	case EVENT_LIST_UI_PAINT:
		ListUI2Paint();
		break;
	case EVENT_FIRST_PROGRESS_UI_PAINT:
		FirstProgressUI2Paint(pcMsg);
		//NextState(STATE_WORK);
		break;
	case EVENT_NORMAL_PROGRESS_UI_PAINT:
		NormalProgressUI2Paint(pcMsg);
		//NextState(STATE_WORK);
		break;
	case EVENT_LAST_PROGRESS_UI_PAINT:
		LastProgressUI2Paint(pcMsg);
		//NextState(STATE_WORK);
		break;

    default:
        // Def_Fuction(pMsg);
        break;
    }
    
    return;
}


// CDemoListContainerElementUI类函数定义;
#if 1
void CDemoListContainerElementUI::SetPos(RECT rc)  
{  
    CContainerUI::SetPos(rc);  
    if (m_pOwner == NULL)
    {
        return;
    }
    if (m_pHeader == NULL)  
    {  
        return;  
    }  
    TListInfoUI* pInfo = m_pOwner->GetListInfo();  
    int nCount = m_items.GetSize();  
    for (int i = 0; i < nCount; i++)  
    {  
        CControlUI *pHorizontalLayout = static_cast<CControlUI*>(m_items[i]);  
        // if (pHorizontalLayout != NULL)   
        // {   
        // RECT rtHeader = pHeaderItem->GetPos();   
        // RECT rt = pHorizontalLayout->GetPos();   
        // rt.left = pInfo->rcColumn[i].left;   
        // rt.right = pInfo->rcColumn[i].right;   
        // pHorizontalLayout->SetPos(rt);   
        // }   

        CListHeaderItemUI *pHeaderItem = static_cast<CListHeaderItemUI*>(m_pHeader->GetItemAt(i));  
        if (pHorizontalLayout != NULL && pHeaderItem != NULL)  
        {  
            RECT rtHeader = pHeaderItem->GetPos();  
            RECT rt = pHorizontalLayout->GetPos();  
            rt.left = rtHeader.left;  
            rt.right = rtHeader.right;  
            pHorizontalLayout->SetPos(rt);  
        }
    } 
}
#endif