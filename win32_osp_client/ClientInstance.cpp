#include "StdAfx.h"
#include "ClientInstance.h"
#include "win32_osp_client.h"

/****************************************************
 *
 * CClientInstance ��������;
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

// InstanceEntry:event ��Ϣ����������;
////////////////////////////////////////

// �����ʹ�����;
void CClientInstance::SendFileInfo(s32 fStart,s32 fSize,char *fHead)
{
    BYTE m_sendBuffer[MAX_FILE_PACKET + 1] = {0};   // ����������buffer;
	DWORD nByte;

    // �ļ����������ֵ;
    m_dnProgValve = 100 * m_dwPktIndex / m_tFileInfo.filePacketNum;

    u32 dwFilePacketBuff = MAX_FILE_PACKET - 4 - 2*sizeof(s32);

    TFileMessage strFileMsg;
    // �������ڷ����ļ����ݵ��ļ�����fileHead����Ϊ��FFF��;
    strFileMsg.fileHead[0] = 'F';
    strFileMsg.fileHead[1] = 'F';
    strFileMsg.fileHead[2] = 'F';
    strFileMsg.fileHead[3] = '\0';

    // ��ʼ��;
    strFileMsg.fileStart   = 0;
    strFileMsg.fileSize    = 0;
	ZeroMemory(m_sendBuffer, sizeof(BYTE)*(MAX_FILE_PACKET) + 1);
	ZeroMemory(strFileMsg.filePacket, sizeof(BYTE)*(MAX_FILE_PACKET) - 4 - 2*sizeof(s32));

	// ��ͣ����;  // ������ǰ����λ��;
	if (m_nPuase != 0)
	{
		return;
	}

    // �Ƿ�����ϴηְ���ƫ��λ��;
    if (fStart == m_nLastStart && fSize == m_nLastSize)
    {
        // ���һ�����������;
        if (!strcmp(fHead, "END"))
        {
            OspPrintf(TRUE, FALSE, "Enter END.\r\n");
			//FindClose(m_hFile);
			CloseHandle(m_hFile);

			// �ͷ���Դ�����»�ͼ;
			CliFilePostInsRelease(m_instId);

			// �������ʱ�����Լ�������Ϣ�����н������Ļ���;
			/*OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_LAST_PROGRESS_UI_PAINT, NULL,
				0, 0, MAKEIID(DEMO_APP_CLIENT_NO, m_instId), 0, DEMO_POST_TIMEOUT);*/

            return;
        }
        // ���͵�һ����;
        else if (fStart == 0 && fSize == 0 && !strcmp(fHead,"0"))
        {
            // ��������־��0����������1;
            m_nErrorPktNum = 0;
            m_dwPktIndex   = 1;

            // ����ֻ��һ���������;
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

			// ���͵�һ����ʱ�����Լ�������Ϣ�����н������Ļ���;
			OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_FIRST_PROGRESS_UI_PAINT, NULL,
				0, 0, MAKEIID(DEMO_APP_CLIENT_NO, m_instId), 0, DEMO_POST_TIMEOUT);
        }
        // ���յ�server�˻ظ�������ȷ�ϰ�;
        else if (!strcmp(fHead,"OK!"))
        {
            // ��������־��0;
            m_nErrorPktNum = 0;
            // �����������ۼ�;
            m_dwPktIndex++;

            // ��Ҫ�������һ������ʱ��;
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

			// ��������������ʱ�����Լ�������Ϣ�����н������Ļ���;
			OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_NORMAL_PROGRESS_UI_PAINT, NULL,
				0, 0, MAKEIID(DEMO_APP_CLIENT_NO, m_instId), 0, DEMO_POST_TIMEOUT);
        }
        // ȷ�ϰ���ʾ���յ��İ��д��������·�����һ����;
        else if (!strcmp(fHead, "ERR"))   
        {
            OspPrintf(TRUE, FALSE, "Error packet. Index(%d)\r\n",  m_dwPktIndex);
            m_nErrorPktNum++;
            strFileMsg.fileStart = m_nLastStart;
            strFileMsg.fileSize  = m_nLastSize;
        }
        // ȷ�ϰ���ʾ�û�ȡ�����ļ�����;
        else if (!strcmp(fHead, "CCL"))
        {
            OspPrintf(TRUE, FALSE, "Cancel the packet send.\r\n");
			// �ͷž��;
			//FindClose(m_hFile);
			CloseHandle(m_hFile);
            
			// �ͷ���Դ�����»�ͼ;
			CliFilePostInsRelease(m_instId);

            return;
        }
        // ��ͣ����ʾ�û�ֹͣ�ļ�����;
        else if (!strcmp(fHead, "STP"))
        {
            OspPrintf(TRUE, FALSE, "Stop the packet send.\r\n\r\n\r\n");
            Sleep(1);
        }
        // ���յ�����Ϣ������ʱĬ�ϳ������·�����һ����;
        else
        {
            OspPrintf(TRUE, FALSE, "Repeat the previous packet. Index(%d)\r\n",  m_dwPktIndex);
            m_nErrorPktNum++;
            strFileMsg.fileStart = m_nLastStart;
            strFileMsg.fileSize  = m_nLastSize;
        }
    }
    // ���յ�����Ϣ������ʱĬ�ϳ������·�����һ����;
    else 
    {
        OspPrintf(TRUE, FALSE, "Other error, Repeat the previous packet. Index(%d)\r\n", m_dwPktIndex);
        m_nErrorPktNum++;
        strFileMsg.fileStart = m_nLastStart;
        strFileMsg.fileSize  = m_nLastSize;
    }

    // ������Խ��г�����;
    if (m_nErrorPktNum > 10)       
    {
        strFileMsg.fileHead[0] = 'E';
        strFileMsg.fileHead[1] = 'R';
        strFileMsg.fileHead[2] = 'R';
        strFileMsg.fileHead[3] = '\0';
    }

	// ȡ��������;  // ����һ������;
	if (m_nCancel == 1)
	{
		strFileMsg.fileHead[0] = 'C';
		strFileMsg.fileHead[1] = 'C';
		strFileMsg.fileHead[2] = 'L';
		strFileMsg.fileHead[3] = '\0';

		//BYTE* filePacket = (BYTE*)&strFileMsg;
		// �������Ͱ���������������ͻ�����;
		memcpy(m_sendBuffer, (BYTE*)&strFileMsg, strFileMsg.fileSize + 4 + 2*sizeof(s32));

		// ����ȡ����;
		OspPost(MAKEIID(DEMO_APP_SERVER_NO, m_wSerInsNum), EVENT_FILE_POST2S, m_sendBuffer,
			(strFileMsg.fileSize + 4 + 2*sizeof(s32)), g_dwNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, m_instId));
	}

    // ��ȡһ����������;
    SetFilePointer(m_hFile, strFileMsg.fileStart, NULL, FILE_BEGIN);
    ReadFile(m_hFile, strFileMsg.filePacket, strFileMsg.fileSize, &nByte, NULL);

    BYTE* filePacket = (BYTE*)&strFileMsg;
    // �������Ͱ���������������ͻ�����;
    memcpy(m_sendBuffer, filePacket, strFileMsg.fileSize + 4 + 2*sizeof(s32));

    OspPrintf(TRUE, FALSE, "Start to post the file message to server. SerInsNo:%d\r\n", m_wSerInsNum);

    // ���Ͱ��������;
    OspPost(MAKEIID(DEMO_APP_SERVER_NO, m_wSerInsNum), EVENT_FILE_POST2S, m_sendBuffer,
        strFileMsg.fileSize + 4 + 2*sizeof(s32), g_dwNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, m_instId));
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

	if (pMsg->event == OSP_DISCONNECT)
	{
		return;
	}

    // ���ڸ�ֵ;
    pFrame->m_pEditRecv->SetText(CA2W(pchMsgGet));

    // �ͷſռ�;
    delete [] pchMsgGet;
    pchMsgGet = NULL;
}

