#pragma once

#include "../include/ospdemo_com.h"

#define MAX_INS_NO 10

/****************************************************
 *
 * CDemoInstance ��������;
 *
 ****************************************************/

// InstanceEntry:event ��Ϣ����������;
////////////////////////////////////////

// �����ʹ�����;
void SendFileInfo(s32 fStart,s32 fSize,char *fHead, u16 wCliPostInsNo, u16 wSerPostInsNo, u16 wIndex)
{
    OspPrintf(TRUE, FALSE, "SendFileInfo.\r\n");
    u16 wListIndex = 0;
    u16 wListItemNum = 0;
    s8 achProgress[MAX_FILE_NAME] = {0};
    //s8 achBtnName[MAX_STR_LEN]  = {0};
    s8 achPrgName[MAX_STR_LEN]  = {0};
    //s8 achTmpName[MAX_FILE_NAME]  = {0};
    BYTE m_sendBuffer[MAX_FILE_PACKET + 1] = {0};   // ����������buffer;

    ZeroMemory(achPrgName, MAX_STR_LEN);
    sprintf(achPrgName, "DemoProgress%u", wIndex);

    // �ļ����������ֵ;
    g_pvcFilePstInsNo[wIndex]->m_dnProgValve = 100 * g_pvcFilePstInsNo[wIndex]->m_nPktIndex / g_pvcFilePstInsNo[wIndex]->m_tFileInfo.filePacketNum;

    u32 dwFilePacketBuff = MAX_FILE_PACKET - 4 - 2*sizeof(s32) - 3*sizeof(u16);

    TFileMessage strFileMsg;
    // �������ڷ����ļ����ݵ��ļ�����fileHead����Ϊ��FFF��;
    strFileMsg.fileHead[0] = 'F';
    strFileMsg.fileHead[1] = 'F';
    strFileMsg.fileHead[2] = 'F';
    strFileMsg.fileHead[3] = '\0';

    // ���м�¼�ͻ���/����˵ķ���instance ID;
    strFileMsg.wCliPostInsNum = wCliPostInsNo;
    strFileMsg.wSerPostInsNum = wSerPostInsNo;

    // ���м�¼�ͻ��˵ķ���instance ����ֵ;
    strFileMsg.wIndex = wIndex;

    // ��ʼ��;
    strFileMsg.fileStart   = 0;
    strFileMsg.fileSize    = 0;

    // �Ƿ�����ϴηְ���ƫ��λ��;
    if (fStart == g_pvcFilePstInsNo[wIndex]->m_nLastStart && fSize == g_pvcFilePstInsNo[wIndex]->m_nLastSize)
    {
        // ���һ�����������;
        if (!strcmp(fHead, "END"))
        {
            OspPrintf(TRUE, FALSE, "Enter END.\r\n");
#if 0
            //�������ö�������ļ�����;
            pProgress->SetValue(100);
            pProgress->SetText(L"Progress(100%)");

            // �ļ�У�鲢��ʾ���; --TODO
            ::MessageBox(NULL, _T("������ļ����䣡�ļ�����OK!!"), _T("�ļ�������"), NULL);
            USES_CONVERSION;
            // ��ȡ�ؼ�������ָ��;
            pcPrg = static_cast<CProgressUI*>(pFrame->m_pm.FindControl(A2W(achPrgName)));

            // ��������ֵ;
            ZeroMemory(achProgress, MAX_FILE_NAME);
            sprintf(achProgress, "%s(%d%%)", g_tInsNo[wIndex].m_tFileInfo.strFileName, 100);
            pcPrg->SetValue(100);
            pcPrg->SetText(A2W(achProgress));

            pcLoadList->RemoveAt(wIndex);
#endif
			// �������ʱ�����Լ�������Ϣ�����н������Ļ���;
			OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_LAST_PROGRESS_UI_PAINT, NULL,
				0, 0, MAKEIID(DEMO_APP_CLIENT_NO, wCliPostInsNo), 0, DEMO_POST_TIMEOUT);

            return;
        }
        // ���͵�һ����;
        else if (fStart == 0 && fSize == 0 && !strcmp(fHead,"0"))
        {
            // ��������־����������0;
            g_pvcFilePstInsNo[wIndex]->m_nErrorPktNum = 0;
            g_pvcFilePstInsNo[wIndex]->m_nPktIndex   = 0;

            // ����ֻ��һ���������;
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

			// ���͵�һ����ʱ�����Լ�������Ϣ�����н������Ļ���;
			OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_FIRST_PROGRESS_UI_PAINT, NULL,
				0, 0, MAKEIID(DEMO_APP_CLIENT_NO, wCliPostInsNo), 0, DEMO_POST_TIMEOUT);
        }
        // ���յ�server�˻ظ�������ȷ�ϰ�;
        else if (!strcmp(fHead,"OK!"))
        {
#if 0  // ������߼���������Ϣʵ�ֻ�ͼ;

            USES_CONVERSION;
            CProgressUI* pcPrg = static_cast<CProgressUI*>(pFrame->m_pm.FindControl(A2W(achPrgName)));
            if (pcPrg != NULL)
            {
                OspPrintf(TRUE, FALSE, "Can't find the progress control\r\n");
                // ��������ֵ;
                ZeroMemory(achProgress, MAX_FILE_NAME);
                sprintf(achProgress, "%s(%ld%%)", g_pvcFilePstInsNo[wIndex]->m_tFileInfo.strFileName, g_pvcFilePstInsNo[wIndex]->m_dnProgValve);
                pcPrg->SetValue((s32)g_pvcFilePstInsNo[wIndex]->m_dnProgValve);
                pcPrg->SetText(A2W(achProgress));
            }
#endif
            // ��������־��0;
            g_pvcFilePstInsNo[wIndex]->m_nErrorPktNum = 0;
            // �����������ۼ�;
            g_pvcFilePstInsNo[wIndex]->m_nPktIndex++;

            // ��Ҫ�������һ������ʱ��;
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

			// ��������������ʱ�����Լ�������Ϣ�����н������Ļ���;
			OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_NORMAL_PROGRESS_UI_PAINT, NULL,
				0, 0, MAKEIID(DEMO_APP_CLIENT_NO, wCliPostInsNo), 0, DEMO_POST_TIMEOUT);
        }
        // ȷ�ϰ���ʾ���յ��İ��д��������·�����һ����;
        else if (!strcmp(fHead, "ERR"))   
        {
            OspPrintf(TRUE, FALSE, "Error packet. Index(%d)\r\n",  g_pvcFilePstInsNo[wIndex]->m_nPktIndex);
            g_pvcFilePstInsNo[wIndex]->m_nErrorPktNum++;
            strFileMsg.fileStart = g_pvcFilePstInsNo[wIndex]->m_nLastStart;
            strFileMsg.fileSize  = g_pvcFilePstInsNo[wIndex]->m_nLastSize;
        }
        // ȷ�ϰ���ʾ�û�ȡ�����ļ�����;
        else if (!strcmp(fHead, "CCL"))
        {
            string temp=("");
            //GetDlgItemText(IDC_EDIT_CLIENT,temp);
            //temp += _T("\r\n");
            //temp += _T("�Է�ȡ�����ļ�����");
            //SetDlgItemText(IDC_EDIT_CLIENT,temp);
            return;
        }
        // ��ͣ����ʾ�û�ֹͣ�ļ�����;
        else if (!strcmp(fHead, "STP"))
        {
            OspPrintf(TRUE, FALSE, "stop the packet send.\r\n\r\n\r\n");
            Sleep(1);
        }
        // ���յ�����Ϣ������ʱĬ�ϳ������·�����һ����;
        else
        {
            OspPrintf(TRUE, FALSE, "Repeat the previous packet. Index(%d)\r\n",  g_pvcFilePstInsNo[wIndex]->m_nPktIndex);
            g_pvcFilePstInsNo[wIndex]->m_nErrorPktNum++;
            strFileMsg.fileStart = g_pvcFilePstInsNo[wIndex]->m_nLastStart;
            strFileMsg.fileSize  = g_pvcFilePstInsNo[wIndex]->m_nLastSize;
        }
    }
    // ���յ�����Ϣ������ʱĬ�ϳ������·�����һ����;
    else 
    {
        OspPrintf(TRUE, FALSE, "Other error, Repeat the previous packet. Index(%d)\r\n",  g_pvcFilePstInsNo[wIndex]->m_nPktIndex);
        g_pvcFilePstInsNo[wIndex]->m_nErrorPktNum++;
        strFileMsg.fileStart = g_pvcFilePstInsNo[wIndex]->m_nLastStart;
        strFileMsg.fileSize  = g_pvcFilePstInsNo[wIndex]->m_nLastSize;
    }

    // ������Խ��г�����;
    if (g_pvcFilePstInsNo[wIndex]->m_nErrorPktNum > 10)       
    {
        strFileMsg.fileHead[0] = 'E';
        strFileMsg.fileHead[1] = 'R';
        strFileMsg.fileHead[2] = 'R';
        strFileMsg.fileHead[3] = '\0';
    }

    ZeroMemory(m_sendBuffer, sizeof(BYTE)*(MAX_FILE_PACKET) + 1);
    ZeroMemory(strFileMsg.filePacket, sizeof(BYTE)*(MAX_FILE_PACKET) - 4 - 2*sizeof(s32) - 3*sizeof(u16));

    // ��ȡһ����������;
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
    // �������Ͱ���������������ͻ�����;
    memcpy(m_sendBuffer, filePacket, strFileMsg.fileSize + 4 + 2*sizeof(s32) + 3*sizeof(u16));

    OspPrintf(TRUE, FALSE, "Start to post the file message to server. InsNo:%d\r\n", wCliPostInsNo);

    // ���Ͱ��������;
    OspPost(MAKEIID(DEMO_APP_SERVER_NO, wSerPostInsNo), EVENT_FILE_POST2S, m_sendBuffer,
        (strFileMsg.fileSize + 4 + 2*sizeof(s32) + 3*sizeof(u16)), g_wNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, wCliPostInsNo));
}

