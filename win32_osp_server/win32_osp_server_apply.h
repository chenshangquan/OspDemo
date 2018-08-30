#pragma once

#include "../include/ospdemo_com.h"

#define MAX_INS_NO 20
//map< u16, vector<CDemoInstance*> > g_mInfo;
vector<CDemoInstance*> g_pvcFilePstInsNo;

enum FILE_PACKET_TYPE
{
    FILE_PACKET_OK = 1,
    FILE_PACKET_ERROR,
    FILE_PACKET_END,
    FILE_PACKET_CANCEL
};

// ����intance ID���ҵ���Ӧ��ȫ�ֱ�������;
u16 FindInsIndex(u16 wSerPostInsNo)
{
    u16 wIndex = 0;

    // ����server���ļ����͵�instance ID���ҵ���Ӧ��������ȡ�ļ�д��·����Ϣ;
	for (wIndex = 0; wIndex < g_pvcFilePstInsNo.size(); wIndex++)
	{
		if (g_pvcFilePstInsNo[wIndex]->m_instId == wSerPostInsNo)
        {
            return wIndex;
        }
    }

    return (MAX_INS_NO + 1);
}

// Instance�ͷ�;
void InsRelease(u16 wCliPostInsNo, u16 wSerPostInsNo)
{
	s8 achCliPostInsNoStr[4] = {0};
	s8 achSerPostInsNoStr[4] = {0};

	// �ͷ�Client���ļ����͵�instance;
	sprintf(achCliPostInsNoStr, "%d", wCliPostInsNo);
	OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_CLIENT_FILE_POST_INS_RELEASE,
		achCliPostInsNoStr, strlen(achCliPostInsNoStr), g_wNodeNum, MAKEIID(DEMO_APP_SERVER_NO, wSerPostInsNo), 0, DEMO_POST_TIMEOUT);

	// �ͷ�Server���ļ����͵�instance;
	sprintf(achSerPostInsNoStr, "%d", wCliPostInsNo);
	OspPost(MAKEIID(DEMO_APP_SERVER_NO, CInstance::DAEMON), EVENT_SERVER_FILE_POST_INS_RELEASE,
		achSerPostInsNoStr, strlen(achSerPostInsNoStr), 0, MAKEIID(DEMO_APP_SERVER_NO, wSerPostInsNo), 0, DEMO_POST_TIMEOUT);
}

/****************************************************
 *
 * CDemoInstance ��������;
 *
 ****************************************************/

// InstanceEntry:event ��Ϣ����������;
/////////////////////////////////////////////////////

// ��Ϣ���մ�����;
void MsgPostFunc(CMessage *const pMsg)
{
    u16 wMsgLen = 0;
    s8 *pchMsgGet = NULL;

    // ��Ϣ������ȡ;
    wMsgLen = pMsg->length;
    
    // �������Ϣ��һ�����ڷ���˻�ȡsrcnode;
    if (wMsgLen == 0)
    {
        g_wNodeNum = pMsg->srcnode;
        return;
    }

    pchMsgGet = new char[wMsgLen + 1];
    ZeroMemory(pchMsgGet, wMsgLen + 1);
    memcpy_s(pchMsgGet, wMsgLen, pMsg->content, wMsgLen);
    OspPrintf(TRUE, FALSE, "message content is: %s, length is: %d.\n", pchMsgGet, wMsgLen);

    // ���ڸ�ֵ;
    CEditUI* pEditRecv =  pFrame->m_pEditRecv;
    pEditRecv->SetText(CA2W(pchMsgGet));

    // �ͷſռ�;
    delete [] pchMsgGet;
    pchMsgGet = NULL;
}

// �����ܵ��İ�����д���ļ�;
bool StoreFilePacket(TFileMessage *strFileMsgGet, u16 wSerIndex)
{
    DWORD nByte;

    OspPrintf(TRUE, FALSE, "Server: Start to store the packet\r\n");

    // ��¼����ƫ��λ��;
    g_pvcFilePstInsNo[wSerIndex]->m_nLastStart = strFileMsgGet->fileStart;
    g_pvcFilePstInsNo[wSerIndex]->m_nLastSize  = strFileMsgGet->fileSize;

    // �����ļ���д���ļ�;
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

// �ظ�����亯��;
void SendFilePacketEcho(s32 nFlag, u16 wCliPostInsNo, u16 wSerPostInsNo, u16 wIndex)    //��Client����ȷ�ϰ���m_fileMsgSend��CMySocket�ı���;
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

    // ����instanceID����ȡinstance������;
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
    // ���Ͱ����ͻ���
    OspPost(MAKEIID(DEMO_APP_CLIENT_NO, wCliPostInsNo), EVENT_FILE_POST2C,
        newPacket, wLen, g_wNodeNum, MAKEIID(DEMO_APP_SERVER_NO, wSerPostInsNo));
}