// ������Ϣ����;
void OspDisconnect(CMessage *const pMsg)
{
    // ���ڸ�ֵ;
    OspPrintf(TRUE, FALSE, "DisConnect by app!!\r\n");
    pFrame->m_pEditMsg->SetText(_T("DisConnect!!"));
}

// �������˷����ļ�����instance�ظ�;
void CClientInstance::ClientFilePostInsAllotAck(CMessage *const pMsg)
{

	m_wSerInsNum = GETINS(pMsg->srcid);

    OspPrintf(TRUE, FALSE, "Client: wCliPostInsNo: %d, wSerPostInsNo: %d.\r\n", GETINS(pMsg->dstid), GETINS(pMsg->srcid));

    // ���͵�һ����;
	// �����ļ���������ļ�;
	m_hFile = CreateFile(m_strFilePath,	GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		DWORD dw = GetLastError();
		OspPrintf(TRUE, FALSE, "Client: CreateFile Failed!, Error: %d.\r\n", dw);
		return;
	}
	
	// ���͵�һ����;
    SendFileInfo(0, 0, "0");
    return;
}

// �����server�˵Ļظ�����������һ�ε��ļ�����;
void CClientInstance::OnClientReceive(CMessage *const pMsg)
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

    // ��ȡ��Ϣ���ʹ���;
    SendFileInfo(nFileStart, nFileSize, achFileHead);

    delete [] strMsgGet;
    strMsgGet = NULL;
}