// �������˷����ļ�����instance�ظ�;
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
	// ���͵�һ����;
	SendFileInfo(0, 0, "0", wCliPostInsNo, wSerPostInsNo, wVecIndex);
	return;
}

// ��Ϣ���մ�����;
void MsgPostFunc(CMessage *const pMsg)
{
    u16 wMsgLen = 0;
    s8 *pchMsgGet = NULL;

    // ��Ϣ������ȡ;
    wMsgLen = pMsg->length;

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

// �����server�˵Ļظ�����������һ�ε��ļ�����;
void OnClientReceive(CMessage *const pMsg)
{
    OspPrintf(TRUE, FALSE, "Client: Receive the response package\r\n");
    // ��ȡ�յ�����Ϣ����;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];
    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                

    // ����������;
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

    // ��ͣ����;
    while(g_pvcFilePstInsNo[wIndex]->m_nPuase != 0)
    {
        Sleep(100);
    }

    OspPrintf(TRUE, FALSE, "Client: Next send, wCliPostInsNo: %d, wSerPostInsNo: %d, wIndex: %d\r\n", wCliPostInsNo, wSerPostInsNo, wIndex);
    // ��ȡ��Ϣ���ʹ���;
    SendFileInfo(nFileStart, nFileSize, achFileHead, wCliPostInsNo, wSerPostInsNo, wIndex);

    delete [] strMsgGet;
    strMsgGet = NULL;
}

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


// DaemonInstanceEntry:event ��Ϣ����������;
//////////////////////////////////////////////

// ��ȡ���е�instanceָ��;
CDemoInstance* GetIdleInsID(CApp* pcApp)
{
    u16 wIndex = 0;
    CDemoInstance *pCInst = NULL;

    // ��������instance��Ѱ�ҵ�һ�����е�instance������;
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

    // ��ǰû�п��е�instance;
    if (wIndex > MAX_INS_NO)
    {
        OspPrintf(TRUE, FALSE, "Current have none idle instance.\r\n");
        return NULL;
    }

    return pCInst;
}

// �ͻ����ļ�����instance����;
void ClientFilePostInsAllot(CMessage *const pcMsg, CApp* pcApp)
{
	u16 wMsgLen = 0;

	// ��Ϣ������ȡ;
	wMsgLen = pcMsg->length;
	s8 *pchMsgGet = new char[wMsgLen + 1];
	ZeroMemory(pchMsgGet, wMsgLen + 1);
	memcpy_s(pchMsgGet, wMsgLen, pcMsg->content, wMsgLen);
	//OspPrintf(TRUE, FALSE, "message content is: %s, length is: %d.\n", pchMsgGet, wMsgLen);

    // ��ȡ���е�instanceָ��;
    CDemoInstance *pcIns = GetIdleInsID(pcApp);
    if (pcIns == 0)
    {
        OspPrintf(TRUE, FALSE, "Client:have none idle instance.\r\n");
        return;
    }

	TFileInfo *tFileInfo = (TFileInfo *)pchMsgGet;

	// ���ݻ�ȡ;
	pcIns->m_tFileInfo.fileLength = tFileInfo->fileLength;
	pcIns->m_tFileInfo.filePacketNum = tFileInfo->filePacketNum;
	ZeroMemory(pcIns->m_tFileInfo.strFileName, MAX_FILE_NAME + 1);
	memcpy_s(pcIns->m_tFileInfo.strFileName, MAX_FILE_NAME, tFileInfo->strFileName, (wMsgLen - 2*sizeof(u32)));
    
    // �ļ���ȡ·����Ϣ����;
    ZeroMemory(pcIns->m_strFilePath, MAX_PATH);
    lstrcpy(pcIns->m_strFilePath, g_strFilePath);
    
    g_pvcFilePstInsNo.push_back(pcIns);
    OspPrintf(TRUE, FALSE, "Client: Get a idle instance, ID: %d, FileName: %s\r\n", pcIns->m_instId, pcIns->m_tFileInfo.strFileName);

	// �ļ���Ϣ��ȡ�󣬸��Լ�������Ϣ�������б�ͼ�λ���;
	OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_LIST_UI_PAINT, NULL,
		0, 0, MAKEIID(DEMO_APP_CLIENT_NO, pcIns->m_instId), 0, DEMO_POST_TIMEOUT);

	// �ͷſռ�;
	delete [] pchMsgGet;
	pchMsgGet = NULL;

    return;
}

