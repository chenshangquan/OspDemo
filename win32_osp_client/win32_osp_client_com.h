#pragma once

#include "../osplib/include/kdvtype.h"
#include "../osplib/include/osp.h"

CFrameWindowWnd* pFrame = NULL;
//#include "atlstr.h"

#define MAX_POST_MSG_LEN 255
#define MAX_POST_FILE_PACK_LEN 1024
#define MAX_INS_NO 10
#define MAX_INS_STR_NO 16
#define MAX_STR_LEN 16
#define MAX_FILE_PACKET 10240 * 2
#define MAX_FILE_NAME 128
#define MAX_THREADNO 3
#define MAX_FILE_POST_INS 5

#define INS_MSG_POST_NO 1
#define INS_FILE_POST_NO 2

#define DEMO_APP_SERVER_NO 1
#define DEMO_APP_CLIENT_NO 2
#define DEMO_APP_PRIO 100
#define DEMO_APP_QUE_SIZE (200 << 10)

#define DEMO_POST_TIMEOUT 2000

enum EM_USED_FLAG
{
    USED_NONE      = 0,
    USED_MSG_POST  = 1,
    USED_FILE_POST = 2
};

enum EM_EVENT_TYPE
{
    EVENT_MSG_POST_INS_ALLOT_ACK = 1,
    EVENT_FILE_POST_INS_ALLOT_ACK,
    EVENT_MSG_POST,
    EVENT_FILE_ATR_POST,
    EVENT_FILE_POST2C,
    EVENT_FILE_POST2S,
    EVENT_FILE_POST_ACK,
    EVENT_FILE_LEN_POST,
    EVENT_FILE_INFO_POST,
    EVENT_TIMEOUT,
    EVENT_TERM
};

enum EM_DAEM_EVENT_TYPE
{
    EVENT_CLIENT_MSG_POST_INS_ALLOT = 1,
    EVENT_SERVER_MSG_POST_INS_ALLOT_ACK,
    EVENT_CLIENT_FILE_POST_INS_ALLOT,
    EVENT_SERVER_FILE_POST_INS_ALLOT_ACK,
	EVENT_CLIENT_FILE_POST_INS_RELEASE
};

enum INS_STATE_TYPE
{
    IDLE_STATE = 0,
    STATE_ACK,
    //STATE_TERM,
    STATE_WORK
};

enum FILE_PACKET_TYPE
{
    FILE_PACKET_OK = 1,
    FILE_PACKET_ERROR,
    FILE_PACKET_END,
    FILE_PACKET_CANCEL
};

// ���Ľṹ��;
struct FILEMESSAGE
{
    s8  fileHead[4];       // ��־����;
    u32 fileStart;         // ���������������ļ��е���ʼλ�ñ�־;
    u32 fileSize;          // �������ݵĳ���;
    u16 wCliPostInsNum;    // ���м�¼�ͻ��˵ķ���instanceID;
    u16 wSerPostInsNum;    // ���м�¼����˵ķ���instanceID;
    u16 wIndex;            // ���м�¼�ͻ��˵ķ���instance������;
    u8  filePacket[MAX_FILE_PACKET - 4 - 2*sizeof(int)];       // ������������ļ�����;
};

// �ļ���Ϣ�ṹ��;
typedef struct tagFileInfo
{
    // �ļ��ְ���;
    int filePacketNum;
    // ������������Ϊ�ڼ�����;
    int filePacketIndex;
    // ʵ�����ڴ����ļ����ݵĴ�С;
    int nFilePacketBuff;
    // ��ǰ����ƫ��λ��;
    int fileStart;
    // ��ǰ���Ĵ�С;
    int fileSize;
    // ��һ�ΰ���ƫ��λ��;
    int lastStart;
    // ��һ�ΰ��Ĵ�С;
    int lastSize;
    // �ļ��ܳ���;
    u32 fileLength;
    // �ļ�����;
    char strFileName[MAX_FILE_NAME + 1];
}TFileInfo;

// ��¼�ļ���������е������Ϣ;
//FILEINFO m_fileInfo = {0};
// ��Ŵ������ļ��ĳ���;
DWORD m_length = 0;

