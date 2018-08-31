#include "StdAfx.h"
#include "ServerInstance.h"
#include "win32_osp_server.h"

/****************************************************
 *
 * CClientInstance 函数定义;
 *
 ****************************************************/
vector<CServerInstance*> g_pvcFilePstInsNo;

TCHAR g_strFilePath[MAX_PATH] = _T("");
TCHAR g_strFileName[MAX_FILE_NAME] = _T("");
TCHAR g_strFolderPath[MAX_PATH] = _T("F:\\2");

u16 g_wNodeNum;


extern CFrameWindowWnd *pFrame;
extern BOOL IsIpFormatRight(LPCTSTR pIpAddr);
extern u16 FindIndexByInsNo(u16 wInsNo);
CServerInstance::CServerInstance(void)
{
}

CServerInstance::~CServerInstance(void)
{
}

// 根据intance ID，找到对应的全局变量索引;
u16 FindInsIndex(u16 wSerPostInsNo)
{
    u16 wIndex = 0;

    // 根据server端文件发送的instance ID，找到对应索引，获取文件写入路径信息;
	for (wIndex = 0; wIndex < g_pvcFilePstInsNo.size(); wIndex++)
	{
		if (g_pvcFilePstInsNo[wIndex]->m_instId == wSerPostInsNo)
        {
            return wIndex;
        }
    }

    return (MAX_INS_NO + 1);
}

// Instance释放;
void InsRelease(u16 wCliPostInsNo, u16 wSerPostInsNo)
{
	s8 achCliPostInsNoStr[4] = {0};
	s8 achSerPostInsNoStr[4] = {0};

	// 释放Client端文件发送的instance;
	sprintf(achCliPostInsNoStr, "%d", wCliPostInsNo);
	OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_CLIENT_FILE_POST_INS_RELEASE,
		achCliPostInsNoStr, strlen(achCliPostInsNoStr), g_wNodeNum, MAKEIID(DEMO_APP_SERVER_NO, wSerPostInsNo), 0, DEMO_POST_TIMEOUT);

	// 释放Server端文件发送的instance;
	sprintf(achSerPostInsNoStr, "%d", wCliPostInsNo);
	OspPost(MAKEIID(DEMO_APP_SERVER_NO, CInstance::DAEMON), EVENT_SERVER_FILE_POST_INS_RELEASE,
		achSerPostInsNoStr, strlen(achSerPostInsNoStr), 0, MAKEIID(DEMO_APP_SERVER_NO, wSerPostInsNo), 0, DEMO_POST_TIMEOUT);
}

/****************************************************
 *
 * CServerInstance 函数定义;
 *
 ****************************************************/

// InstanceEntry:event 消息处理函数定义;
/////////////////////////////////////////////////////

