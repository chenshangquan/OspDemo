#pragma once

#include "../osplib/include/kdvtype.h"
#include "../osplib/include/osp.h"

CFrameWindowWnd* pFrame = NULL;
//#include "atlstr.h"

#define MAX_POST_MSG_LEN 255
#define MAX_POST_FILE_PACK_LEN 1024
#define MAX_INS_NO 10
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
    EVENT_SERVER_MSG_POST_INS_ALLOT = 1,
    EVENT_SERVER_FILE_POST_INS_ALLOT,
    EVENT_CLIENT_FILE_POST_INS_ALLOT
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
    CHAR fileHead[4];       // ��־����;
    s32  fileStart;         // ���������������ļ��е���ʼλ�ñ�־;
    s32  fileSize;          // �������ݵĳ���;
    u16  wCliPostInsNum;    // ���м�¼�ͻ��˵ķ���instanceID;
    u16  wSerPostInsNum;    // ���м�¼����˵ķ���instanceID;
    u16  wIndex;            // ���м�¼�ͻ��˵ķ���instance������;
    BYTE filePacket[MAX_FILE_PACKET - 4 - 2*sizeof(int)];       // ������������ļ�����;
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
    DWORD fileLength;
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

s32 g_PauseFlag;

class CPublic
{
public:
    CPublic();
    virtual ~CPublic(); 
    // �Զ���ȫ�ֱ�������;
public:
    static u32 g_uNodeNum;
    static u32 g_uAppNum;
    static u16 g_uInsNum;
    static u16 m_swTmpNum;
};

#if 0
class CFileInfo
{
public:
    CFileInfo();
    virtual ~CFileInfo();
public:

};
#endif