BYTE m_sendBuffer[MAX_FILE_PACKET + 1];   // ����������buffer;
UINT m_nLength = 0;                       // ���������ݳ���;

// client read;
TCHAR g_strFilePath[MAX_PATH] = _T("");
TCHAR g_strFileName[MAX_FILE_NAME] = _T("");
TCHAR g_strFolderPath[MAX_PATH] = _T("F:\\2");
CHAR strFileLen[16] = {0};

// CDemoListContainerElementUI�ຯ������;
class CDemoListContainerElementUI : public CListContainerElementUI
{
public:
	void SetPos(RECT rc);
	CListHeaderUI *m_pHeader;
};

// CDemoListContainerElementUI�ຯ������;
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

// CDemoInstance�ຯ������;
class CDemoInstance : public CInstance
{
public:
    // Instance�е��û�����;
    u32 m_uNodeNum;
    u16 m_uCliInsNum;
    u16 m_uSerInsNum;
    u16 m_nUsedFlag;

    s32 m_nLastStart;
    s32 m_nLastSize;
    s32 m_nPktIndex;
    s64 m_dnProgValve;
    s32 m_nErrorIndex;
    TCHAR m_strFilePath[MAX_PATH];
    s32 m_nPuase;			// �ļ�ֹͣ���ͱ�־;
    s32 m_nCancel;			// �ļ�ȡ�����ͱ�־;
    TFileInfo m_tFileInfo;
public:
    void InstanceEntry(CMessage *const pMsg);    
    void DaemonInstanceEntry(CMessage *const pcMsg, CApp* pcApp);

    // Instance�е��û�����;
    //void OnMyReceive(CMessage *const pMsg);
public:
    CDemoInstance()
    {
        //m_lastStart = 0;
        //m_lastSize = 0;
        //m_errorIndex = 0;
        // Instance�е��û����ݳ�ʼ����;
    };
    ~CDemoInstance();
};

// typedef zTemplate<CDemoInstance, 20, CDemoAppData, 20> CDemoApp;
typedef zTemplate<CDemoInstance, MAX_INS_NO> CDemoApp;
CDemoApp g_cDemoApp;

// ��ʱȫ�ֱ���;
vector<CDemoInstance> g_tInsNo;