// �ͻ����ļ�����instance�ͷ�;
#if 0
void CliFilePostInsRelease(CMessage *const pcMsg, CApp* pcApp)
{
	u16 wInsid = 0;
	u16 wIndex = 0;
	CInstance *pCInst = NULL;

	// ��ȡ�յ�����Ϣ����;
	u16 wMsgLen = pcMsg->length;
	char *pchMsgGet = new char[wMsgLen + 1];
	ZeroMemory(pchMsgGet, wMsgLen + 1);
	memcpy_s(pchMsgGet, wMsgLen, pcMsg->content, wMsgLen);

	wInsid = atoi(pchMsgGet);

    // �ͷ�ȫ�ֱ���;
    vector<TInStatus>::iterator itIndex;
    for (itIndex = g_tInsNo.begin(); itIndex != g_tInsNo.end(); itIndex++)
    {
        if (itIndex->uInsNum == wInsid)
        {
            g_tInsNo.erase(itIndex);
            break;
        }
    }

	// �ͷ�instance��Դ;
	pCInst = pcApp->GetInstance(wInsid);
	pCInst->m_curState = IDLE_STATE;
	pCInst->m_lastState = STATE_WORK;
}
#endif

u16 FindIndexByInsNo(u16 wInsNo)
{
	u16 wIndex = 0;
	// Ѱ��Insid��Ӧ������;
	for (wIndex = 0; wIndex < g_pvcFilePstInsNo.size(); wIndex++)
	{
		if (g_pvcFilePstInsNo[wIndex]->m_instId == wInsNo)
		{
			break;
		}
	}

	// ��ȡ����ʧ��;
	if (wIndex == g_pvcFilePstInsNo.size())
	{
		OspPrintf(TRUE, FALSE, "Find Index Failed.\r\n");
		return MAX_INS_NO;
	}

	return wIndex;
}

