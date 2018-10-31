#include "StdAfx.h"
#include "ServerInstance.h"
#include "win32_osp_server.h"

/****************************************************
 *
 * CClientInstance ��������;
 *
 ****************************************************/
vector<CServerInstance*> g_pvcFilePstInsNo;
//map<u32, CServerInstance*> g_mapFilePst;

TCHAR g_strFilePath[MAX_PATH] = _T("");
TCHAR g_strFileName[MAX_FILE_NAME] = _T("");
TCHAR g_strFolderPath[MAX_PATH] = _T("F:\\2");

//map<u32, u32> g_dwNodeNum;
u32 g_dwNodeNum;

extern CFrameWindowWnd *g_pFrame;
extern BOOL IsIpFormatRight(LPCTSTR pIpAddr);
extern u16 FindIndexByInsNo(u16 wInsNo);
CServerInstance::CServerInstance(void)
{
}

CServerInstance::~CServerInstance(void)
{
}

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
//void CServerInstance::InsRelease(CMessage *const pMsg)
//{
//	// �ͷ�Client���ļ����͵�instance;
//	/*OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_CLIENT_FILE_POST_INS_RELEASE,
//		NULL, 0, 0, pMsg->srcid, 0, DEMO_POST_TIMEOUT);*/
//
//	// �ͷ�Server���ļ����͵�instance;
//	OspPost(MAKEIID(DEMO_APP_SERVER_NO, CInstance::DAEMON), EVENT_SERVER_FILE_POST_INS_RELEASE,
//		NULL, 0, 0, pMsg->dstid, 0, DEMO_POST_TIMEOUT);
//}

/****************************************************
 *
 * CServerInstance ��������;
 *
 ****************************************************/

// InstanceEntry:event ��Ϣ����������;
/////////////////////////////////////////////////////

// ������Ϣ����;
void OspDisconnect(CMessage *const pMsg)
{
	// ���ڸ�ֵ;
	OspPrintf(TRUE, FALSE, "DisConnect by app!!\r\n");
    //CDuiString strSrcNode(std::to_string(pMsg->srcnode));
    CDuiString strSrcNode = _T("1");
    CDuiString strDisCnt(_T("DisCnt, NodeID:"));
    CDuiString strDisMsg = strDisCnt + strSrcNode;
	g_pFrame->m_pEditMsg->SetText(strDisMsg);
}

// ��Ϣ���մ�����;
void CServerInstance::MsgPostFunc(CMessage *const pMsg)
{
    u16 wMsgLen = 0;
    s8 *pchMsgGet = NULL;
    s32 nErroNo    = 0;

    // ��Ϣ������ȡ;
    wMsgLen = pMsg->length;
    
    // �������Ϣ��һ�����ڷ���˻�ȡsrcnode;
    if (wMsgLen == 0)
    {
        g_dwNodeNum = pMsg->srcnode;

        // ���ӳɹ�������Ϣ��ʾ;
        g_pFrame->m_pEditMsg->SetText(_T("Connect Successful!"));

        // ���ö���������;
        if (OspSetHBParam(g_dwNodeNum, MAX_HB_TIMER, MAX_HB_NUM) != TRUE)
        {
            OspPrintf(TRUE, FALSE, "OspSetHBParam Failed!\r\n");
            ::MessageBox(NULL, _T("OspSetHBParam Failed!"), _T("OspSetHBParam Result"), NULL);
        }

        // ������node�����ж�ʱ��֪ͨ��appid��InstId;
        nErroNo = OspNodeDiscCBReg(g_dwNodeNum, DEMO_APP_SERVER_NO, INS_MSG_POST_NO);
        if ( nErroNo != OSP_OK)
        {
            OspPrintf(TRUE, FALSE, "OspNodeDiscCBReg Failed, ErrorNo: %d!\r\n", nErroNo);
            ::MessageBox(NULL, _T("OspNodeDiscCBReg Failed!"), _T("OspNodeDiscCBReg Result"), NULL);
        }
        return;
    }
    g_dwNodeNum = pMsg->srcnode;

    pchMsgGet = new char[wMsgLen + 1];
    ZeroMemory(pchMsgGet, wMsgLen + 1);
    memcpy_s(pchMsgGet, wMsgLen, pMsg->content, wMsgLen);
    OspPrintf(TRUE, FALSE, "message content is: %s, length is: %d.\n", pchMsgGet, wMsgLen);

    // ���ڸ�ֵ;
    CEditUI* pEditRecv =  g_pFrame->m_pEditRecv;
    pEditRecv->SetText(CA2W(pchMsgGet));

    // �ͷſռ�;
    delete [] pchMsgGet;
    pchMsgGet = NULL;
}