// InstanceEntry��Ϣ���ͷַ��������;
void CClientInstance::InstanceEntry(CMessage *const pMsg)
{
    /*�õ���ǰ��Ϣ������;*/
    u16 wCurEvent = pMsg->event;

    // ���ݲ�ͬ����Ϣ���ͽ��д���;
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


// DaemonInstanceEntry:event ��Ϣ����������;
//////////////////////////////////////////////

// ��ȡ���е�instanceָ��;
CClientInstance* GetIdleInsID(CApp* pcApp)
{
    u16 wIndex = 0;
    CClientInstance *pCInst = NULL;

    // ��������instance��Ѱ�ҵ�һ�����е�instance������;
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

    // ��ǰû�п��е�instance;
    if (wIndex > MAX_INS_NO)
    {
        OspPrintf(TRUE, FALSE, "Current have none idle instance.\r\n");
        return NULL;
    }

    return pCInst;
}

// �ͻ����ļ�����instance����;
void CClientInstance::ClientFilePostInsAllot(CMessage *const pcMsg, CApp* pcApp)
{
	u16 wMsgLen = 0;

	// ��Ϣ������ȡ;
	wMsgLen = pcMsg->length;
	s8 *pchMsgGet = new char[wMsgLen + 1];
	ZeroMemory(pchMsgGet, wMsgLen + 1);
	memcpy_s(pchMsgGet, wMsgLen, pcMsg->content, wMsgLen);
	//OspPrintf(TRUE, FALSE, "message content is: %s, length is: %d.\n", pchMsgGet, wMsgLen);

    // ��ȡ���е�instanceָ��;
    CClientInstance *pcIns = GetIdleInsID(pcApp);
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

    // �����Ա��Ϣ��ʼ��;
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

    // ��ť��Ч;
    pFrame->m_pBtnFilePost->SetName(_T("FilePstButton"));
    
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
void CClientInstance::CliFilePostInsRelease(u32 dwInsNo)
{
    // �ͷ�ȫ�ֱ���;
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
    // �б���Ϣ�ػ�;
    OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_LIST_UI_PAINT, NULL,
        0, 0, MAKEIID(DEMO_APP_CLIENT_NO, dwInsNo), 0, DEMO_POST_TIMEOUT);

	// �ͷ�instance��Դ;
	m_curState = IDLE_STATE;
	m_lastState = STATE_WORK;
}

// �б���Ϣ����;
void CClientInstance::ListUI2Paint()
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
		sprintf(achProgress, "%s(%ld%%)", g_pvcFilePstInsNo[wListIndex]->m_tFileInfo.strFileName, g_pvcFilePstInsNo[wListIndex]->m_dnProgValve);
		pcProgress->SetValue((s32)g_pvcFilePstInsNo[wListIndex]->m_dnProgValve);
		pcProgress->SetText(A2W(achProgress));
		pcListContainer->Add(pcProgress);

		//pcHorizontalLayout->ApplyAttributeList(_T(""));

		// ��ʼ�����ؼ����;
		ZeroMemory(achBtnName, MAX_STR_LEN);
		sprintf(achBtnName, "FileSttButton%u", g_pvcFilePstInsNo[wListIndex]->m_instId);
		pcButtonStt->ApplyAttributeList(_T("width=\"20\" height=\"20\" normalimage=\"file=\'OspDemoSkins\\start.png\'\" pushedimage=\"file=\'OspDemoSkins\\start2.png\'\""));
		pcButtonStt->SetName(A2W(achBtnName));
		pcHorizontalLayout->Add(pcButtonStt);

		// ��ͣ�����ؼ����;
		ZeroMemory(achBtnName, MAX_STR_LEN);
		sprintf(achBtnName, "FileStpButton%u", g_pvcFilePstInsNo[wListIndex]->m_instId);
		pcButtonStp->ApplyAttributeList(_T("width=\"20\" height=\"20\" normalimage=\"file=\'OspDemoSkins\\pause.png\'\" pushedimage=\"file=\'OspDemoSkins\\pause2.png\'\""));
		pcButtonStp->SetName(A2W(achBtnName));
		pcHorizontalLayout->Add(pcButtonStp);

		// ȡ�������ؼ����;
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
	
	// Ԥ��;
	//sprintf(achPrgName, "DemoProgress%u", wInsNo);


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
void CClientInstance::DaemonInstanceEntry(CMessage *const pcMsg, CApp* pcApp)
{
    /*�õ���ǰ��Ϣ������;*/
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


// CDemoListContainerElementUI�ຯ������;
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