void sendFileInfo(s32 fStart,s32 fSize,char *fHead, u16 wCliPostInsNo, u16 wSerPostInsNo, u16 wIndex)
{
    OspPrintf(TRUE, FALSE, "sendFileInfo.\r\n");
    u16 wListIndex = 0;
    u16 wListItemNum = 0;
	s8 achProgress[MAX_FILE_NAME] = {0};
	s8 achBtnName[MAX_STR_LEN]  = {0};
	s8 achPrgName[MAX_STR_LEN]  = {0};
    s8 achTmpName[MAX_FILE_NAME]  = {0};
    CDuiString strListTag;
	CProgressUI* pcPrg = NULL;

	ZeroMemory(achPrgName, MAX_STR_LEN);
	sprintf(achPrgName, "DemoProgress%u", wIndex);
    // �ļ����������ֵ;
    g_tInsNo[wIndex].dnProgValve = 100 * g_tInsNo[wIndex].nPktIndex / g_tInsNo[wIndex].m_tFileInfo.filePacketNum;

	// ��ȡ�б���ָ��;
	CListUI* pcLoadList = pFrame->m_pList;
	
	u32 nFilePacketBuff = MAX_FILE_PACKET - 4 - 2*sizeof(s32) - 3*sizeof(u16);

    FILEMESSAGE strFileMsg;
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
    if (fStart == g_tInsNo[wIndex].nLastStart && fSize == g_tInsNo[wIndex].nLastSize)
    {
        // ���һ�����������;
        //if (fStart == 0 && fSize == 0 && !strcmp(fHead, "END"))
        if (!strcmp(fHead, "END"))
        {
            OspPrintf(TRUE, FALSE, "Enter END.\r\n");
#if 0
            //�������ö�������ļ�����;
            pProgress->SetValue(100);
            pProgress->SetText(L"Progress(100%)");

            // �ļ�У�鲢��ʾ���; --TODO
            ::MessageBox(NULL, _T("������ļ����䣡�ļ�����OK!!"), _T("�ļ�������"), NULL);
#endif
			USES_CONVERSION;
			// ��ȡ�ؼ�������ָ��;
			pcPrg = static_cast<CProgressUI*>(pFrame->m_pm.FindControl(A2W(achPrgName)));

			// ��������ֵ;
			ZeroMemory(achProgress, MAX_FILE_NAME);
			sprintf(achProgress, "%s(%d%%)", g_tInsNo[wIndex].m_tFileInfo.strFileName, 100);
			pcPrg->SetValue(100);
			pcPrg->SetText(A2W(achProgress));

			pcLoadList->RemoveAt(wIndex);

            return;
        }
        // ���͵�һ����;
        else if (fStart == 0 && fSize == 0 && !strcmp(fHead,"0"))
        {
            // ��������־����������0;
            g_tInsNo[wIndex].nErrorIndex = 0;
            g_tInsNo[wIndex].nPktIndex   = 0;

			// ��ͼ;
			pcLoadList->RemoveAll();    // ����ػ�;
			for (wListIndex = 0; wListIndex < g_tInsNo.size(); wListIndex++)
			{
				CDemoListContainerElementUI* pcListContainer = new CDemoListContainerElementUI;
				pcListContainer->m_pHeader = pFrame->m_pListHeader;
				CProgressUI* pcProgress = new CProgressUI;
				CHorizontalLayoutUI* pcHorizontalLayout = new CHorizontalLayoutUI;
				CButtonUI* pcButtonStp = new CButtonUI;
				CButtonUI* pcButtonCcl = new CButtonUI;

				pcListContainer->ApplyAttributeList(_T("height=\"25\" align=\"right\""));

				// �������ؼ����;
				USES_CONVERSION;
				pcProgress->ApplyAttributeList(_T("width=\"200\" height=\"20\" foreimage=\"OspDemoSkins\\progress_fore.png\"\
												  min=\"0\" max=\"100\" hor=\"true\" align=\"center\""));
				pcProgress->SetName(A2W(achPrgName));
				ZeroMemory(achProgress, MAX_FILE_NAME);
				sprintf(achProgress, "%s(%ld%%)", g_tInsNo[wListIndex].m_tFileInfo.strFileName, g_tInsNo[wListIndex].dnProgValve);
				pcProgress->SetValue((int)g_tInsNo[wListIndex].dnProgValve);
				pcProgress->SetText(A2W(achProgress));
				pcListContainer->Add(pcProgress);

				//pcHorizontalLayout->ApplyAttributeList(_T(""));

				// ��ͣ�����ؼ����;
				ZeroMemory(achBtnName, MAX_STR_LEN);
				sprintf(achBtnName, "FileStpButton%u", wListIndex);
				pcButtonStp->ApplyAttributeList(_T("text=\"S\" width=\"25\" height=\"20\""));
				pcButtonStp->SetName(A2W(achBtnName));
				pcHorizontalLayout->Add(pcButtonStp);

				// ȡ�������ؼ����;
				pcButtonCcl->ApplyAttributeList(_T("name=\"FileStpButton\" text=\"C\" width=\"25\" height=\"20\""));
				pcHorizontalLayout->Add(pcButtonCcl);

				pcListContainer->Add(pcHorizontalLayout);
				pcLoadList->Add(pcListContainer);

				//delete pcListContainer;
				//delete pcProgress;
				//delete pcHorizontalLayout;
				//delete pcButtonStp;
				//delete pcButtonCcl;
			}

            // ����ֻ��һ���������;
            if (g_tInsNo[wIndex].m_tFileInfo.fileLength <= nFilePacketBuff)
            {
                strFileMsg.fileStart = 0;
                strFileMsg.fileSize  = g_tInsNo[wIndex].m_tFileInfo.fileLength;
                g_tInsNo[wIndex].nLastStart = 0;
                g_tInsNo[wIndex].nLastSize  = g_tInsNo[wIndex].m_tFileInfo.fileLength;
                OspPrintf(TRUE, FALSE, "Only one packet. Index(%d)\r\n", g_tInsNo[wIndex].nPktIndex);
            }
            else
            {
                strFileMsg.fileStart = 0;
                strFileMsg.fileSize  = nFilePacketBuff;
                g_tInsNo[wIndex].nLastStart = 0;
                g_tInsNo[wIndex].nLastSize  = nFilePacketBuff;
                OspPrintf(TRUE, FALSE, "First packet. Index(%d)\r\n", g_tInsNo[wIndex].nPktIndex);
            }
        }
        // ���յ�server�˻ظ�������ȷ�ϰ�;
        else if (!strcmp(fHead,"OK!"))
        {
            USES_CONVERSION;
            pcPrg = static_cast<CProgressUI*>(pFrame->m_pm.FindControl(A2W(achPrgName)));

			// ��������ֵ;
			ZeroMemory(achProgress, MAX_FILE_NAME);
			sprintf(achProgress, "%s(%ld%%)", g_tInsNo[wIndex].m_tFileInfo.strFileName, g_tInsNo[wIndex].dnProgValve);
			pcPrg->SetValue((s32)g_tInsNo[wIndex].dnProgValve);
			pcPrg->SetText(A2W(achProgress));

            // ��������־��0;
            g_tInsNo[wIndex].nErrorIndex = 0;
            // �����������ۼ�;
            g_tInsNo[wIndex].nPktIndex++;

            // ��Ҫ�������һ������ʱ��;
            if ((g_tInsNo[wIndex].m_tFileInfo.fileLength - fStart - nFilePacketBuff) <= nFilePacketBuff)
            {
                strFileMsg.fileStart = fStart + nFilePacketBuff;
                //strFileMsg.fileSize  = g_tInsNo[wIndex].m_tFileInfo.fileLength % nFilePacketBuff;
                strFileMsg.fileSize  = g_tInsNo[wIndex].m_tFileInfo.fileLength - strFileMsg.fileStart;
                g_tInsNo[wIndex].nLastStart = strFileMsg.fileStart;
                g_tInsNo[wIndex].nLastSize  = strFileMsg.fileSize;
                OspPrintf(TRUE, FALSE, "Last packet. Index(%d)\r\n",  g_tInsNo[wIndex].nPktIndex);
            }
            else
            {
                strFileMsg.fileStart = fStart + nFilePacketBuff;
                strFileMsg.fileSize  = nFilePacketBuff;
                g_tInsNo[wIndex].nLastStart = strFileMsg.fileStart;
                g_tInsNo[wIndex].nLastSize  = strFileMsg.fileSize;
                OspPrintf(TRUE, FALSE, "Next packet. Index(%d)\r\n",  g_tInsNo[wIndex].nPktIndex);
            }
        }
        // ȷ�ϰ���ʾ���յ��İ��д��������·�����һ����;
        else if (!strcmp(fHead, "ERR"))   
        {
            OspPrintf(TRUE, FALSE, "Error packet. Index(%d)\r\n",  g_tInsNo[wIndex].nPktIndex);
            g_tInsNo[wIndex].nErrorIndex++;
            strFileMsg.fileStart = g_tInsNo[wIndex].nLastStart;
            strFileMsg.fileSize  = g_tInsNo[wIndex].nLastSize;
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
            OspPrintf(TRUE, FALSE, "Repeat the previous packet. Index(%d)\r\n",  g_tInsNo[wIndex].nPktIndex);
            g_tInsNo[wIndex].nErrorIndex++;
            strFileMsg.fileStart = g_tInsNo[wIndex].nLastStart;
            strFileMsg.fileSize  = g_tInsNo[wIndex].nLastSize;
        }
    }
    // ���յ�����Ϣ������ʱĬ�ϳ������·�����һ����;
    else 
    {
        OspPrintf(TRUE, FALSE, "Other error, Repeat the previous packet. Index(%d)\r\n",  g_tInsNo[wIndex].nPktIndex);
        g_tInsNo[wIndex].nErrorIndex++;
        strFileMsg.fileStart = g_tInsNo[wIndex].nLastStart;
        strFileMsg.fileSize  = g_tInsNo[wIndex].nLastSize;
    }

    // ������Խ��г�����;
    if (g_tInsNo[wIndex].nErrorIndex > 10)       
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

    HANDLE mFile = CreateFile(g_tInsNo[wIndex].strFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (mFile == INVALID_HANDLE_VALUE)
    {
        OspPrintf(TRUE, FALSE, "Client: CreateFile fail!\r\n");
        return;
    }

    SetFilePointer(mFile, strFileMsg.fileStart, NULL, FILE_BEGIN);
    ReadFile(mFile, strFileMsg.filePacket, strFileMsg.fileSize, &nByte, NULL);
    CloseHandle(mFile);

    BYTE* filePacket = (BYTE*)&strFileMsg;
    // �������Ͱ���������������ͻ�����;
    memcpy(m_sendBuffer, filePacket, strFileMsg.fileSize + 4 + 2*sizeof(s32) + 3*sizeof(u16));
    //m_nLength = strFileMsg.fileSize + 4 + 2*sizeof(s32) + 3*sizeof(u16);

    OspPrintf(TRUE, FALSE, "Start to post the file message to server. InsNo:%d\r\n", wCliPostInsNo);

    // ���Ͱ��������;
    OspPost(MAKEIID(DEMO_APP_SERVER_NO, wSerPostInsNo), EVENT_FILE_POST2S, m_sendBuffer,
        (strFileMsg.fileSize + 4 + 2*sizeof(s32) + 3*sizeof(u16)), CPublic::g_uNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, wCliPostInsNo));
}

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
    char cFileHead[4];
    FILEMESSAGE *strFileMsg;
    strFileMsg = (FILEMESSAGE*)strMsgGet;
    cFileHead[0] = strFileMsg->fileHead[0];
    cFileHead[1] = strFileMsg->fileHead[1];                                                                                                                                                                                                                                                                     
    cFileHead[2] = strFileMsg->fileHead[2];
    cFileHead[3] = '\0';
    s32 nFileStart = strFileMsg->fileStart;
    s32 nFileSize  = strFileMsg->fileSize;
	u16 wCliPostInsNo = strFileMsg->wCliPostInsNum;
    u16 wSerPostInsNo = strFileMsg->wSerPostInsNum;
	u16 wIndex     = strFileMsg->wIndex;

    // ��ͣ����;
    while(g_tInsNo[wIndex].m_nPuase != 0)
    {
        Sleep(1000);
    }

    OspPrintf(TRUE, FALSE, "Client: Next send, wCliPostInsNo: %d, wSerPostInsNo: %d, wIndex: %d\r\n", wCliPostInsNo, wSerPostInsNo, wIndex);
    // ��ȡ��Ϣ���ʹ���;
    sendFileInfo(nFileStart, nFileSize, cFileHead, wCliPostInsNo, wSerPostInsNo, wIndex);

    delete [] strMsgGet;
    strMsgGet = NULL;
}