// �����ܵ��İ�����д���ļ�;
bool CServerInstance::StoreFilePacket(TFileMessage *strFileMsgGet)
{
    DWORD nByte;

    OspPrintf(TRUE, FALSE, "Server: Start to store the packet\r\n");

    // ��¼����ƫ��λ��;
    m_nLastStart = strFileMsgGet->fileStart;
    m_nLastSize  = strFileMsgGet->fileSize;

    SetFilePointer(m_hFile, 0, 0, FILE_END);
    FlushFileBuffers(m_hFile);
    if (WriteFile(m_hFile, strFileMsgGet->filePacket, strFileMsgGet->fileSize, &nByte, NULL) == FALSE)
    {
        OspPrintf(TRUE, FALSE, "Packet write to local failed\r\n");
        return FALSE;
    }
    FlushFileBuffers(m_hFile);

    return TRUE;
}

// �ظ�����亯��;
void CServerInstance::SendFilePacketEcho(s32 nFlag)    //��Client����ȷ�ϰ���m_fileMsgSend��CMySocket�ı���;
{
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

    strFileMsgAck.fileStart      = m_nLastStart;
    strFileMsgAck.fileSize       = m_nLastSize;

    ZeroMemory(strFileMsgAck.filePacket, MAX_FILE_PACKET - 4 - 2*sizeof(s32));
    BYTE* newPacket = (BYTE*)&strFileMsgAck;

    OspPrintf(TRUE, FALSE, "Server: Response package.\r\n");
    // ���Ͱ����ͻ���
    OspPost(MAKEIID(DEMO_APP_CLIENT_NO, m_wCliInsNum), EVENT_FILE_POST2C,
        newPacket, (4 + 2*sizeof(s32)), m_dwNodeNum, MAKEIID(DEMO_APP_SERVER_NO, GetInsID()));
}

// ���ܰ�������;
void CServerInstance::ReceiveFilePacket(TFileMessage *strFileMsgGet, CMessage *const pMsg)
{
    LONG64 iValue = 0;
    TCHAR szProgressText[16] = {0};

#if 0
    // Server�ˣ��������滭����ʾ;
    iValue = 100 * m_fileInfo.filePacketIndex / m_fileInfo.filePacketNum;
    wsprintf(szProgressText, L"Progress(%ld%%)", iValue);
    pProgress->SetValue((int)iValue);
    pProgress->SetText(szProgressText);

    //m_fileInfo.filePacketIndex++;
#endif

    // �ĳ����ÿ�������;
    m_dwPktIndex++;

    // �ļ����������ֵ;
    m_dnProgValve = 100 * m_dwPktIndex / m_tFileInfo.filePacketNum;

	// ��һ��������ȡд���ļ��ľ��;
	if (m_dwPktIndex == 1)
	{
		// �����ļ���д���ļ�;
		m_hFile = CreateFile(m_strFilePath,
			GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_hFile == INVALID_HANDLE_VALUE)
		{
			OspPrintf(TRUE, FALSE, "Server: CreateFile fail!\r\n");
		}

        // ��ʼ�б���Ϣ����;
        ListUI2Paint();
	}
    else
    {
        // �б������Ϣ����;
        ListPrgUI2Paint();
    }

    // �յ��İ�Ϊ���һ����;
    if (m_dwPktIndex == m_tFileInfo.filePacketNum)
    {
        if (StoreFilePacket(strFileMsgGet) == TRUE)
        {
            OspPrintf(TRUE, FALSE, "Server: End package.\r\n");
            SendFilePacketEcho(FILE_PACKET_END);

			//FindClose(m_hFile);
            if (m_hFile)
            {
                CloseHandle(m_hFile);
            }
            SerFilePostInsRelease(m_instId);
            return;
        }

        OspPrintf(TRUE, FALSE, "Server: Store file packet fail!\r\n");
        SendFilePacketEcho(FILE_PACKET_ERROR);
        return;
    }
    // �յ��İ�Ϊ���������һ��к�����;
    else if (m_dwPktIndex < m_tFileInfo.filePacketNum)
    {
        if (StoreFilePacket(strFileMsgGet) == TRUE)
        {
            OspPrintf(TRUE, FALSE, "Server: Next package.\r\n");
            SendFilePacketEcho(FILE_PACKET_OK);

            return;
        }

        OspPrintf(TRUE, FALSE, "Server: Store file packet fail!\r\n");
        SendFilePacketEcho(FILE_PACKET_ERROR);
        return;
    }
    else
    {
        OspPrintf(TRUE, FALSE, "Server: Not current packet to save!\r\n");
        SendFilePacketEcho(FILE_PACKET_ERROR);
    }
    return;
}

