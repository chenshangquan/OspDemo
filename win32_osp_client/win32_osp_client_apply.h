#pragma once

#include "../include/ospdemo_com.h"

#define MAX_INS_NO 10

/****************************************************
 *
 * CDemoInstance 函数定义;
 *
 ****************************************************/

// InstanceEntry:event 消息处理函数定义;
////////////////////////////////////////

// 包发送处理函数;
void SendFileInfo(s32 fStart,s32 fSize,char *fHead, u16 wCliPostInsNo, u16 wSerPostInsNo, u16 wIndex)
{
    OspPrintf(TRUE, FALSE, "SendFileInfo.\r\n");
    u16 wListIndex = 0;
    u16 wListItemNum = 0;
    s8 achProgress[MAX_FILE_NAME] = {0};
    //s8 achBtnName[MAX_STR_LEN]  = {0};
    s8 achPrgName[MAX_STR_LEN]  = {0};
    //s8 achTmpName[MAX_FILE_NAME]  = {0};
    BYTE m_sendBuffer[MAX_FILE_PACKET + 1] = {0};   // 待传送数据buffer;

    ZeroMemory(achPrgName, MAX_STR_LEN);
    sprintf(achPrgName, "DemoProgress%u", wIndex);

    // 文件传输进度数值;
    g_pvcFilePstInsNo[wIndex]->m_dnProgValve = 100 * g_pvcFilePstInsNo[wIndex]->m_nPktIndex / g_pvcFilePstInsNo[wIndex]->m_tFileInfo.filePacketNum;

    u32 dwFilePacketBuff = MAX_FILE_PACKET - 4 - 2*sizeof(s32) - 3*sizeof(u16);

    TFileMessage strFileMsg;
    // 定义用于发送文件数据的文件包，fileHead定义为“FFF”;
    strFileMsg.fileHead[0] = 'F';
    strFileMsg.fileHead[1] = 'F';
    strFileMsg.fileHead[2] = 'F';
    strFileMsg.fileHead[3] = '\0';

    // 包中记录客户端/服务端的发送instance ID;
    strFileMsg.wCliPostInsNum = wCliPostInsNo;
    strFileMsg.wSerPostInsNum = wSerPostInsNo;

    // 包中记录客户端的发送instance 索引值;
    strFileMsg.wIndex = wIndex;

    // 初始化;
    strFileMsg.fileStart   = 0;
    strFileMsg.fileSize    = 0;

    // 是否基于上次分包的偏移位置;
    if (fStart == g_pvcFilePstInsNo[wIndex]->m_nLastStart && fSize == g_pvcFilePstInsNo[wIndex]->m_nLastSize)
    {
        // 最后一个包发送完成;
        if (!strcmp(fHead, "END"))
        {
            OspPrintf(TRUE, FALSE, "Enter END.\r\n");
#if 0
            //进度条置顶，完成文件传输;
            pProgress->SetValue(100);
            pProgress->SetText(L"Progress(100%)");

            // 文件校验并显示结果; --TODO
            ::MessageBox(NULL, _T("已完成文件传输！文件检验OK!!"), _T("文件传输结果"), NULL);
            USES_CONVERSION;
            // 获取控件进度条指针;
            pcPrg = static_cast<CProgressUI*>(pFrame->m_pm.FindControl(A2W(achPrgName)));

            // 进度条赋值;
            ZeroMemory(achProgress, MAX_FILE_NAME);
            sprintf(achProgress, "%s(%d%%)", g_tInsNo[wIndex].m_tFileInfo.strFileName, 100);
            pcPrg->SetValue(100);
            pcPrg->SetText(A2W(achProgress));

            pcLoadList->RemoveAt(wIndex);
#endif
			// 传送完毕时，给自己发送消息并进行进度条的绘制;
			OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_LAST_PROGRESS_UI_PAINT, NULL,
				0, 0, MAKEIID(DEMO_APP_CLIENT_NO, wCliPostInsNo), 0, DEMO_POST_TIMEOUT);

            return;
        }
        // 发送第一个包;
        else if (fStart == 0 && fSize == 0 && !strcmp(fHead,"0"))
        {
            // 传输出错标志、包索引置0;
            g_pvcFilePstInsNo[wIndex]->m_nErrorPktNum = 0;
            g_pvcFilePstInsNo[wIndex]->m_nPktIndex   = 0;

            // 考虑只有一个包的情况;
            if ( g_pvcFilePstInsNo[wIndex]->m_tFileInfo.fileLength <= dwFilePacketBuff)
            {
                strFileMsg.fileStart = 0;
                strFileMsg.fileSize  = g_pvcFilePstInsNo[wIndex]->m_tFileInfo.fileLength;
                g_pvcFilePstInsNo[wIndex]->m_nLastStart = 0;
                g_pvcFilePstInsNo[wIndex]->m_nLastSize  = g_pvcFilePstInsNo[wIndex]->m_tFileInfo.fileLength;
                OspPrintf(TRUE, FALSE, "Only one packet. Index(%d)\r\n", g_pvcFilePstInsNo[wIndex]->m_nPktIndex);
            }
            else
            {
                strFileMsg.fileStart = 0;
                strFileMsg.fileSize  = dwFilePacketBuff;
                g_pvcFilePstInsNo[wIndex]->m_nLastStart = 0;
                g_pvcFilePstInsNo[wIndex]->m_nLastSize  = dwFilePacketBuff;
                OspPrintf(TRUE, FALSE, "First packet. Index(%d)\r\n", g_pvcFilePstInsNo[wIndex]->m_nPktIndex);
            }

			// 发送第一个包时，给自己发送消息并进行进度条的绘制;
			OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_FIRST_PROGRESS_UI_PAINT, NULL,
				0, 0, MAKEIID(DEMO_APP_CLIENT_NO, wCliPostInsNo), 0, DEMO_POST_TIMEOUT);
        }
        // 接收到server端回复的正常确认包;
        else if (!strcmp(fHead,"OK!"))
        {
#if 0  // 需更改逻辑，发送消息实现绘图;

            USES_CONVERSION;
            CProgressUI* pcPrg = static_cast<CProgressUI*>(pFrame->m_pm.FindControl(A2W(achPrgName)));
            if (pcPrg != NULL)
            {
                OspPrintf(TRUE, FALSE, "Can't find the progress control\r\n");
                // 进度条赋值;
                ZeroMemory(achProgress, MAX_FILE_NAME);
                sprintf(achProgress, "%s(%ld%%)", g_pvcFilePstInsNo[wIndex]->m_tFileInfo.strFileName, g_pvcFilePstInsNo[wIndex]->m_dnProgValve);
                pcPrg->SetValue((s32)g_pvcFilePstInsNo[wIndex]->m_dnProgValve);
                pcPrg->SetText(A2W(achProgress));
            }
#endif
            // 传输出错标志置0;
            g_pvcFilePstInsNo[wIndex]->m_nErrorPktNum = 0;
            // 包索引进行累加;
            g_pvcFilePstInsNo[wIndex]->m_nPktIndex++;

            // 当要发送最后一个包的时候;
            if ((g_pvcFilePstInsNo[wIndex]->m_tFileInfo.fileLength - fStart - dwFilePacketBuff) <= dwFilePacketBuff)
            {
                strFileMsg.fileStart = fStart + dwFilePacketBuff;
                //strFileMsg.fileSize  = g_pvcFilePstInsNo[wIndex].m_tFileInfo.fileLength % nFilePacketBuff;
                strFileMsg.fileSize  = g_pvcFilePstInsNo[wIndex]->m_tFileInfo.fileLength - strFileMsg.fileStart;
                g_pvcFilePstInsNo[wIndex]->m_nLastStart = strFileMsg.fileStart;
                g_pvcFilePstInsNo[wIndex]->m_nLastSize  = strFileMsg.fileSize;
                OspPrintf(TRUE, FALSE, "Last packet. Index(%d)\r\n",  g_pvcFilePstInsNo[wIndex]->m_nPktIndex);
            }
            else
            {
                strFileMsg.fileStart = fStart + dwFilePacketBuff;
                strFileMsg.fileSize  = dwFilePacketBuff;
                g_pvcFilePstInsNo[wIndex]->m_nLastStart = strFileMsg.fileStart;
                g_pvcFilePstInsNo[wIndex]->m_nLastSize  = strFileMsg.fileSize;
                OspPrintf(TRUE, FALSE, "Next packet. Index(%d)\r\n",  g_pvcFilePstInsNo[wIndex]->m_nPktIndex);
            }

			// 后续正常包传送时，给自己发送消息并进行进度条的绘制;
			OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_NORMAL_PROGRESS_UI_PAINT, NULL,
				0, 0, MAKEIID(DEMO_APP_CLIENT_NO, wCliPostInsNo), 0, DEMO_POST_TIMEOUT);
        }
        // 确认包表示接收到的包有错误，则重新发送上一个包;
        else if (!strcmp(fHead, "ERR"))   
        {
            OspPrintf(TRUE, FALSE, "Error packet. Index(%d)\r\n",  g_pvcFilePstInsNo[wIndex]->m_nPktIndex);
            g_pvcFilePstInsNo[wIndex]->m_nErrorPktNum++;
            strFileMsg.fileStart = g_pvcFilePstInsNo[wIndex]->m_nLastStart;
            strFileMsg.fileSize  = g_pvcFilePstInsNo[wIndex]->m_nLastSize;
        }
        // 确认包表示用户取消了文件传输;
        else if (!strcmp(fHead, "CCL"))
        {
            string temp=("");
            //GetDlgItemText(IDC_EDIT_CLIENT,temp);
            //temp += _T("\r\n");
            //temp += _T("对方取消了文件传输");
            //SetDlgItemText(IDC_EDIT_CLIENT,temp);
            return;
        }
        // 暂停包表示用户停止文件传送;
        else if (!strcmp(fHead, "STP"))
        {
            OspPrintf(TRUE, FALSE, "stop the packet send.\r\n\r\n\r\n");
            Sleep(1);
        }
        // 接收到的信息不正常时默认出错，重新发送上一个包;
        else
        {
            OspPrintf(TRUE, FALSE, "Repeat the previous packet. Index(%d)\r\n",  g_pvcFilePstInsNo[wIndex]->m_nPktIndex);
            g_pvcFilePstInsNo[wIndex]->m_nErrorPktNum++;
            strFileMsg.fileStart = g_pvcFilePstInsNo[wIndex]->m_nLastStart;
            strFileMsg.fileSize  = g_pvcFilePstInsNo[wIndex]->m_nLastSize;
        }
    }
    // 接收到的信息不正常时默认出错，重新发送上一个包;
    else 
    {
        OspPrintf(TRUE, FALSE, "Other error, Repeat the previous packet. Index(%d)\r\n",  g_pvcFilePstInsNo[wIndex]->m_nPktIndex);
        g_pvcFilePstInsNo[wIndex]->m_nErrorPktNum++;
        strFileMsg.fileStart = g_pvcFilePstInsNo[wIndex]->m_nLastStart;
        strFileMsg.fileSize  = g_pvcFilePstInsNo[wIndex]->m_nLastSize;
    }

    // 这里可以进行出错处理;
    if (g_pvcFilePstInsNo[wIndex]->m_nErrorPktNum > 10)       
    {
        strFileMsg.fileHead[0] = 'E';
        strFileMsg.fileHead[1] = 'R';
        strFileMsg.fileHead[2] = 'R';
        strFileMsg.fileHead[3] = '\0';
    }

    ZeroMemory(m_sendBuffer, sizeof(BYTE)*(MAX_FILE_PACKET) + 1);
    ZeroMemory(strFileMsg.filePacket, sizeof(BYTE)*(MAX_FILE_PACKET) - 4 - 2*sizeof(s32) - 3*sizeof(u16));

    // 读取一个包的内容;
    OspPrintf(TRUE, FALSE, "Start to read the file message.\r\n");

    DWORD nByte;

    //HANDLE mFile = CreateFile(g_pvcFilePstInsNo[wIndex]->m_strFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE mFile = CreateFile(g_pvcFilePstInsNo[wIndex]->m_strFilePath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (mFile == INVALID_HANDLE_VALUE)
    {
        DWORD dw = GetLastError();
        OspPrintf(TRUE, FALSE, "Client: CreateFile Failed!, Error: %d.\r\n", dw);
        return;
    }

    SetFilePointer(mFile, strFileMsg.fileStart, NULL, FILE_BEGIN);
    ReadFile(mFile, strFileMsg.filePacket, strFileMsg.fileSize, &nByte, NULL);
    CloseHandle(mFile);

    BYTE* filePacket = (BYTE*)&strFileMsg;
    // 将待发送包的内容填充至发送缓存区;
    memcpy(m_sendBuffer, filePacket, strFileMsg.fileSize + 4 + 2*sizeof(s32) + 3*sizeof(u16));

    OspPrintf(TRUE, FALSE, "Start to post the file message to server. InsNo:%d\r\n", wCliPostInsNo);

    // 发送包到服务端;
    OspPost(MAKEIID(DEMO_APP_SERVER_NO, wSerPostInsNo), EVENT_FILE_POST2S, m_sendBuffer,
        (strFileMsg.fileSize + 4 + 2*sizeof(s32) + 3*sizeof(u16)), g_wNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, wCliPostInsNo));
}