// ����intance ID���ҵ���Ӧ��ȫ�ֱ�������;
u16 FindInsIndex(u16 wSerPostInsNo)
{
    u16 wIndex = 0;

    // ����server���ļ����͵�instance ID���ҵ���Ӧ��������ȡ�ļ�д��·����Ϣ;
	for (wIndex = 0; wIndex < g_tInsNo.size(); wIndex++)
	{
		if (g_tInsNo[wIndex].uInsNum == wSerPostInsNo)
        {
            return wIndex;
        }
    }

    return MAX_FILE_POST_INS;
}

// CDemoInstance �ඨ�弰��ʼ��;
// InstanceEntry:event ��Ϣ����������;
//////////////////////////////////////////////

// server��������Ϣ���͵�instance�ظ�������;
void MsgPostInsAllotAck(CMessage *const pMsg)
{
    CDemoInstance cDemoIns;
    cDemoIns.CDemoInstance();

    // ��Ϣ������ȡ;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];

    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    cDemoIns.m_uInsNum = atoi(strMsgGet);


}

// Server�������ļ����͵�instance�ظ�������;
void FilePostInsAllotAck(CMessage *const pMsg)
{
    // ��Ϣ������ȡ;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];

    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    // ��ȡInstance ID;
    CPublic::m_swTmpNum = atoi(strMsgGet);

}