// �����պ���;
void CServerInstance::OnServerReceive(CMessage *const pMsg)
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
        ReceiveFilePacket(strFileMsgGet, pMsg);
    }
    // ����client���͵Ĵ�����Ϣ����ֹ�ļ�����;
    else if (!strncmp(strFileMsgGet->fileHead, "ERR", 3)
		|| !strncmp(strFileMsgGet->fileHead, "CCL", 3))
    {
		// ��¼����ƫ��λ��;
		m_nLastStart = strFileMsgGet->fileStart;
		m_nLastSize  = strFileMsgGet->fileSize;

        // -- TODO:ɾ���ļ�;
		//BOOL bIsClosed = FindClose(m_hFile);
        //BOOL bIsClosed = CloseHandle(m_hFile);
        CloseHandle(m_hFile);
		//SetFileAttributes(m_strFilePath, FILE_ATTRIBUTE_NORMAL);
		BOOL bIsdelFile = DeleteFile(m_strFilePath);

        // ����б���Ϣ;
        ListClearUI2Paint();

		// �ͷ���Դ;
		SerFilePostInsRelease(m_instId);

		SendFilePacketEcho(FILE_PACKET_CANCEL);
    }

    strFileMsgGet = NULL;
    delete [] pchMsgGet;
    pchMsgGet = NULL;

    return;
}

// InstanceEntry��Ϣ���ͷַ��������;
void CServerInstance::InstanceEntry(CMessage *const pMsg)
{
    /*�õ���ǰ��Ϣ������;*/
    u16 wCurEvent = pMsg->event;

    // ���ݲ�ͬ����Ϣ���ͽ��д���;
    switch (wCurEvent)
    {
	case OSP_DISCONNECT:
		OspDisconnect(pMsg);
		//NextState(IDLE_STATE);
		break;
    case EVENT_MSG_POST:
        MsgPostFunc(pMsg);
        //NextState(STATE_WORK);
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
CServerInstance* GetIdleInsID(CApp* pcApp)
{
    u16 wIndex = 0;
    CServerInstance *pCInst = NULL;

    // ��������instance��Ѱ�ҵ���һ�����е�instance������;
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

    // ��ǰû�п��е�instance;
    if (wIndex > MAX_INS_NO)
    {
        OspPrintf(TRUE, FALSE, "Current have none idle instance.\r\n");
        return 0;
    }

    return pCInst;
}

// �ַ����ָ�;
void SplitString(const string& s, vector<string>& v, const string& c)
{
    string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));
         
        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
}

string to_String(int n)
{
    int m = n;
    char s[100];
    char ss[100];
    int i=0,j=0;
    if (n < 0)// ������;
    {
        m = 0 - m;
        j = 1;
        ss[0] = '-';
    }    
    while (m>0)
    {
        s[i++] = m % 10 + '0';
        m /= 10;
    }
    s[i] = '\0';
    i = i - 1;
    while (i >= 0)
    {
        ss[j++] = s[i--];
    }    
    ss[j] = '\0';    
    return ss;
}