// ���ܰ�������;
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
    // Server�ˣ��������滭����ʾ;
    iValue = 100 * m_fileInfo.filePacketIndex / m_fileInfo.filePacketNum;
    wsprintf(szProgressText, L"Progress(%ld%%)", iValue);
    pProgress->SetValue((int)iValue);
    pProgress->SetText(szProgressText);

    //m_fileInfo.filePacketIndex++;
#endif

    // ����instanceID����ȡinstance������;
    wSerIndex = FindInsIndex(wSerPostInsNo);
    OspPrintf(TRUE, FALSE, "Server: FindInsIndex: %d.\r\n", wSerIndex);
    if (wSerIndex == MAX_INS_NO + 1)
    {
        OspPrintf(TRUE, FALSE, "Can't find the instance index.\r\n");
        return;
    }

    // �ĳ����ÿ�������;
    g_pvcFilePstInsNo[wSerIndex]->m_nPktIndex++;
    // �յ��İ�Ϊ���һ����;
    if (g_pvcFilePstInsNo[wSerIndex]->m_nPktIndex == g_pvcFilePstInsNo[wSerIndex]->m_tFileInfo.filePacketNum)
    {
        if (StoreFilePacket(strFileMsgGet, wSerIndex) == TRUE)
        {
            OspPrintf(TRUE, FALSE, "Server: End package.\r\n");
            SendFilePacketEcho(FILE_PACKET_END, wCliPostInsNo, wSerPostInsNo, wIndex);
#if 0
            //�������ö�������ļ�����;
            pProgress->SetValue(100);
            pProgress->SetText(L"Progress(100%)");

            // �ļ�У�鲢��ʾ���; --TODO
            ::MessageBox(NULL, _T("������ļ����䣡�ļ�����OK!!"), _T("�ļ�������"), NULL);
#endif
            InsRelease(wCliPostInsNo, wSerPostInsNo);
            return;
        }

        OspPrintf(TRUE, FALSE, "Server: Store file packet fail!\r\n");
        SendFilePacketEcho(FILE_PACKET_ERROR, wCliPostInsNo, wSerPostInsNo, wIndex);
        return;
    }
    // �յ��İ�Ϊ���������һ��к�����;
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

// �����պ���;
void OnServerReceive(CMessage *const pMsg)
{
    OspPrintf(TRUE, FALSE, "server receive the message from client\n");
    // ��ȡ�յ�����Ϣ����;
    u16 wMsgLen = pMsg->length;
    s8 *pchMsgGet = new char[wMsgLen + 1];
    ZeroMemory(pchMsgGet, wMsgLen + 1);
    memcpy_s(pchMsgGet, wMsgLen, pMsg->content, wMsgLen);

    TFileMessage *strFileMsgGet = (TFileMessage *)pchMsgGet;

    // �������ܰ��������ܴ����������ظ�һ��ȷ�ϱ�����client����������һ����;
    if (!strncmp(strFileMsgGet->fileHead, "FFF", 3))
    {
        ReceiveFilePacket(strFileMsgGet);
    }
    // ����client���͵Ĵ�����Ϣ����ֹ�ļ�����;
    else if (!strncmp(strFileMsgGet->fileHead, "ERR", 3))
    {
        // -- TODO:ɾ���ļ�;
        //SendFilePacketEcho(FILE_PACKET_CANCEL);
    }

    strFileMsgGet = NULL;
    delete [] pchMsgGet;
    pchMsgGet = NULL;

    return;
}