// 申请服务端分配文件发送instance回复;
void ClientFilePostInsAllotAck(CMessage *const pMsg)
{
	u16 wVecIndex = 0;
	u16 wCliPostInsNo = 0;
	u16 wSerPostInsNo = 0;

	wCliPostInsNo = GETINS(pMsg->dstid);
	wSerPostInsNo = GETINS(pMsg->srcid);

	for (wVecIndex = 0; wVecIndex < g_pvcFilePstInsNo.size(); wVecIndex++)
	{
		if (g_pvcFilePstInsNo[wVecIndex]->m_instId == wCliPostInsNo)
		{
			break;
		}
	}

	if (wVecIndex == g_pvcFilePstInsNo.size())
	{
        OspPrintf(TRUE, FALSE, "Client: Find vector index failed.\r\n");
		return;
	}

    OspPrintf(TRUE, FALSE, "Client: wCliPostInsNo: %d, wSerPostInsNo: %d, wVecIndex: %d.\r\n", wCliPostInsNo, wSerPostInsNo, wVecIndex);
	// 发送第一个包;
	SendFileInfo(0, 0, "0", wCliPostInsNo, wSerPostInsNo, wVecIndex);
	return;
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

    // 窗口赋值;
    CEditUI* pEditRecv =  pFrame->m_pEditRecv;
    pEditRecv->SetText(CA2W(pchMsgGet));

    // 释放空间;
    delete [] pchMsgGet;
    pchMsgGet = NULL;
}