// ������ļ�����instance����;
void CServerInstance::ServerFilePostInsAllot(CMessage *const pcMsg, CApp* pcApp)
{
	u16 wMsgLen = 0;
	s8 *pchMsgGet = NULL;
    vector<string> vecSplit;
    string str1, str2, str3, str4;
    s32 nIndex = 0;
	
	// ��Ϣ������ȡ;
	wMsgLen = pcMsg->length;
	pchMsgGet = new char[wMsgLen + 1];
	ZeroMemory(pchMsgGet, wMsgLen + 1);
	memcpy_s(pchMsgGet, wMsgLen, pcMsg->content, wMsgLen);
	//OspPrintf(TRUE, FALSE, "message content is: %s, length is: %d.\n", pchMsgGet, wMsgLen);

	// ��ȡ���е�instanceָ��;
	CServerInstance *pcIns = GetIdleInsID(pcApp);
	if (pcIns == 0)
	{
		OspPrintf(TRUE, FALSE, "Server:have none idle instance.\r\n");
		return;
	}

    // ��ȡ�ͻ���NodeNum;
    pcIns->m_dwNodeNum = pcMsg->srcnode;

	// ��ȡ�ͻ���InsNo;
	pcIns->m_wCliInsNum = GETINS(pcMsg->srcid);

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
    BOOL bIsExists = PathFileExists(g_strFilePath);
    if(PathFileExists(g_strFilePath))
    {
        str1 = (CW2A)g_strFilePath;
        SplitString(str1, vecSplit, ".");
        str3 = " - ���� (";

        //ZeroMemory(pcIns->m_strFilePath, MAX_PATH);
        //wcscpy(pcIns->m_strFilePath, (CA2W)s.c_str());

        for (s32 nIndex = 1; PathFileExists(pcIns->m_strFilePath) == TRUE; nIndex++)
        {
            str2 = "";
            str4 = to_String(nIndex);
            for(vector<string>::size_type i = 0; i != vecSplit.size(); ++i)
            {
                if (i == vecSplit.size() - 2)
                {
                    str2 += vecSplit[i];
                    str2 += str3;
                    str2 += str4;
                    str2 += ")";
                    continue;
                }
                if (i == vecSplit.size() - 1)
                {
                    str2 += ".";
                    str2 += vecSplit[i];
                    break;
                }
                str2 += vecSplit[i];
            }

            ZeroMemory(pcIns->m_strFilePath, MAX_PATH);
            wcscpy(pcIns->m_strFilePath, (CA2W)str2.c_str());
        }
        
        //DeleteFile(g_strFilePath);
    }

	// ���������������ʼ��; --TODO
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

	g_pvcFilePstInsNo.push_back(pcIns);

	// Instance������Ϣ�ظ�;
	OspPost(pcMsg->srcid, EVENT_SERVER_FILE_POST_INS_ALLOT_ACK,
		NULL, 0, pcIns->m_dwNodeNum, MAKEIID(DEMO_APP_SERVER_NO, pcIns->m_instId), 0, DEMO_POST_TIMEOUT);

    return;
}

// ������ļ�����instance�ͷ�;
void CServerInstance::SerFilePostInsRelease(u32 dwInsNo)
{
	// �ͷ�ȫ�ֱ���;
    vector<CServerInstance*>::iterator itIndex;
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

	// �б���Ϣ�ػ�; --TODO

	// �ͷ�instance��Դ;
	m_curState = IDLE_STATE;
	m_lastState = STATE_WORK;
}

// DaemonInstanceEntry��Ϣ�ַ��������;
void CServerInstance::DaemonInstanceEntry(CMessage *const pcMsg, CApp* pcApp)
{
    /*�õ���ǰ��Ϣ������;*/
    u16 wCurEvent = pcMsg->event;

    switch (wCurEvent)
    {
    case EVENT_SERVER_FILE_POST_INS_ALLOT:
        ServerFilePostInsAllot(pcMsg, pcApp);
        //NextState(STATE_WORK);
        break;
    default:
        // Def_Fuction(pMsg);
        break;
    }
    
    return;
}