// ��Ϣ���մ�����;
void MsgPostFunc(CMessage *const pMsg)
{
    // ��Ϣ������ȡ;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];

    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);
    OspPrintf(TRUE, FALSE, "message content is: %s, length is: %d.\n", strMsgGet, pMsg->length);

    // ��ֵ;
    USES_CONVERSION;
    wchar_t *lpMsg = A2W(strMsgGet);
    CEditUI* pEditRecv =  pFrame->m_pEditRecv;
    pEditRecv->SetText(lpMsg);

    // ��ȡԴ����;
    CPublic::g_uNodeNum = pMsg->srcnode;

    // �ͷſռ�;
    delete [] strMsgGet;
    strMsgGet = NULL;
}

// �ļ���Ϣ��ʼ��;
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

void FileMsgInit(CMessage *const pMsg)
{
    u16 wIndex = 0;
    
    // ��ȡ�յ�����Ϣ����;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];
    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    
    //memcpy_s(m_fileInfo.strFileName, MAX_FILE_NAME, strMsgGet, MsgLen);
    //m_fileInfo.strFileName[MsgLen] = '\0';

    // ���ļ���Ϣд���Ӧ��instance �ڵ�;
    for (wIndex = 0; wIndex < MAX_FILE_POST_INS; wIndex++)
    {
        if (g_tInsNo[wIndex].uInsNum != 0 && g_tInsNo[wIndex].nUsedFlag == 0)
        {
            // �ļ���Ϣ��ʼ��;
            FileInfoInit(wIndex);

            // ��ȡ�ļ�����Ϣ;
            memcpy_s(g_tInsNo[wIndex].m_tFileInfo.strFileName, MAX_FILE_NAME, strMsgGet, MsgLen);
            g_tInsNo[wIndex].m_tFileInfo.strFileName[MsgLen] = '\0';

            // �ļ��洢·������;
            USES_CONVERSION;
            ZeroMemory(g_strFilePath, MAX_PATH);
            lstrcat(g_strFilePath, g_strFolderPath);
            lstrcat(g_strFilePath, L"\\");
            lstrcat(g_strFilePath, A2W(g_tInsNo[wIndex].m_tFileInfo.strFileName));
            //g_tInsNo[wIndex].nFlag = 1;     //ʹ��ʱ��1;

            ZeroMemory(g_tInsNo[wIndex].strFilePath, MAX_PATH);
            lstrcpy(g_tInsNo[wIndex].strFilePath, g_strFilePath);
            break;
        }
    }

    OspPrintf(TRUE, FALSE, "index is :%d\r\n", wIndex);

    if (wIndex == MAX_FILE_POST_INS)
    {
        OspPrintf(TRUE, FALSE, "Server:FileMsgInit, Get instance index error!!\r\n");
        return;
    }
}