// 文件信息初始化;
#if 0
void FileInfoInit(u16 wIndex)
{
    // 初始化文件信息内容;
    g_tInsNo[wIndex].m_tFileInfo.filePacketNum = 0;
    g_tInsNo[wIndex].m_tFileInfo.filePacketIndex = 0;
    g_tInsNo[wIndex].m_tFileInfo.nFilePacketBuff = 0;

    g_tInsNo[wIndex].m_tFileInfo.fileStart  = 0;
    g_tInsNo[wIndex].m_tFileInfo.fileSize   = 0;
    g_tInsNo[wIndex].m_tFileInfo.lastStart  = 0;
    g_tInsNo[wIndex].m_tFileInfo.lastSize   = 0;
    g_tInsNo[wIndex].m_tFileInfo.fileLength = 0;

    ZeroMemory(g_tInsNo[wIndex].m_tFileInfo.strFileName, MAX_FILE_NAME + 1);
}
#endif

// 处理的server端的回复包，决定下一次的文件发送;
void OnClientReceive(CMessage *const pMsg)
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
    u16 wCliPostInsNo = strFileMsg->wCliPostInsNum;
    u16 wSerPostInsNo = strFileMsg->wSerPostInsNum;
    u16 wIndex     = strFileMsg->wIndex;

    // 暂停发送;
    while(g_pvcFilePstInsNo[wIndex]->m_nPuase != 0)
    {
        Sleep(100);
    }

    OspPrintf(TRUE, FALSE, "Client: Next send, wCliPostInsNo: %d, wSerPostInsNo: %d, wIndex: %d\r\n", wCliPostInsNo, wSerPostInsNo, wIndex);
    // 提取消息发送处理;
    SendFileInfo(nFileStart, nFileSize, achFileHead, wCliPostInsNo, wSerPostInsNo, wIndex);

    delete [] strMsgGet;
    strMsgGet = NULL;
}