void CServerInstance::ListUI2Paint()
{
    s8 achProgress[MAX_FILE_NAME] = {0};
    s8 achCtnName[MAX_STR_LEN]  = {0};
    s8 achPrgName[MAX_STR_LEN]  = {0};
	s8 achTxtName[MAX_STR_LEN]  = {0};

    // ��ͼ;
    CDemoListContainerElementUI* pcListContainer = new CDemoListContainerElementUI;
    pcListContainer->m_pHeader = g_pFrame->m_pListHeader;
    CProgressUI* pcProgress = new CProgressUI;
    CTextUI* pcButtonDone = new CTextUI;

    // �����ؼ����;
    ZeroMemory(achCtnName, MAX_STR_LEN);
    sprintf(achCtnName, "DemoContainer%u", m_instId);
    pcListContainer->ApplyAttributeList(_T("height=\"25\" align=\"right\""));
    pcListContainer->SetName((CA2W)achCtnName);

    // �������ؼ����;
    ZeroMemory(achPrgName, MAX_STR_LEN);
    sprintf(achPrgName, "DemoProgress%u", m_instId);
    pcProgress->ApplyAttributeList(_T("width=\"200\" height=\"20\" foreimage=\"OspDemoSkins\\progress_fore.png\"\
                                      min=\"0\" max=\"100\" hor=\"true\" align=\"center\""));
    pcProgress->SetName(CA2W(achPrgName));
    ZeroMemory(achProgress, MAX_FILE_NAME);
    sprintf(achProgress, "%s(%ld%%)", m_tFileInfo.strFileName, m_dnProgValve);
    pcProgress->SetValue(m_dnProgValve);
    pcProgress->SetText(CA2W(achProgress));
    pcListContainer->Add(pcProgress);

    // ״̬�ؼ����;
    ZeroMemory(achTxtName, MAX_STR_LEN);
    sprintf(achTxtName, "DemoText%u", m_instId);
    pcButtonDone->ApplyAttributeList(_T("width=\"20\" height=\"20\""));
    pcButtonDone->SetName(CA2W(achTxtName));
    pcButtonDone->SetText(_T("UpLoading"));
    pcListContainer->Add(pcButtonDone);

    g_pFrame->m_pList->Add(pcListContainer);

    return;
}

void CServerInstance::ListPrgUI2Paint()
{
    s8 achPrgName[MAX_STR_LEN]  = {0};
    s8 achProgress[MAX_FILE_NAME] = {0};
	s8 achTxtName[MAX_STR_LEN]  = {0};

    ZeroMemory(achPrgName, MAX_STR_LEN);
    sprintf(achPrgName, "DemoProgress%u", m_instId);
    CProgressUI* pcProgress = static_cast<CProgressUI*>(g_pFrame->m_pm.FindControl((CA2W)achPrgName));
    pcProgress->SetValue(m_dnProgValve);
    ZeroMemory(achProgress, MAX_FILE_NAME);
    sprintf(achProgress, "%s(%ld%%)", m_tFileInfo.strFileName, m_dnProgValve);
    pcProgress->SetText(CA2W(achProgress));

    ZeroMemory(achTxtName, MAX_STR_LEN);
    sprintf(achTxtName, "DemoText%u", m_instId);
    CTextUI* pcText = static_cast<CTextUI*>(g_pFrame->m_pm.FindControl((CA2W)achTxtName));
    if (m_dnProgValve == 100)
    {
        pcText->SetText(_T("Done"));
    }

    return;
}

void CServerInstance::ListClearUI2Paint()
{
    s8 achCtnName[MAX_STR_LEN]  = {0};

    // �������ؼ����;
    ZeroMemory(achCtnName, MAX_STR_LEN);
    sprintf(achCtnName, "DemoContainer%u", m_instId);

    CDemoListContainerElementUI* pcContainer = static_cast<CDemoListContainerElementUI*>(g_pFrame->m_pm.FindControl((CA2W)achCtnName));
    if (pcContainer != NULL)
    {
        g_pFrame->m_pList->Remove(pcContainer);
    }
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