class CDemoInstance : public CInstance
{
private:
    //int m_lastStart;
    //int m_lastSize;
    //int m_errorIndex;
    // Instance�е��û�����;
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

// CPublic �ඨ�弰��ʼ��;
CPublic::CPublic()
{

}

CPublic::~CPublic()
{

}

u32 CPublic::g_uNodeNum  = 0;
u32 CPublic::g_uAppNum   = 0;
u16 CPublic::g_uInsNum   = 0;
u16 CPublic::m_swTmpNum  = 0;

#if 0 // ȫ�ֱ�����ʼ��;
s32 CPublic::m_lastStart = 0;
s32 CPublic::m_lastSize  = 0;
s32 CPublic::m_PktIndex  = 0;
s32 CPublic::m_errorIndex = 0;
#endif
// CDemoInstance �ඨ�弰��ʼ��;


// ��ʱȫ�ֱ���;
typedef struct tagInStatus
{
    u16 uCliInsNum;
    u16 uSerInsNum;
    s32 nFlag;
    s32 nLastStart;
    s32 nLastSize;
    s32 nPktIndex;
    s32 nErrorIndex;
	TCHAR strFilePath[MAX_PATH];
    TFileInfo m_tFileInfo;
}TInStatus;

TInStatus g_uInsNo[MAX_FILE_POST_INS] = {0};

void sendFileInfo(s32 fStart,s32 fSize,char *fHead, u16 wCliPostInsNo, u16 wSerPostInsNo, u16 wIndex)
{
    LONG64 iValue = 0;
    TCHAR szProgress[16] = {0};
    CProgressUI* pProgress =  pFrame->m_pProgress;
#if 0
    // Client�ˣ��������滭����ʾ;
    iValue = 100 * g_uInsNo[wIndex].nPktIndex / m_fileInfo.filePacketNum;
    wsprintf(szProgress, L"Progress(%ld%%)", iValue);
    pProgress->SetValue((int)iValue);
    pProgress->SetText(szProgress);
#endif
    UINT nFilePacketBuff = MAX_FILE_PACKET - 4 - 2*sizeof(s32) - 3*sizeof(u16);

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
    if (fStart == g_uInsNo[wIndex].nLastStart && fSize == g_uInsNo[wIndex].nLastSize)
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
            return;
        }
        // ���͵�һ����;
        else if (fStart == 0 && fSize == 0 && !strcmp(fHead,"0"))
        {
            // ��������־����������0;
            g_uInsNo[wIndex].nErrorIndex = 0;
            g_uInsNo[wIndex].nPktIndex   = 0;

            // ����ֻ��һ���������;
            if (g_uInsNo[wIndex].m_tFileInfo.fileLength <= nFilePacketBuff)
            {
                strFileMsg.fileStart = 0;
                strFileMsg.fileSize  = g_uInsNo[wIndex].m_tFileInfo.fileLength;
                g_uInsNo[wIndex].nLastStart = 0;
                g_uInsNo[wIndex].nLastSize  = g_uInsNo[wIndex].m_tFileInfo.fileLength;
                OspPrintf(TRUE, FALSE, "Only one packet. Index(%d)\r\n", g_uInsNo[wIndex].nPktIndex);
            }
            else
            {
                strFileMsg.fileStart = 0;
                strFileMsg.fileSize  = nFilePacketBuff;
                g_uInsNo[wIndex].nLastStart = 0;
                g_uInsNo[wIndex].nLastSize  = nFilePacketBuff;
                OspPrintf(TRUE, FALSE, "First packet. Index(%d)\r\n", g_uInsNo[wIndex].nPktIndex);
            }
        }
        // ���յ�server�˻ظ�������ȷ�ϰ�;
        else if (!strcmp(fHead,"OK!"))
        {
            // ��������־��0;
            g_uInsNo[wIndex].nErrorIndex = 0;
            // �����������ۼ�;
            g_uInsNo[wIndex].nPktIndex++;

            // ��Ҫ�������һ������ʱ��;
            if ((g_uInsNo[wIndex].m_tFileInfo.fileLength - fStart - nFilePacketBuff) <= nFilePacketBuff)
            {
                strFileMsg.fileStart = fStart + nFilePacketBuff;
                //strFileMsg.fileSize  = g_uInsNo[wIndex].m_tFileInfo.fileLength % nFilePacketBuff;
                strFileMsg.fileSize  = g_uInsNo[wIndex].m_tFileInfo.fileLength - strFileMsg.fileStart;
                g_uInsNo[wIndex].nLastStart = strFileMsg.fileStart;
                g_uInsNo[wIndex].nLastSize  = strFileMsg.fileSize;
                OspPrintf(TRUE, FALSE, "Last packet. Index(%d)\r\n",  g_uInsNo[wIndex].nPktIndex);
            }
            else
            {
                strFileMsg.fileStart = fStart + nFilePacketBuff;
                strFileMsg.fileSize  = nFilePacketBuff;
                g_uInsNo[wIndex].nLastStart = strFileMsg.fileStart;
                g_uInsNo[wIndex].nLastSize  = strFileMsg.fileSize;
                OspPrintf(TRUE, FALSE, "Next packet. Index(%d)\r\n",  g_uInsNo[wIndex].nPktIndex);
            }
        }
        // ȷ�ϰ���ʾ���յ��İ��д��������·�����һ����;
        else if (!strcmp(fHead, "ERR"))   
        {
            OspPrintf(TRUE, FALSE, "Error packet. Index(%d)\r\n",  g_uInsNo[wIndex].nPktIndex);
            g_uInsNo[wIndex].nErrorIndex++;
            strFileMsg.fileStart = g_uInsNo[wIndex].nLastStart;
            strFileMsg.fileSize  = g_uInsNo[wIndex].nLastSize;
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
            OspPrintf(TRUE, FALSE, "Repeat the previous packet. Index(%d)\r\n",  g_uInsNo[wIndex].nPktIndex);
            g_uInsNo[wIndex].nErrorIndex++;
            strFileMsg.fileStart = g_uInsNo[wIndex].nLastStart;
            strFileMsg.fileSize  = g_uInsNo[wIndex].nLastSize;
        }
    }
    // ���յ�����Ϣ������ʱĬ�ϳ������·�����һ����;
    else 
    {
        OspPrintf(TRUE, FALSE, "Other error, Repeat the previous packet. Index(%d)\r\n",  g_uInsNo[wIndex].nPktIndex);
        g_uInsNo[wIndex].nErrorIndex++;
        strFileMsg.fileStart = g_uInsNo[wIndex].nLastStart;
        strFileMsg.fileSize  = g_uInsNo[wIndex].nLastSize;
    }

    // ������Խ��г�����;
    if (g_uInsNo[wIndex].nErrorIndex > 10)       
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

    HANDLE mFile = CreateFile(g_uInsNo[wIndex].strFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
    while(g_PauseFlag != 0)
    {
        Sleep(1000);
    }

    // ��ȡ��Ϣ���ʹ���;
    sendFileInfo(nFileStart, nFileSize, cFileHead, wCliPostInsNo, wSerPostInsNo, wIndex);

    delete [] strMsgGet;
    strMsgGet = NULL;
}