void FileLenInit(CMessage *const pMsg)
{
    u16 wIndex = 0;

    // ��ȡ�յ�����Ϣ����;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];
    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    // ���ļ���Ϣд���Ӧ��instance �ڵ�;
    for (wIndex = 0; wIndex < g_tInsNo.size(); wIndex++)
    {
        if (g_tInsNo[wIndex].uInsNum != 0 && g_tInsNo[wIndex].nUsedFlag == 0)
        {
			g_tInsNo[wIndex].nUsedFlag = 1;     //ʹ��ʱ��1;
            // ��ȡ�ļ�������Ϣ;
            g_tInsNo[wIndex].m_tFileInfo.fileLength = atoi(strMsgGet);

            // �����ļ��ְ���Ŀ;
            g_tInsNo[wIndex].m_tFileInfo.filePacketNum = g_tInsNo[wIndex].m_tFileInfo.fileLength/(MAX_FILE_PACKET - 4 - 2*sizeof(s32) - 3*sizeof(u16)) + 1;
            break;
        }
    }

    if (wIndex == g_tInsNo.size())
    {
        OspPrintf(TRUE, FALSE, "Server:FileLenInit, Get instance index error!!\r\n");
        return;
    }

    OspPrintf(TRUE, FALSE, "Server received file attribute, name is : %s, length : %d, packageNum : %d\n",
        g_tInsNo[wIndex].m_tFileInfo.strFileName, g_tInsNo[wIndex].m_tFileInfo.fileLength, g_tInsNo[wIndex].m_tFileInfo.filePacketNum);
   
    return;
}

