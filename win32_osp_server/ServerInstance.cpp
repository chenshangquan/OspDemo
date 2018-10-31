#include "StdAfx.h"
#include "ServerInstance.h"
#include "win32_osp_server.h"

/****************************************************
 *
 * CClientInstance 函数定义;
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
//void CServerInstance::InsRelease(CMessage *const pMsg)
//{
//	// 释放Client端文件发送的instance;
//	/*OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_CLIENT_FILE_POST_INS_RELEASE,
//		NULL, 0, 0, pMsg->srcid, 0, DEMO_POST_TIMEOUT);*/
//
//	// 释放Server端文件发送的instance;
//	OspPost(MAKEIID(DEMO_APP_SERVER_NO, CInstance::DAEMON), EVENT_SERVER_FILE_POST_INS_RELEASE,
//		NULL, 0, 0, pMsg->dstid, 0, DEMO_POST_TIMEOUT);
//}

/****************************************************
 *
 * CServerInstance 函数定义;
 *
 ****************************************************/

// InstanceEntry:event 消息处理函数定义;
/////////////////////////////////////////////////////

// 断链消息处理;
void OspDisconnect(CMessage *const pMsg)
{
	// 窗口赋值;
	OspPrintf(TRUE, FALSE, "DisConnect by app!!\r\n");
    //CDuiString strSrcNode(std::to_string(pMsg->srcnode));
    CDuiString strSrcNode = _T("1");
    CDuiString strDisCnt(_T("DisCnt, NodeID:"));
    CDuiString strDisMsg = strDisCnt + strSrcNode;
	g_pFrame->m_pEditMsg->SetText(strDisMsg);
}