//void ReceiveFilePacket(int nErrorCode)     //��Socket�Ľ��ջ������ж�ȡ����;
//void SendFilePacketEcho(int nFlag);        //��Client�����ļ���Ӧ��Ϣ;
//bool StoreFilePacket();                    //������յ����ļ���;
//bool IsFilePacket();                       //�жϲ���֤�ļ�������;

// 
u16 FindInsIndex(u16 wSerPostInsNo)
{
    u16 wIndex = 0;

    // ����server���ļ����͵�instance ID���ҵ���Ӧ��������ȡ�ļ�д��·����Ϣ;
    for (wIndex = 0; wIndex < MAX_FILE_POST_INS; wIndex++)
    {
        if (g_uInsNo[wIndex].uSerInsNum == wSerPostInsNo)
        {
            return wIndex;
        }
    }

    return MAX_FILE_POST_INS;
}

bool StoreFilePacket(FILEMESSAGE *strFileMsgGet, u16 wSerPostInsNo)
{
    DWORD nByte;
    u16 wIndex = 0;

    OspPrintf(TRUE, FALSE, "Server: Start to store the packet\r\n");
    //m_fileInfo.fileStart = strFileMsgGet->fileStart;
    //m_fileInfo.fileSize = strFileMsgGet->fileSize;

    // ����instanceID����ȡinstance������;
    wIndex = FindInsIndex(wSerPostInsNo);
    if (wIndex == MAX_FILE_POST_INS)
    {
        OspPrintf(TRUE, FALSE, "can't find the instance index.\r\n");
        return FALSE;
    }

    g_uInsNo[wIndex].m_tFileInfo.fileStart = strFileMsgGet->fileStart;
    g_uInsNo[wIndex].m_tFileInfo.fileSize = strFileMsgGet->fileSize;
    

    // �����ļ���д���ļ�;
    HANDLE mFileW = CreateFile(g_uInsNo[wIndex].strFilePath, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (mFileW == INVALID_HANDLE_VALUE)
    {
        OspPrintf(TRUE, FALSE, "Server: CreateFile fail!\r\n");
        return FALSE;
    }

    SetFilePointer(mFileW, 0, 0, FILE_END);
    FlushFileBuffers(mFileW);
    WriteFile(mFileW, strFileMsgGet->filePacket, strFileMsgGet->fileSize, &nByte, NULL);
    FlushFileBuffers(mFileW);
    CloseHandle(mFileW);

    // ��¼����ƫ��λ��;
    //m_fileInfo.lastStart = m_fileInfo.fileStart;
    //m_fileInfo.lastSize = m_fileInfo.fileSize;

    g_uInsNo[wIndex].m_tFileInfo.lastStart = g_uInsNo[wIndex].m_tFileInfo.fileStart;
    g_uInsNo[wIndex].m_tFileInfo.lastSize  = g_uInsNo[wIndex].m_tFileInfo.fileSize;

    return TRUE;
}

void SendFilePacketEcho(s32 nFlag, u16 wCliPostInsNo, u16 wSerPostInsNo, u16 wIndex)    //��Client����ȷ�ϰ���m_fileMsgSend��CMySocket�ı���;
{
    u16 wSerIndex = 0;
    FILEMESSAGE strFileMsgAck = {0};

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
    if (wIndex == MAX_FILE_POST_INS)
    {
        OspPrintf(TRUE, FALSE, "can't find the instance index.\r\n");
        return;
    }

    //strFileMsgAck.fileStart   = m_fileInfo.lastStart; 
    //strFileMsgAck.fileSize    = m_fileInfo.lastSize;
    strFileMsgAck.fileStart      = g_uInsNo[wSerIndex].m_tFileInfo.lastStart;
    strFileMsgAck.fileSize       = g_uInsNo[wSerIndex].m_tFileInfo.lastSize;
    strFileMsgAck.wCliPostInsNum = wCliPostInsNo;
	strFileMsgAck.wSerPostInsNum = wSerPostInsNo;
	strFileMsgAck.wIndex = wIndex;

    ZeroMemory(strFileMsgAck.filePacket, MAX_FILE_PACKET - 4 - 2*sizeof(s32) - 3*sizeof(u16));
    BYTE* newPacket = (BYTE*)&strFileMsgAck;
    u16 uLen = 4 + 2*sizeof(s32) + 3*sizeof(u16);
    
    // ���Ͱ����ͻ���
    OspPost(MAKEIID(DEMO_APP_CLIENT_NO, wCliPostInsNo), EVENT_FILE_POST2C, newPacket, uLen, CPublic::g_uNodeNum);
}

void ReceiveFilePacket(FILEMESSAGE *strFileMsgGet)
{
    LONG64 iValue = 0;
    TCHAR szProgressText[16] = {0};
    u16 wSerIndex = 0;

	u16 wCliPostInsNo = strFileMsgGet->wCliPostInsNum;
    u16 wSerPostInsNo = strFileMsgGet->wSerPostInsNum;
	u16 wIndex = strFileMsgGet->wIndex;

    CProgressUI* pProgress =  pFrame->m_pProgress;
#if 0
    // Server�ˣ��������滭����ʾ;
    iValue = 100 * m_fileInfo.filePacketIndex / m_fileInfo.filePacketNum;
    wsprintf(szProgressText, L"Progress(%ld%%)", iValue);
    pProgress->SetValue((int)iValue);
    pProgress->SetText(szProgressText);

    m_fileInfo.filePacketIndex++;
#endif

    // ����instanceID����ȡinstance������;
    wSerIndex = FindInsIndex(wSerPostInsNo);
    if (wSerIndex == MAX_FILE_POST_INS)
    {
        OspPrintf(TRUE, FALSE, "can't find the instance index.\r\n");
        return;
    }

    g_uInsNo[wSerIndex].m_tFileInfo.filePacketIndex++;
    // �յ��İ�Ϊ���һ����;
    //if (m_fileInfo.filePacketIndex == m_fileInfo.filePacketNum)
    if (g_uInsNo[wSerIndex].m_tFileInfo.filePacketIndex == g_uInsNo[wSerIndex].m_tFileInfo.filePacketNum)
    {
        if (StoreFilePacket(strFileMsgGet, wSerPostInsNo) == TRUE)
        {
            SendFilePacketEcho(FILE_PACKET_END, wCliPostInsNo, wSerPostInsNo, wIndex);
#if 0
            //�������ö�������ļ�����;
            pProgress->SetValue(100);
            pProgress->SetText(L"Progress(100%)");

            // �ļ�У�鲢��ʾ���; --TODO
            ::MessageBox(NULL, _T("������ļ����䣡�ļ�����OK!!"), _T("�ļ�������"), NULL);
#endif
            return;
        }

        OspPrintf(TRUE, FALSE, "Server: Store file packet fail!\r\n");
        SendFilePacketEcho(FILE_PACKET_ERROR, wCliPostInsNo, wSerPostInsNo, wIndex);
        return;
    }
    // �յ��İ�Ϊ���������һ��к�����;
    //else if (m_fileInfo.filePacketIndex < m_fileInfo.filePacketNum)
    else if (g_uInsNo[wSerIndex].m_tFileInfo.filePacketIndex < g_uInsNo[wSerIndex].m_tFileInfo.filePacketNum)
    {
        if (StoreFilePacket(strFileMsgGet, wSerPostInsNo) == TRUE)
        {
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

// InstanceEntry:event ��Ϣ����������;
//////////////////////////////////////////////

// server��������Ϣ���͵�instance�ظ�������;
void MsgPostInsAllotAck(CMessage *const pMsg)
{
    // ��Ϣ������ȡ;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];

    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    CPublic::g_uInsNum = atoi(strMsgGet);

}

// server�������ļ����͵�instance�ظ�������;
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

void OnServerReceive(CMessage *const pMsg)
{
    OspPrintf(TRUE, FALSE, "server receive the message from client\n");
    // ��ȡ�յ�����Ϣ����;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];
    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    FILEMESSAGE *strFileMsgGet = (FILEMESSAGE *)strMsgGet;

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
    delete [] strMsgGet;
    strMsgGet = NULL;


}

// �ļ���Ϣ��ʼ��;
void FileInfoInit(u16 wIndex)
{
    // ��ʼ���ļ���Ϣ����;
    g_uInsNo[wIndex].m_tFileInfo.filePacketNum = 0;
    g_uInsNo[wIndex].m_tFileInfo.filePacketIndex = 0;
    g_uInsNo[wIndex].m_tFileInfo.nFilePacketBuff = 0;

    g_uInsNo[wIndex].m_tFileInfo.fileStart  = 0;
    g_uInsNo[wIndex].m_tFileInfo.fileSize   = 0;
    g_uInsNo[wIndex].m_tFileInfo.lastStart  = 0;
    g_uInsNo[wIndex].m_tFileInfo.lastSize   = 0;
    g_uInsNo[wIndex].m_tFileInfo.fileLength = 0;

    ZeroMemory(g_uInsNo[wIndex].m_tFileInfo.strFileName, MAX_FILE_NAME + 1);
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
        if (g_uInsNo[wIndex].uSerInsNum != 0 && g_uInsNo[wIndex].nFlag == 0)
        {
            // �ļ���Ϣ��ʼ��;
            FileInfoInit(wIndex);

            // ��ȡ�ļ�����Ϣ;
            memcpy_s(g_uInsNo[wIndex].m_tFileInfo.strFileName, MAX_FILE_NAME, strMsgGet, MsgLen);
            g_uInsNo[wIndex].m_tFileInfo.strFileName[MsgLen] = '\0';

            // �ļ��洢·������;
            USES_CONVERSION;
            ZeroMemory(g_strFilePath, MAX_PATH);
            lstrcat(g_strFilePath, g_strFolderPath);
            lstrcat(g_strFilePath, L"\\");
            lstrcat(g_strFilePath, A2W(g_uInsNo[wIndex].m_tFileInfo.strFileName));
            //g_uInsNo[wIndex].nFlag = 1;     //ʹ����0, Ĭ��ʱ;

            ZeroMemory(g_uInsNo[wIndex].strFilePath, MAX_PATH);
            lstrcpy(g_uInsNo[wIndex].strFilePath, g_strFilePath);
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
    for (wIndex = 0; wIndex < MAX_FILE_POST_INS; wIndex++)
    {
        if (g_uInsNo[wIndex].uSerInsNum != 0 && g_uInsNo[wIndex].nFlag == 0)
        {
            // ��ȡ�ļ�������Ϣ;
            g_uInsNo[wIndex].m_tFileInfo.fileLength = atoi(strMsgGet);

            // �����ļ��ְ���Ŀ;
            g_uInsNo[wIndex].m_tFileInfo.filePacketNum = g_uInsNo[wIndex].m_tFileInfo.fileLength/(MAX_FILE_PACKET - 4 - 2*sizeof(s32) - 3*sizeof(u16)) + 1;
            break;
        }
    }

    if (wIndex == MAX_FILE_POST_INS)
    {
        OspPrintf(TRUE, FALSE, "Server:FileLenInit, Get instance index error!!\r\n");
        return;
    }

    OspPrintf(TRUE, FALSE, "server received file attribute, name is : %s, length : %d, packageNum : %d\n",
        g_uInsNo[wIndex].m_tFileInfo.strFileName, g_uInsNo[wIndex].m_tFileInfo.fileLength, g_uInsNo[wIndex].m_tFileInfo.filePacketNum);
   
    return;
}

void CDemoInstance::InstanceEntry(CMessage *const pMsg)
{
    /*�õ���ǰ��Ϣ������;*/
    u16 wCurEvent = pMsg->event;

    switch (wCurEvent)
    {
        // ���ݲ�ͬ����Ϣ���ͽ��д���;
    case EVENT_MSG_POST_INS_ALLOT_ACK:
        MsgPostInsAllotAck(pMsg);
        NextState(STATE_WORK);
        //SetTimer(EVENT_TIMEOUT, 1000);
        break;
    case EVENT_FILE_POST_INS_ALLOT_ACK:
        FilePostInsAllotAck(pMsg);
        NextState(STATE_WORK);
        break;
    case EVENT_MSG_POST:
        OspPrintf(TRUE, FALSE, "received a message.\n");
        MsgPostFunc(pMsg);
        NextState(STATE_WORK);
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
    case EVENT_FILE_POST2S:
        OnServerReceive(pMsg);
        break;
    case EVENT_TERM:
        //Term_Fuction(pMsg);
        KillTimer(EVENT_TIMEOUT);
        NextState(IDLE_STATE);
        //�ظ�EVENT_QUIT��Ϣ
        // post(pMsg-> srcid, EVENT_QUIT, NULL, 0, pMsg->srcnode);
        break;
    case EVENT_TIMEOUT:
        //Timeout_Function();
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

// �������Ϣ����instance����;
void ServerMsgPostInsAllot(CMessage *const pcMsg, CApp* pcApp)
{
    s8 achInsNo[4];
    CPublic::g_uInsNum = GetIdleInsID(pcApp);
    OspPrintf(TRUE, FALSE, "Server:Get a idle instance, ID: %d.\r\n", CPublic::g_uInsNum);
    CPublic::g_uNodeNum = pcMsg->srcnode;

    ZeroMemory(achInsNo, 4);
    sprintf(achInsNo, "%d", CPublic::g_uInsNum);

    // �ظ���������뵽�Ŀ���instance�������͸��ͻ���;
    s32 nPostRet = OspPost(MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), EVENT_MSG_POST_INS_ALLOT_ACK, achInsNo, strlen(achInsNo), CPublic::g_uNodeNum);
    
    return;
}

// ������ļ�����instance����;
void ServerFilePostInsAllot(CMessage *const pcMsg, CApp* pcApp)
{
    s32 nIndex = 0;
    s8 achInsNo[4];

    for (nIndex = 0; nIndex < MAX_FILE_POST_INS; nIndex++)
    {
        if (g_uInsNo[nIndex].uSerInsNum == 0)
        {
            g_uInsNo[nIndex].uSerInsNum = GetIdleInsID(pcApp);
            if (g_uInsNo[nIndex].uSerInsNum == 0)
            {
                OspPrintf(TRUE, FALSE, "Server:have none idle instance.\r\n");
                return;
            }

            g_uInsNo[nIndex].nFlag = 0;
            OspPrintf(TRUE, FALSE, "Server:Get a idle instance, ID: %d.\r\n", g_uInsNo[nIndex].uSerInsNum);
            break;
        }
    }

    if (nIndex == MAX_FILE_POST_INS)
    {
        OspPrintf(TRUE, FALSE, "Server:reach its maximum.\r\n");
        return;
    }

    ZeroMemory(achInsNo, 4);
    sprintf(achInsNo, "%d", g_uInsNo[nIndex].uSerInsNum);

    // �ظ���������뵽�Ŀ���instance�������͸��ͻ���;
    s32 nPostRet = OspPost(MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), EVENT_FILE_POST_INS_ALLOT_ACK, achInsNo, strlen(achInsNo), CPublic::g_uNodeNum);

    return;
}

// �ͻ����ļ�����instance����;
void ClientFilePostInsAllot(CMessage *const pcMsg, CApp* pcApp)
{
    s32 nIndex = 0;
    for (nIndex = 0; nIndex < MAX_FILE_POST_INS; nIndex++)
    {
        if (g_uInsNo[nIndex].uCliInsNum == 0)
        {
            g_uInsNo[nIndex].uCliInsNum = GetIdleInsID(pcApp);
            OspPrintf(TRUE, FALSE, "GetIdleInsID:%d\r\n", g_uInsNo[nIndex].uCliInsNum);
            if (g_uInsNo[nIndex].uCliInsNum == 0)
            {
                OspPrintf(TRUE, FALSE, "Client:have none idle instance.\r\n");
                return;
            }

            g_uInsNo[nIndex].nFlag = 0;
            OspPrintf(TRUE, FALSE, "Client:Get a idle instance, ID: %d.\r\n", g_uInsNo[nIndex].uCliInsNum);
            break;
        }
    }

    if (nIndex == MAX_FILE_POST_INS)
    {
        OspPrintf(TRUE, FALSE, "Client:reach its maximum.\r\n");
        return;
    }

    return;
}


void CDemoInstance::DaemonInstanceEntry(CMessage *const pcMsg, CApp* pcApp)
{
    /*�õ���ǰ��Ϣ������;*/
    u16 wCurEvent = pcMsg->event;

    switch (wCurEvent)
    {
    case EVENT_SERVER_MSG_POST_INS_ALLOT:
        ServerMsgPostInsAllot(pcMsg, pcApp);
        NextState(STATE_WORK);
        break;
    case EVENT_SERVER_FILE_POST_INS_ALLOT:
        ServerFilePostInsAllot(pcMsg, pcApp);
        NextState(STATE_WORK);
        break;
    case EVENT_CLIENT_FILE_POST_INS_ALLOT:
        ClientFilePostInsAllot(pcMsg, pcApp);
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