void CDemoInstance::InstanceEntry(CMessage *const pMsg)
{
    /*�õ���ǰ��Ϣ������;*/
    u16 wCurEvent = pMsg->event;

    // ���ݲ�ͬ����Ϣ���ͽ��д���;
    switch (wCurEvent)
    {
    case EVENT_MSG_POST_INS_ALLOT_ACK:
        MsgPostInsAllotAck(pMsg);
        NextState(STATE_WORK);
        break;
    case EVENT_FILE_POST_INS_ALLOT_ACK:
        FilePostInsAllotAck(pMsg);
        NextState(STATE_WORK);
        break;
    case EVENT_MSG_POST:
        MsgPostFunc(pMsg);
        NextState(STATE_WORK);
        //SetTimer(EVENT_TIMEOUT, 1000);
        break;
    case EVENT_FILE_LEN_POST:
        FileLenInit(pMsg);
        break;
    case EVENT_FILE_ATR_POST:
        FileMsgInit(pMsg);
        break;
    case EVENT_FILE_POST2C:
        OnClientReceive(pMsg);
        break;
    default:
        
        break;
    }

    return;
}


// DaemonInstanceEntry:event ��Ϣ����������;
//////////////////////////////////////////////

// ��ȡ���е�instanceID;
u16 GetIdleInsID(CApp* pcApp)
{
    u16 wIndex = 0;
    CInstance *pCInst = NULL;

    // ��������instance��Ѱ�ҵ�һ�����е�instance������;
    for (wIndex = 1; wIndex <= MAX_INS_NO; wIndex++)
    {
        pCInst = pcApp->GetInstance(wIndex);
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

    return wIndex;
}

// �ͻ�����Ϣ����instance����;
void ClientMsgPostInsAllot(CMessage *const pcMsg, CApp* pcApp)
{
    s32 nIndex = 0;
    TInStatus tInStatus = {0};

    // �ж������ļ������instance�Ƿ�ﵽ��ֵ;
    if (g_tInsNo.size() == MAX_FILE_POST_INS)
    {
        OspPrintf(TRUE, FALSE, "Instances used for file transfer are limited.\r\n");
        return;
    }

    tInStatus.uInsNum = GetIdleInsID(pcApp);
    if (tInStatus.uInsNum == 0)
    {
        OspPrintf(TRUE, FALSE, "Client:have none idle instance.\r\n");
        return;
    }

    tInStatus.nUsedFlag = 0;
    OspPrintf(TRUE, FALSE, "Client: Get a idle instance, ID: %d.\r\n", tInStatus.uInsNum);

    g_tInsNo.push_back(tInStatus);
    return;
}

// �ͻ����ļ�����instance����;
void ClientFilePostInsAllot(CMessage *const pcMsg, CApp* pcApp)
{
    s32 nIndex = 0;
    TInStatus tInStatus = {0};

    // �ж������ļ������instance�Ƿ�ﵽ��ֵ;
    if (g_tInsNo.size() == MAX_FILE_POST_INS)
    {
        OspPrintf(TRUE, FALSE, "Instances used for file transfer are limited.\r\n");
        return;
    }

    tInStatus.uInsNum = GetIdleInsID(pcApp);
    if (tInStatus.uInsNum == 0)
    {
        OspPrintf(TRUE, FALSE, "Client:have none idle instance.\r\n");
        return;
    }

    tInStatus.nUsedFlag = 0;
    OspPrintf(TRUE, FALSE, "Client: Get a idle instance, ID: %d.\r\n", tInStatus.uInsNum);

    g_tInsNo.push_back(tInStatus);
    return;
}

// �ͻ����ļ�����instance�ͷ�;
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

void CDemoInstance::DaemonInstanceEntry(CMessage *const pcMsg, CApp* pcApp)
{
    /*�õ���ǰ��Ϣ������;*/
    u16 wCurEvent = pcMsg->event;

    switch (wCurEvent)
    {
    case EVENT_CLIENT_MSG_POST_INS_ALLOT:
        ClientMsgPostInsAllot(pcMsg, pcApp);
        NextState(STATE_WORK);
        break;
    case EVENT_CLIENT_FILE_POST_INS_ALLOT:
        ClientFilePostInsAllot(pcMsg, pcApp);
        NextState(STATE_WORK);
        break;
	case EVENT_CLIENT_FILE_POST_INS_RELEASE:
		CliFilePostInsRelease(pcMsg, pcApp);
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