// 消息接收处理函数;
void CServerInstance::MsgPostFunc(CMessage *const pMsg)
{
    u16 wMsgLen = 0;
    s8 *pchMsgGet = NULL;
    s32 nErroNo    = 0;

    // 消息内容提取;
    wMsgLen = pMsg->length;
    
    // 处理空消息，一般用于服务端获取srcnode;
    if (wMsgLen == 0)
    {
        g_dwNodeNum = pMsg->srcnode;

        // 连接成功窗口信息提示;
        g_pFrame->m_pEditMsg->SetText(_T("Connect Successful!"));

        // 设置断链检测参数;
        if (OspSetHBParam(g_dwNodeNum, MAX_HB_TIMER, MAX_HB_NUM) != TRUE)
        {
            OspPrintf(TRUE, FALSE, "OspSetHBParam Failed!\r\n");
            ::MessageBox(NULL, _T("OspSetHBParam Failed!"), _T("OspSetHBParam Result"), NULL);
        }

        // 设置在node连接中断时需通知的appid和InstId;
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

    // 窗口赋值;
    CEditUI* pEditRecv =  g_pFrame->m_pEditRecv;
    pEditRecv->SetText(CA2W(pchMsgGet));

    // 释放空间;
    delete [] pchMsgGet;
    pchMsgGet = NULL;
}

// 将接受到的包内容写入文件;
bool CServerInstance::StoreFilePacket(TFileMessage *strFileMsgGet)
{
    DWORD nByte;

    OspPrintf(TRUE, FALSE, "Server: Start to store the packet\r\n");

    // 记录本次偏移位置;
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

// 回复包填充函数;
void CServerInstance::SendFilePacketEcho(s32 nFlag)    //向Client发送确认包，m_fileMsgSend是CMySocket的变量;
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
    // 发送包到客户端
    OspPost(MAKEIID(DEMO_APP_CLIENT_NO, m_wCliInsNum), EVENT_FILE_POST2C,
        newPacket, (4 + 2*sizeof(s32)), m_dwNodeNum, MAKEIID(DEMO_APP_SERVER_NO, GetInsID()));
}

// 接受包处理函数;
void CServerInstance::ReceiveFilePacket(TFileMessage *strFileMsgGet, CMessage *const pMsg)
{
    LONG64 iValue = 0;
    TCHAR szProgressText[16] = {0};

#if 0
    // Server端，进度条绘画并显示;
    iValue = 100 * m_fileInfo.filePacketIndex / m_fileInfo.filePacketNum;
    wsprintf(szProgressText, L"Progress(%ld%%)", iValue);
    pProgress->SetValue((int)iValue);
    pProgress->SetText(szProgressText);

    //m_fileInfo.filePacketIndex++;
#endif

    // 改成外置控制属性;
    m_dwPktIndex++;

    // 文件传输进度数值;
    m_dnProgValve = 100 * m_dwPktIndex / m_tFileInfo.filePacketNum;

	// 第一个包，获取写入文件的句柄;
	if (m_dwPktIndex == 1)
	{
		// 创建文件并写入文件;
		m_hFile = CreateFile(m_strFilePath,
			GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_hFile == INVALID_HANDLE_VALUE)
		{
			OspPrintf(TRUE, FALSE, "Server: CreateFile fail!\r\n");
		}

        // 初始列表信息绘制;
        ListUI2Paint();
	}
    else
    {
        // 列表进度信息绘制;
        ListPrgUI2Paint();
    }

    // 收到的包为最后一个包;
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
    // 收到的包为正常包，且还有后续包;
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

// 包接收函数;
void CServerInstance::OnServerReceive(CMessage *const pMsg)
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
        ReceiveFilePacket(strFileMsgGet, pMsg);
    }
    // 接收client发送的错误信息，中止文件传输;
    else if (!strncmp(strFileMsgGet->fileHead, "ERR", 3)
		|| !strncmp(strFileMsgGet->fileHead, "CCL", 3))
    {
		// 记录本次偏移位置;
		m_nLastStart = strFileMsgGet->fileStart;
		m_nLastSize  = strFileMsgGet->fileSize;

        // -- TODO:删除文件;
		//BOOL bIsClosed = FindClose(m_hFile);
        //BOOL bIsClosed = CloseHandle(m_hFile);
        CloseHandle(m_hFile);
		//SetFileAttributes(m_strFilePath, FILE_ATTRIBUTE_NORMAL);
		BOOL bIsdelFile = DeleteFile(m_strFilePath);

        // 清除列表信息;
        ListClearUI2Paint();

		// 释放资源;
		SerFilePostInsRelease(m_instId);

		SendFilePacketEcho(FILE_PACKET_CANCEL);
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

// 字符串分割;
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
    if (n < 0)// 处理负数;
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

// 服务端文件发送instance分配;
void CServerInstance::ServerFilePostInsAllot(CMessage *const pcMsg, CApp* pcApp)
{
	u16 wMsgLen = 0;
	s8 *pchMsgGet = NULL;
    vector<string> vecSplit;
    string str1, str2, str3, str4;
    s32 nIndex = 0;
	
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

    // 获取客户端NodeNum;
    pcIns->m_dwNodeNum = pcMsg->srcnode;

	// 获取客户端InsNo;
	pcIns->m_wCliInsNum = GETINS(pcMsg->srcid);

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
    BOOL bIsExists = PathFileExists(g_strFilePath);
    if(PathFileExists(g_strFilePath))
    {
        str1 = (CW2A)g_strFilePath;
        SplitString(str1, vecSplit, ".");
        str3 = " - 副本 (";

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

	// 服务端其他变量初始化; --TODO
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

	// Instance申请消息回复;
	OspPost(pcMsg->srcid, EVENT_SERVER_FILE_POST_INS_ALLOT_ACK,
		NULL, 0, pcIns->m_dwNodeNum, MAKEIID(DEMO_APP_SERVER_NO, pcIns->m_instId), 0, DEMO_POST_TIMEOUT);

    return;
}

// 服务端文件发送instance释放;
void CServerInstance::SerFilePostInsRelease(u32 dwInsNo)
{
	// 释放全局变量;
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

	// 列表信息重绘; --TODO

	// 释放instance资源;
	m_curState = IDLE_STATE;
	m_lastState = STATE_WORK;
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

    // 绘图;
    CDemoListContainerElementUI* pcListContainer = new CDemoListContainerElementUI;
    pcListContainer->m_pHeader = g_pFrame->m_pListHeader;
    CProgressUI* pcProgress = new CProgressUI;
    CTextUI* pcButtonDone = new CTextUI;

    // 容器控件添加;
    ZeroMemory(achCtnName, MAX_STR_LEN);
    sprintf(achCtnName, "DemoContainer%u", m_instId);
    pcListContainer->ApplyAttributeList(_T("height=\"25\" align=\"right\""));
    pcListContainer->SetName((CA2W)achCtnName);

    // 进度条控件添加;
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

    // 状态控件添加;
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

    // 进度条控件添加;
    ZeroMemory(achCtnName, MAX_STR_LEN);
    sprintf(achCtnName, "DemoContainer%u", m_instId);

    CDemoListContainerElementUI* pcContainer = static_cast<CDemoListContainerElementUI*>(g_pFrame->m_pm.FindControl((CA2W)achCtnName));
    if (pcContainer != NULL)
    {
        g_pFrame->m_pList->Remove(pcContainer);
    }
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