// InstanceEntry消息类型分发处理入口;
void CDemoInstance::InstanceEntry(CMessage *const pMsg)
{
    /*得到当前消息的类型;*/
    u16 wCurEvent = pMsg->event;

    // 根据不同的消息类型进行处理;
    switch (wCurEvent)
    {
    case EVENT_MSG_POST:
        MsgPostFunc(pMsg);
        NextState(STATE_WORK);
        //SetTimer(EVENT_TIMEOUT, 1000);
        break;
	case EVENT_SERVER_FILE_POST_INS_ALLOT_ACK:
		ClientFilePostInsAllotAck(pMsg);
        NextState(STATE_WORK);
		break;
    case EVENT_FILE_POST2C:
        OnClientReceive(pMsg);
        NextState(STATE_WORK);
        break;
    default:
        
        break;
    }

    return;
}


// DaemonInstanceEntry:event 消息处理函数定义;
//////////////////////////////////////////////

// 获取空闲的instance指针;
CDemoInstance* GetIdleInsID(CApp* pcApp)
{
    u16 wIndex = 0;
    CDemoInstance *pCInst = NULL;

    // 遍历所有instance，寻找到一个空闲的instance并返回;
    for (wIndex = 2; wIndex <= MAX_INS_NO; wIndex++)
    {
        pCInst = (CDemoInstance *)pcApp->GetInstance(wIndex);
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
void ClientFilePostInsAllot(CMessage *const pcMsg, CApp* pcApp)
{
	u16 wMsgLen = 0;

	// 消息内容提取;
	wMsgLen = pcMsg->length;
	s8 *pchMsgGet = new char[wMsgLen + 1];
	ZeroMemory(pchMsgGet, wMsgLen + 1);
	memcpy_s(pchMsgGet, wMsgLen, pcMsg->content, wMsgLen);
	//OspPrintf(TRUE, FALSE, "message content is: %s, length is: %d.\n", pchMsgGet, wMsgLen);

    // 获取空闲的instance指针;
    CDemoInstance *pcIns = GetIdleInsID(pcApp);
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
#if 0
void CliFilePostInsRelease(CMessage *const pcMsg, CApp* pcApp)
{
	u16 wInsid = 0;
	u16 wIndex = 0;
	CInstance *pCInst = NULL;

	// 获取收到的消息内容;
	u16 wMsgLen = pcMsg->length;
	char *pchMsgGet = new char[wMsgLen + 1];
	ZeroMemory(pchMsgGet, wMsgLen + 1);
	memcpy_s(pchMsgGet, wMsgLen, pcMsg->content, wMsgLen);

	wInsid = atoi(pchMsgGet);

    // 释放全局变量;
    vector<TInStatus>::iterator itIndex;
    for (itIndex = g_tInsNo.begin(); itIndex != g_tInsNo.end(); itIndex++)
    {
        if (itIndex->uInsNum == wInsid)
        {
            g_tInsNo.erase(itIndex);
            break;
        }
    }

	// 释放instance资源;
	pCInst = pcApp->GetInstance(wInsid);
	pCInst->m_curState = IDLE_STATE;
	pCInst->m_lastState = STATE_WORK;
}
#endif

u16 FindIndexByInsNo(u16 wInsNo)
{
	u16 wIndex = 0;
	// 寻找Insid对应的索引;
	for (wIndex = 0; wIndex < g_pvcFilePstInsNo.size(); wIndex++)
	{
		if (g_pvcFilePstInsNo[wIndex]->m_instId == wInsNo)
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

// 列表信息绘制;
void ListUI2Paint()
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
		sprintf(achProgress, "%s(%ld%%)", g_pvcFilePstInsNo[wListIndex]->m_tFileInfo.strFileName, 0);
		pcProgress->SetValue(0);
		pcProgress->SetText(A2W(achProgress));
		pcListContainer->Add(pcProgress);

		//pcHorizontalLayout->ApplyAttributeList(_T(""));

		// 开始按键控件添加;
		ZeroMemory(achBtnName, MAX_STR_LEN);
		sprintf(achBtnName, "FileSttButton%u", g_pvcFilePstInsNo[wListIndex]->m_instId);
		pcButtonStt->ApplyAttributeList(_T("text=\"S\" width=\"17\" height=\"20\""));
		pcButtonStt->SetName(A2W(achBtnName));
		pcHorizontalLayout->Add(pcButtonStt);

		// 暂停按键控件添加;
		ZeroMemory(achBtnName, MAX_STR_LEN);
		sprintf(achBtnName, "FileStpButton%u", g_pvcFilePstInsNo[wListIndex]->m_instId);
		pcButtonStp->ApplyAttributeList(_T("text=\"P\" width=\"17\" height=\"20\""));
		pcButtonStp->SetName(A2W(achBtnName));
		pcHorizontalLayout->Add(pcButtonStp);

		// 取消按键控件添加;
		ZeroMemory(achBtnName, MAX_STR_LEN);
		sprintf(achBtnName, "FileCclButton%u", g_pvcFilePstInsNo[wListIndex]->m_instId);
		pcButtonCcl->ApplyAttributeList(_T("text=\"C\" width=\"16\" height=\"20\""));
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
	
	sprintf(achPrgName, "DemoProgress%u", wInsNo);


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
void CDemoInstance::DaemonInstanceEntry(CMessage *const pcMsg, CApp* pcApp)
{
    /*得到当前消息的类型;*/
    u16 wCurEvent = pcMsg->event;

    switch (wCurEvent)
    {
    case EVENT_CLIENT_FILE_POST_INS_ALLOT:
        ClientFilePostInsAllot(pcMsg, pcApp);
        NextState(STATE_WORK);
        break;
	case EVENT_CLIENT_FILE_POST_INS_RELEASE:
		//CliFilePostInsRelease(pcMsg, pcApp);
		NextState(STATE_WORK);
		break;
	case EVENT_LIST_UI_PAINT:
		ListUI2Paint();
	case EVENT_FIRST_PROGRESS_UI_PAINT:
		FirstProgressUI2Paint(pcMsg);
		NextState(STATE_WORK);
		break;
	case EVENT_NORMAL_PROGRESS_UI_PAINT:
		NormalProgressUI2Paint(pcMsg);
		NextState(STATE_WORK);
		break;
	case EVENT_LAST_PROGRESS_UI_PAINT:
		LastProgressUI2Paint(pcMsg);
		NextState(STATE_WORK);
		break;

    default:
        // Def_Fuction(pMsg);
        break;
    }
    
    return;
}

CDemoInstance::~CDemoInstance()
{

}

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