// 消息接收处理函数;
void MsgPostFunc(CMessage *const pMsg)
{
    u16 wMsgLen = 0;
    s8 *pchMsgGet = NULL;

    // 消息内容提取;
    wMsgLen = pMsg->length;
    
    // 处理空消息，一般用于服务端获取srcnode;
    if (wMsgLen == 0)
    {
        g_wNodeNum = pMsg->srcnode;
        return;
    }

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

// 将接受到的包内容写入文件;
bool StoreFilePacket(TFileMessage *strFileMsgGet, u16 wSerIndex)
{
    DWORD nByte;

    OspPrintf(TRUE, FALSE, "Server: Start to store the packet\r\n");

    // 记录本次偏移位置;
    g_pvcFilePstInsNo[wSerIndex]->m_nLastStart = strFileMsgGet->fileStart;
    g_pvcFilePstInsNo[wSerIndex]->m_nLastSize  = strFileMsgGet->fileSize;

    // 创建文件并写入文件;
    HANDLE mFileW = CreateFile(g_pvcFilePstInsNo[wSerIndex]->m_strFilePath, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (mFileW == INVALID_HANDLE_VALUE)
    {
        OspPrintf(TRUE, FALSE, "Server: CreateFile fail!\r\n");
        return FALSE;
    }

    SetFilePointer(mFileW, 0, 0, FILE_END);
    FlushFileBuffers(mFileW);
    if (WriteFile(mFileW, strFileMsgGet->filePacket, strFileMsgGet->fileSize, &nByte, NULL) == FALSE)
    {
        OspPrintf(TRUE, FALSE, "Packet write to local failed\r\n");
        return FALSE;
    }
    FlushFileBuffers(mFileW);
    CloseHandle(mFileW);

    return TRUE;
}

// 回复包填充函数;
void SendFilePacketEcho(s32 nFlag, u16 wCliPostInsNo, u16 wSerPostInsNo, u16 wIndex)    //向Client发送确认包，m_fileMsgSend是CMySocket的变量;
{
    u16 wSerIndex = 0;
    TFileMessage strFileMsgAck = {0};

    if(nFlag == FILE_PACKET_OK)
    {
        strFileMsgAck.fileHead[0] = 'O';
        strFileMsgAck.fileHead[1] = 'K';
        strFileMsgAck.fileHead[2] = '!';
        strFileMsgAck.fileHead[3] = '\0';
    }
    else if(nFlag == FILE_PACKET_ERROR)
    {
        strFileMsgAck.fileHead[0] = 'E';
        strFileMsgAck.fileHead[1] = 'R';
        strFileMsgAck.fileHead[2] = 'R';
        strFileMsgAck.fileHead[3] = '\0';
    }
    else if(nFlag == FILE_PACKET_END)
    {
        strFileMsgAck.fileHead[0] = 'E';
        strFileMsgAck.fileHead[1] = 'N';
        strFileMsgAck.fileHead[2] = 'D';
        strFileMsgAck.fileHead[3] = '\0';
    }
    else if(nFlag == FILE_PACKET_CANCEL)
    {
        strFileMsgAck.fileHead[0] = 'C';
        strFileMsgAck.fileHead[1] = 'C';
        strFileMsgAck.fileHead[2] = 'L';
        strFileMsgAck.fileHead[3] = '\0';
    }

    // 根据instanceID，获取instance索引号;
    wSerIndex = FindInsIndex(wSerPostInsNo);
    if (wSerIndex == MAX_INS_NO + 1)
    {
        OspPrintf(TRUE, FALSE, "can't find the instance index.\r\n");
        return;
    }

    strFileMsgAck.fileStart      = g_pvcFilePstInsNo[wSerIndex]->m_nLastStart;
    strFileMsgAck.fileSize       = g_pvcFilePstInsNo[wSerIndex]->m_nLastSize;
    strFileMsgAck.wCliPostInsNum = wCliPostInsNo;
    strFileMsgAck.wSerPostInsNum = wSerPostInsNo;
    strFileMsgAck.wIndex = wIndex;

    ZeroMemory(strFileMsgAck.filePacket, MAX_FILE_PACKET - 4 - 2*sizeof(s32) - 3*sizeof(u16));
    BYTE* newPacket = (BYTE*)&strFileMsgAck;
    u16 wLen = 4 + 2*sizeof(s32) + 3*sizeof(u16);

    OspPrintf(TRUE, FALSE, "Server: Response package.\r\n");
    // 发送包到客户端
    OspPost(MAKEIID(DEMO_APP_CLIENT_NO, wCliPostInsNo), EVENT_FILE_POST2C,
        newPacket, wLen, g_wNodeNum, MAKEIID(DEMO_APP_SERVER_NO, wSerPostInsNo));
}

// 接受包处理函数;
void ReceiveFilePacket(TFileMessage *strFileMsgGet)
{
    LONG64 iValue = 0;
    TCHAR szProgressText[16] = {0};
    u16 wSerIndex = 0;

    u16 wCliPostInsNo = strFileMsgGet->wCliPostInsNum;
    u16 wSerPostInsNo = strFileMsgGet->wSerPostInsNum;
    u16 wIndex = strFileMsgGet->wIndex;

    //CProgressUI* pProgress =  pFrame->m_pProgress;
#if 0
    // Server端，进度条绘画并显示;
    iValue = 100 * m_fileInfo.filePacketIndex / m_fileInfo.filePacketNum;
    wsprintf(szProgressText, L"Progress(%ld%%)", iValue);
    pProgress->SetValue((int)iValue);
    pProgress->SetText(szProgressText);

    //m_fileInfo.filePacketIndex++;
#endif

    // 根据instanceID，获取instance索引号;
    wSerIndex = FindInsIndex(wSerPostInsNo);
    OspPrintf(TRUE, FALSE, "Server: FindInsIndex: %d.\r\n", wSerIndex);
    if (wSerIndex == MAX_INS_NO + 1)
    {
        OspPrintf(TRUE, FALSE, "Can't find the instance index.\r\n");
        return;
    }

    // 改成外置控制属性;
    g_pvcFilePstInsNo[wSerIndex]->m_nPktIndex++;
    // 收到的包为最后一个包;
    if (g_pvcFilePstInsNo[wSerIndex]->m_nPktIndex == g_pvcFilePstInsNo[wSerIndex]->m_tFileInfo.filePacketNum)
    {
        if (StoreFilePacket(strFileMsgGet, wSerIndex) == TRUE)
        {
            OspPrintf(TRUE, FALSE, "Server: End package.\r\n");
            SendFilePacketEcho(FILE_PACKET_END, wCliPostInsNo, wSerPostInsNo, wIndex);
#if 0
            //进度条置顶，完成文件传输;
            pProgress->SetValue(100);
            pProgress->SetText(L"Progress(100%)");

            // 文件校验并显示结果; --TODO
            ::MessageBox(NULL, _T("已完成文件传输！文件检验OK!!"), _T("文件传输结果"), NULL);
#endif
            InsRelease(wCliPostInsNo, wSerPostInsNo);
            return;
        }

        OspPrintf(TRUE, FALSE, "Server: Store file packet fail!\r\n");
        SendFilePacketEcho(FILE_PACKET_ERROR, wCliPostInsNo, wSerPostInsNo, wIndex);
        return;
    }
    // 收到的包为正常包，且还有后续包;
    else if (g_pvcFilePstInsNo[wSerIndex]->m_nPktIndex < g_pvcFilePstInsNo[wSerIndex]->m_tFileInfo.filePacketNum)
    {
        if (StoreFilePacket(strFileMsgGet, wSerIndex) == TRUE)
        {
            OspPrintf(TRUE, FALSE, "Server: Next package.\r\n");
            SendFilePacketEcho(FILE_PACKET_OK, wCliPostInsNo, wSerPostInsNo, wIndex);
            return;
        }

        OspPrintf(TRUE, FALSE, "Server: Store file packet fail!\r\n");
        SendFilePacketEcho(FILE_PACKET_ERROR, wCliPostInsNo, wSerPostInsNo, wIndex);
        return;
    }
    else
    {
        OspPrintf(TRUE, FALSE, "Server: Not current packet to save!\r\n");
        SendFilePacketEcho(FILE_PACKET_ERROR, wCliPostInsNo, wSerPostInsNo, wIndex);
    }
    return;
}

// 包接收函数;
void OnServerReceive(CMessage *const pMsg)
{
    OspPrintf(TRUE, FALSE, "server receive the message from client\n");
    // 获取收到的消息内容;
    u16 wMsgLen = pMsg->length;
    s8 *pchMsgGet = new char[wMsgLen + 1];
    ZeroMemory(pchMsgGet, wMsgLen + 1);
    memcpy_s(pchMsgGet, wMsgLen, pMsg->content, wMsgLen);

    TFileMessage *strFileMsgGet = (TFileMessage *)pchMsgGet;

    // 正常接受包，做接受处理工作，并回复一个确认报告诉client继续发送下一个包;
    if (!strncmp(strFileMsgGet->fileHead, "FFF", 3))
    {
        ReceiveFilePacket(strFileMsgGet);
    }
    // 接收client发送的错误信息，中止文件传输;
    else if (!strncmp(strFileMsgGet->fileHead, "ERR", 3))
    {
        // -- TODO:删除文件;
        //SendFilePacketEcho(FILE_PACKET_CANCEL);
    }

    strFileMsgGet = NULL;
    delete [] pchMsgGet;
    pchMsgGet = NULL;

    return;
}

// InstanceEntry消息类型分发处理入口;
void CServerInstance::InstanceEntry(CMessage *const pMsg)
{
    /*得到当前消息的类型;*/
    u16 wCurEvent = pMsg->event;

    // 根据不同的消息类型进行处理;
    switch (wCurEvent)
    {
    case EVENT_MSG_POST:
        MsgPostFunc(pMsg);
        NextState(STATE_WORK);
        break;
    case EVENT_FILE_POST2S:
        OnServerReceive(pMsg);
        break;
    default:
        // Def_Fuction(pMsg);
        break;
    }

    return;
}


// DaemonInstanceEntry:event 消息处理函数定义;
//////////////////////////////////////////////

// 获取空闲的instanceID;
CServerInstance* GetIdleInsID(CApp* pcApp)
{
    u16 wIndex = 0;
    CServerInstance *pCInst = NULL;

    // 遍历所有instance，寻找到第一个空闲的instance并返回;
    for (wIndex = 2; wIndex <= MAX_INS_NO; wIndex++)
    {
        pCInst = (CServerInstance*)pcApp->GetInstance(wIndex);
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
        return 0;
    }

    return pCInst;
}

// 服务端文件发送instance分配;
void ServerFilePostInsAllot(CMessage *const pcMsg, CApp* pcApp)
{
	u16 wMsgLen = 0;
	s8 *pchMsgGet = NULL;
	
	// 消息内容提取;
	wMsgLen = pcMsg->length;
	pchMsgGet = new char[wMsgLen + 1];
	ZeroMemory(pchMsgGet, wMsgLen + 1);
	memcpy_s(pchMsgGet, wMsgLen, pcMsg->content, wMsgLen);
	//OspPrintf(TRUE, FALSE, "message content is: %s, length is: %d.\n", pchMsgGet, wMsgLen);

	// 获取空闲的instance指针;
	CServerInstance *pcIns = GetIdleInsID(pcApp);
	if (pcIns == 0)
	{
		OspPrintf(TRUE, FALSE, "Server:have none idle instance.\r\n");
		return;
	}

	TFileInfo *tFileInfo = (TFileInfo *)pchMsgGet;

	// 数据获取;
	pcIns->m_tFileInfo.fileLength = tFileInfo->fileLength;
	pcIns->m_tFileInfo.filePacketNum = tFileInfo->filePacketNum;
	ZeroMemory(pcIns->m_tFileInfo.strFileName, MAX_FILE_NAME + 1);
	memcpy_s(pcIns->m_tFileInfo.strFileName, MAX_FILE_NAME, tFileInfo->strFileName, (wMsgLen - 2*sizeof(u32)));

    // 文件存储路径处理;
    USES_CONVERSION;
    ZeroMemory(g_strFilePath, MAX_PATH);
    lstrcat(g_strFilePath, g_strFolderPath);
    lstrcat(g_strFilePath, L"\\");
    lstrcat(g_strFilePath, A2W(pcIns->m_tFileInfo.strFileName));

    ZeroMemory(pcIns->m_strFilePath, MAX_PATH);
    lstrcpy(pcIns->m_strFilePath, g_strFilePath);

    // 判断文件是否存在, 存在就删除;  //后续需更改为加后缀传送;
    if(PathFileExists(g_strFilePath))
    {
        remove((CW2A)g_strFilePath);
    }

	g_pvcFilePstInsNo.push_back(pcIns);

	// Instance申请消息回复;
	OspPost(pcMsg->srcid, EVENT_SERVER_FILE_POST_INS_ALLOT_ACK,
		NULL, 0, g_wNodeNum, MAKEIID(DEMO_APP_SERVER_NO, pcIns->m_instId), 0, DEMO_POST_TIMEOUT);

    return;
}

// 服务端文件发送instance释放;
void SerFilePostInsRelease(CMessage *const pcMsg, CApp* pcApp)
{
	u16 wInsid = 0;
	u16 wIndex = 0;

	// 获取收到的消息内容;
	u16 wMsgLen = pcMsg->length;
	char *pchMsgGet = new char[wMsgLen + 1];
	ZeroMemory(pchMsgGet, wMsgLen + 1);
	memcpy_s(pchMsgGet, wMsgLen, pcMsg->content, wMsgLen);

	wInsid = atoi(pchMsgGet);

	// 释放全局变量;
    vector<CServerInstance*>::iterator itIndex;
    for (itIndex = g_pvcFilePstInsNo.begin(); itIndex != g_pvcFilePstInsNo.end(); itIndex++)
	{
		if ((*itIndex)->GetInsID() == wInsid)
		{
			//g_pvcFilePstInsNo.erase(itIndex);
            break;
		}
	}

	// 释放instance资源;
	CInstance *pCInst = pcApp->GetInstance(wInsid);
	pCInst->m_curState = IDLE_STATE;
	pCInst->m_lastState = STATE_WORK;
}

// DaemonInstanceEntry消息分发处理入口;
void CServerInstance::DaemonInstanceEntry(CMessage *const pcMsg, CApp* pcApp)
{
    /*得到当前消息的类型;*/
    u16 wCurEvent = pcMsg->event;

    switch (wCurEvent)
    {
    case EVENT_SERVER_FILE_POST_INS_ALLOT:
        ServerFilePostInsAllot(pcMsg, pcApp);
        NextState(STATE_WORK);
        break;
	case EVENT_SERVER_FILE_POST_INS_RELEASE:
		SerFilePostInsRelease(pcMsg, pcApp);
		NextState(STATE_WORK);
		break;
    default:
        // Def_Fuction(pMsg);
        break;
    }
    
    return;
}