// �б���Ϣ����;
void ListUI2Paint()
{
	// ��ͼ;
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

	pcLoadList->RemoveAll();    // ����ػ�;
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

		// �������ؼ����;
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

		// ��ʼ�����ؼ����;
		ZeroMemory(achBtnName, MAX_STR_LEN);
		sprintf(achBtnName, "FileSttButton%u", g_pvcFilePstInsNo[wListIndex]->m_instId);
		pcButtonStt->ApplyAttributeList(_T("text=\"S\" width=\"17\" height=\"20\""));
		pcButtonStt->SetName(A2W(achBtnName));
		pcHorizontalLayout->Add(pcButtonStt);

		// ��ͣ�����ؼ����;
		ZeroMemory(achBtnName, MAX_STR_LEN);
		sprintf(achBtnName, "FileStpButton%u", g_pvcFilePstInsNo[wListIndex]->m_instId);
		pcButtonStp->ApplyAttributeList(_T("text=\"P\" width=\"17\" height=\"20\""));
		pcButtonStp->SetName(A2W(achBtnName));
		pcHorizontalLayout->Add(pcButtonStp);

		// ȡ�������ؼ����;
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

// ��һ��ͼ�λ���;
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

// ��������������;
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

	// ��������ֵ;
	ZeroMemory(achProgress, MAX_FILE_NAME);
	sprintf(achProgress, "%s(%ld%%)", g_pvcFilePstInsNo[wIndex]->m_tFileInfo.strFileName, g_pvcFilePstInsNo[wIndex]->m_dnProgValve);
	pcPrg->SetValue((s32)g_pvcFilePstInsNo[wIndex]->m_dnProgValve);
	pcPrg->SetText(A2W(achProgress));
	
	return;
}

// ��������б���Ϣ����;
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

	// ��������ֵ;
	ZeroMemory(achProgress, MAX_FILE_NAME);
	sprintf(achProgress, "%s(%ld%%)", g_pvcFilePstInsNo[wIndex]->m_tFileInfo.strFileName, 100);
	pcPrg->SetValue(100);
	pcPrg->SetText(A2W(achProgress));

	// ɾ��ָ���ؼ�;
	CListUI* pcLoadList = pFrame->m_pList;
	if (pcLoadList == NULL)
	{
		return;
	}

	CButtonUI* pcBtnCcl = static_cast<CButtonUI*>(pFrame->m_pm.FindControl(A2W(achBtnNameB)));
	pcLoadList->Remove(pcBtnCcl);
	return;
}

// DaemonInstanceEntry��Ϣ�ַ��������;
void CDemoInstance::DaemonInstanceEntry(CMessage *const pcMsg, CApp* pcApp)
{
    /*�õ���ǰ��Ϣ������;*/
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