// �ļ���Ϣ��ʼ��;
#if 0
void FileInfoInit(u16 wIndex)
{
    // ��ʼ���ļ���Ϣ����;
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

// InstanceEntry��Ϣ���ͷַ��������;
void CDemoInstance::InstanceEntry(CMessage *const pMsg)
{
    /*�õ���ǰ��Ϣ������;*/
    u16 wCurEvent = pMsg->event;

    // ���ݲ�ͬ����Ϣ���ͽ��д���;
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


// DaemonInstanceEntry:event ��Ϣ����������;
//////////////////////////////////////////////

// ��ȡ���е�instanceID;
CDemoInstance* GetIdleInsID(CApp* pcApp)
{
    u16 wIndex = 0;
    CDemoInstance *pCInst = NULL;

    // ��������instance��Ѱ�ҵ���һ�����е�instance������;
    for (wIndex = 2; wIndex <= MAX_INS_NO; wIndex++)
    {
        pCInst = (CDemoInstance*)pcApp->GetInstance(wIndex);
        if (pCInst->CurState() == IDLE_STATE)
        {
            pCInst->m_curState = STATE_WORK;
            pCInst->m_lastState = IDLE_STATE;
            break;
        }
    }

    // ��ǰû�п��е�instance;
    if (wIndex > MAX_INS_NO)
    {
        OspPrintf(TRUE, FALSE, "Current have none idle instance.\r\n");
        return 0;
    }

    return pCInst;
}

// ������ļ�����instance����;
void ServerFilePostInsAllot(CMessage *const pcMsg, CApp* pcApp)
{
	u16 wMsgLen = 0;
	s8 *pchMsgGet = NULL;
	
	// ��Ϣ������ȡ;
	wMsgLen = pcMsg->length;
	pchMsgGet = new char[wMsgLen + 1];
	ZeroMemory(pchMsgGet, wMsgLen + 1);
	memcpy_s(pchMsgGet, wMsgLen, pcMsg->content, wMsgLen);
	//OspPrintf(TRUE, FALSE, "message content is: %s, length is: %d.\n", pchMsgGet, wMsgLen);

	// ��ȡ���е�instanceָ��;
	CDemoInstance *pcIns = GetIdleInsID(pcApp);
	if (pcIns == 0)
	{
		OspPrintf(TRUE, FALSE, "Server:have none idle instance.\r\n");
		return;
	}

	TFileInfo *tFileInfo = (TFileInfo *)pchMsgGet;

	// ���ݻ�ȡ;
	pcIns->m_tFileInfo.fileLength = tFileInfo->fileLength;
	pcIns->m_tFileInfo.filePacketNum = tFileInfo->filePacketNum;
	ZeroMemory(pcIns->m_tFileInfo.strFileName, MAX_FILE_NAME + 1);
	memcpy_s(pcIns->m_tFileInfo.strFileName, MAX_FILE_NAME, tFileInfo->strFileName, (wMsgLen - 2*sizeof(u32)));

    // �ļ��洢·������;
    USES_CONVERSION;
    ZeroMemory(g_strFilePath, MAX_PATH);
    lstrcat(g_strFilePath, g_strFolderPath);
    lstrcat(g_strFilePath, L"\\");
    lstrcat(g_strFilePath, A2W(pcIns->m_tFileInfo.strFileName));

    ZeroMemory(pcIns->m_strFilePath, MAX_PATH);
    lstrcpy(pcIns->m_strFilePath, g_strFilePath);

    // �ж��ļ��Ƿ����, ���ھ�ɾ��;  //���������Ϊ�Ӻ�׺����;
    if(PathFileExists(g_strFilePath))
    {
        remove((CW2A)g_strFilePath);
    }

	g_pvcFilePstInsNo.push_back(pcIns);

	// Instance������Ϣ�ظ�;
	OspPost(pcMsg->srcid, EVENT_SERVER_FILE_POST_INS_ALLOT_ACK,
		NULL, 0, g_wNodeNum, MAKEIID(DEMO_APP_SERVER_NO, pcIns->m_instId), 0, DEMO_POST_TIMEOUT);

    return;
}

// ������ļ�����instance�ͷ�;
void SerFilePostInsRelease(CMessage *const pcMsg, CApp* pcApp)
{
	u16 wInsid = 0;
	u16 wIndex = 0;

	// ��ȡ�յ�����Ϣ����;
	u16 wMsgLen = pcMsg->length;
	char *pchMsgGet = new char[wMsgLen + 1];
	ZeroMemory(pchMsgGet, wMsgLen + 1);
	memcpy_s(pchMsgGet, wMsgLen, pcMsg->content, wMsgLen);

	wInsid = atoi(pchMsgGet);

	// �ͷ�ȫ�ֱ���;
    vector<CDemoInstance*>::iterator itIndex;
    for (itIndex = g_pvcFilePstInsNo.begin(); itIndex != g_pvcFilePstInsNo.end(); itIndex++)
	{
		if ((*itIndex)->GetInsID() == wInsid)
		{
			//g_pvcFilePstInsNo.erase(itIndex);
            break;
		}
	}

	// �ͷ�instance��Դ;
	CInstance *pCInst = pcApp->GetInstance(wInsid);
	pCInst->m_curState = IDLE_STATE;
	pCInst->m_lastState = STATE_WORK;
}

// DaemonInstanceEntry��Ϣ�ַ��������;
void CDemoInstance::DaemonInstanceEntry(CMessage *const pcMsg, CApp* pcApp)
{
    /*�õ���ǰ��Ϣ������;*/
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

CDemoInstance::~CDemoInstance()
{

}

// �Զ���������װ����;
//////////////////////////////////////////////

// �ж�IP�ַ����ĺϷ���;
BOOL IsIpFormatRight(LPCTSTR pIpAddr)
{
    s32 dwCount = 0;
    s32 i = 0;
    s32 dwA, dwB, dwC, dwD;

    // ����Ƿ�ֻ�����������;
    for(i = 0; pIpAddr[i] != '\0'; i++)
    {
        if(!isdigit(pIpAddr[i]) && pIpAddr[i] != '.')
            return FALSE;
    }

    // �����ʽ�Ƿ�ΪX.X.X.X;
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

    // �������ĺϷ���;
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