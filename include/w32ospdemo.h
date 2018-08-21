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
#define MAX_FILE_POST_INS 3

#define INS_MSG_POST_NO 1
#define INS_FILE_POST_NO 2

#define DEMO_APP_SERVER_NO 1
#define DEMO_APP_CLIENT_NO 2
#define DEMO_APP_PRIO 100
#define DEMO_APP_QUE_SIZE (200 << 10)

#define DEMO_POST_TIMEOUT 2000

enum EM_EVENT_TYPE
{
    EVENT_ACK = 1,
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
    EVENT_SERVER_INS_ALLOT = 1,
    EVENT_CLIENT_INS_ALLOT
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
    char fileHead[4];       // ��־����;
    s32  fileStart;         // ���������������ļ��е���ʼλ�ñ�־;
    s32  fileSize;          // �������ݵĳ���;
    u16  wCliPostInsNum;    // ���м�¼�ͻ��˵ķ���instanceID;
    u16  wSerPostInsNum;    // ���м�¼����˵ķ���instanceID;
    u16  wIndex;            // ���м�¼�ͻ��˵ķ���instance������;
    BYTE filePacket[MAX_FILE_PACKET - 4 - 2*sizeof(int)];       // ������������ļ�����;
};

// �ļ���Ϣ�ṹ��;
struct FILEINFO
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
};

// ��¼�ļ���������е������Ϣ;
FILEINFO m_fileInfo = {0};
// ��Ŵ������ļ��ĳ���;
DWORD m_length = 0;

BYTE m_sendBuffer[MAX_FILE_PACKET + 1];   // ����������buffer;
UINT m_nLength = 0;                       // ���������ݳ���;

// client read;
TCHAR g_strFilePath[MAX_PATH] = _T("");
TCHAR g_strFileName[MAX_FILE_NAME] = _T("");
TCHAR g_strFolderPath[MAX_PATH] = _T("F:\\2");
CHAR strFileLen[16] = {0};

typedef void (*func)(s32, s32, const char*);
HANDLE g_hTask;
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
#if 0
    static s32 g_threadno[MAX_THREADNO];
    static s32 m_lastStart[MAX_THREADNO];
    static s32 m_lastSize[MAX_THREADNO];
    static s32 m_PktIndex[MAX_THREADNO];
    static s32 m_errorIndex[MAX_THREADNO];
#else
    static s32 g_threadno;
    static s32 m_lastStart;
    static s32 m_lastSize;
    static s32 m_PktIndex;
    static s32 m_errorIndex;
#endif
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

#if 1 // ȫ�ֱ�����ʼ��;
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
    u16 iSerInsNum;
    s32 nFlag;
    s32 nLastStart;
    s32 nLastSize;
    s32 nPktIndex;
    s32 nErrorIndex;
}TInStatus;

TInStatus g_uInsNo[MAX_FILE_POST_INS] = {0};

void sendFileInfo(s32 fStart,s32 fSize,char *fHead, u16 wCliPostInsNo, u16 wSerPostInsNo, u16 wIndex)
{
    LONG64 iValue = 0;
    TCHAR szProgress[16] = {0};
    CProgressUI* pProgress =  pFrame->m_pProgress;

    // Client�ˣ��������滭����ʾ;
    iValue = 100 * CPublic::m_PktIndex / m_fileInfo.filePacketNum;
    wsprintf(szProgress, L"Progress(%ld%%)", iValue);
    pProgress->SetValue((int)iValue);
    pProgress->SetText(szProgress);

    UINT nFilePacketBuff = MAX_FILE_PACKET - 4 - 2*sizeof(s32) - sizeof(u16);
    FILEMESSAGE strFileMsg;
    // �������ڷ����ļ����ݵ��ļ�����fileHead����Ϊ��FFF��;
    strFileMsg.fileHead[0] = 'F';
    strFileMsg.fileHead[1] = 'F';
    strFileMsg.fileHead[2] = 'F';
    strFileMsg.fileHead[3] = '\0';

    // ���м�¼�ͻ��˵ķ���instance ID;
    strFileMsg.wCliPostInsNum = wCliPostInsNo;

    // ��ʼ��;
    strFileMsg.fileStart   = 0;
    strFileMsg.fileSize    = 0;

    // �Ƿ�����ϴηְ���ƫ��λ��;
    if (fStart == CPublic::m_lastStart && fSize == CPublic::m_lastSize)
    {
        // ���һ�����������;
        //if (fStart == 0 && fSize == 0 && !strcmp(fHead, "END"))
        if (!strcmp(fHead, "END"))
        {
            OspPrintf(TRUE, FALSE, "Enter END.\r\n");
            CloseHandle(g_hTask);
            //�������ö�������ļ�����;
            pProgress->SetValue(100);
            pProgress->SetText(L"Progress(100%)");

            // �ļ�У�鲢��ʾ���; --TODO
            ::MessageBox(NULL, _T("������ļ����䣡�ļ�����OK!!"), _T("�ļ�������"), NULL);
            return;
        }
        // ���͵�һ����;
        else if (fStart == 0 && fSize == 0 && !strcmp(fHead,"0"))
        {
            // ��������־����������0;
            CPublic::m_errorIndex = 0;
            CPublic::m_PktIndex  = 0;

            // ����ֻ��һ���������;
            if (m_length <= nFilePacketBuff)
            {
                strFileMsg.fileStart = 0;
                strFileMsg.fileSize  = m_length;
                CPublic::m_lastStart = 0;
                CPublic::m_lastSize  = m_length;
                OspPrintf(TRUE, FALSE, "Only one packet. Index(%d)\r\n", CPublic::m_PktIndex);
            }
            else
            {
                strFileMsg.fileStart = 0;
                strFileMsg.fileSize  = nFilePacketBuff;
                CPublic::m_lastStart = 0;
                CPublic::m_lastSize  = nFilePacketBuff;
                OspPrintf(TRUE, FALSE, "First packet. Index(%d)\r\n", CPublic::m_PktIndex);
            }
        }
        // ���յ�server�˻ظ�������ȷ�ϰ�;
        else if (!strcmp(fHead,"OK!"))
        {
            // ��������־��0;
            CPublic::m_errorIndex = 0;
            // �����������ۼ�;
            CPublic::m_PktIndex++;

            // ��Ҫ�������һ������ʱ��;
            if ((m_length - fStart - nFilePacketBuff) <= nFilePacketBuff)
            {
                strFileMsg.fileStart = fStart + nFilePacketBuff;
                //strFileMsg.fileSize  = m_length % nFilePacketBuff;
                strFileMsg.fileSize  = m_length - strFileMsg.fileStart;
                CPublic::m_lastStart = strFileMsg.fileStart;
                CPublic::m_lastSize  = strFileMsg.fileSize;
                OspPrintf(TRUE, FALSE, "Last packet. Index(%d)\r\n", CPublic::m_PktIndex);
            }
            else
            {
                strFileMsg.fileStart = fStart + nFilePacketBuff;
                strFileMsg.fileSize  = nFilePacketBuff;
                CPublic::m_lastStart = strFileMsg.fileStart;
                CPublic::m_lastSize  = strFileMsg.fileSize;
                OspPrintf(TRUE, FALSE, "Next packet. Index(%d)\r\n", CPublic::m_PktIndex);
            }
        }
        // ȷ�ϰ���ʾ���յ��İ��д��������·�����һ����;
        else if (!strcmp(fHead, "ERR"))   
        {
            OspPrintf(TRUE, FALSE, "Error packet. Index(%d)\r\n", CPublic::m_PktIndex);
            CPublic::m_errorIndex++;
            strFileMsg.fileStart = CPublic::m_lastStart;
            strFileMsg.fileSize = CPublic::m_lastSize;
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
            if (OspTaskSuspend(g_hTask) == -1)
            {
                OspPrintf(TRUE, FALSE, "OspTaskSuspend Failed.\r\n\r\n\r\n");
            }
            Sleep(1);
        }
        // ���յ�����Ϣ������ʱĬ�ϳ������·�����һ����;
        else
        {
            OspPrintf(TRUE, FALSE, "Repeat the previous packet. Index(%d)\r\n", CPublic::m_PktIndex);
            CPublic::m_errorIndex++;
            strFileMsg.fileStart = CPublic::m_lastStart;
            strFileMsg.fileSize = CPublic::m_lastSize;
        }
    }
    // ���յ�����Ϣ������ʱĬ�ϳ������·�����һ����;
    else 
    {
        OspPrintf(TRUE, FALSE, "Other error, Repeat the previous packet. Index(%d)\r\n", CPublic::m_PktIndex);
        CPublic::m_errorIndex++;
        strFileMsg.fileStart = CPublic::m_lastStart;
        strFileMsg.fileSize = CPublic::m_lastSize;
    }

    // ������Խ��г�����;
    if (CPublic::m_errorIndex > 10)       
    {
        strFileMsg.fileHead[0] = 'E';
        strFileMsg.fileHead[1] = 'R';
        strFileMsg.fileHead[2] = 'R';
        strFileMsg.fileHead[3] = '\0';
    }

    ZeroMemory(m_sendBuffer, sizeof(BYTE)*(MAX_FILE_PACKET) + 1);
    ZeroMemory(strFileMsg.filePacket, sizeof(BYTE)*(MAX_FILE_PACKET) - 4 - 2*sizeof(s32) - sizeof(u16));

    // ��ȡһ����������;
    OspPrintf(TRUE, FALSE, "Start to read the file message.\r\n");

    DWORD nByte;

    HANDLE mFile = CreateFile(g_strFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
    memcpy(m_sendBuffer, filePacket, strFileMsg.fileSize + 4 + 2*sizeof(s32) + sizeof(u16));
    m_nLength = strFileMsg.fileSize + 4 + 2*sizeof(s32) + sizeof(u16);

    OspPrintf(TRUE, FALSE, "Start to post the file message to server. InsNo:%d\r\n", wPostInsNo);

    // ���Ͱ��������;
    OspPost(MAKEIID(DEMO_APP_SERVER_NO, wPostInsNo), EVENT_FILE_POST2S, m_sendBuffer,
        m_nLength, CPublic::g_uNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, wPostInsNo));
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
    u16 wPostInsNo = strFileMsg->wPostInsNum;

    // ��ͣ����;
    while(g_PauseFlag != 0)
    {
        Sleep(1000);
    }

    // ��ȡ��Ϣ���ʹ���;
    sendFileInfo(nFileStart, nFileSize, cFileHead, wPostInsNo);

    delete [] strMsgGet;
    strMsgGet = NULL;
}

//void ReceiveFilePacket(int nErrorCode)     //��Socket�Ľ��ջ������ж�ȡ����;
//void SendFilePacketEcho(int nFlag);        //��Client�����ļ���Ӧ��Ϣ;
//bool StoreFilePacket();                    //������յ����ļ���;
//bool IsFilePacket();                       //�жϲ���֤�ļ�������;

bool StoreFilePacket(FILEMESSAGE *strFileMsgGet)
{
    DWORD nByte;

    OspPrintf(TRUE, FALSE, "Server: Start to store the packet\r\n");
    m_fileInfo.fileStart = strFileMsgGet->fileStart;
    m_fileInfo.fileSize = strFileMsgGet->fileSize;

    // �����ļ���д���ļ�;
    HANDLE mFileW = CreateFile(g_strFilePath, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
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
    m_fileInfo.lastStart = m_fileInfo.fileStart;
    m_fileInfo.lastSize = m_fileInfo.fileSize;

    return TRUE;
}

void SendFilePacketEcho(s32 nFlag, u16 wPostInsNo)    //��Client����ȷ�ϰ���m_fileMsgSend��CMySocket�ı���;
{
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
#if 0
    if(m_fileInfo.lastSize>m_fileInfo.nFilePacketBuff || m_fileInfo.lastSize<0 || m_fileInfo.lastStart>m_fileInfo.fileLength
        || m_fileInfo.lastStart<0 || m_fileInfo.filePacketIndex>m_fileInfo.filePacketNum || m_fileInfo.filePacketIndex<0)
    {
        return;
    }
#endif
    strFileMsgAck.fileStart   = m_fileInfo.lastStart; 
    strFileMsgAck.fileSize    = m_fileInfo.lastSize;
    strFileMsgAck.wPostInsNum = wPostInsNo;

    ZeroMemory(strFileMsgAck.filePacket, MAX_FILE_PACKET - 4 - 2*sizeof(s32) - sizeof(u16));
    BYTE* newPacket = (BYTE*)&strFileMsgAck;
    u16 uLen = 4 + 2*sizeof(s32) + sizeof(u16);
    
    // ���Ͱ����ͻ���
    OspPost(MAKEIID(DEMO_APP_CLIENT_NO, wPostInsNo), EVENT_FILE_POST2C, newPacket, uLen, CPublic::g_uNodeNum);
}

void ReceiveFilePacket(FILEMESSAGE *strFileMsgGet)
{
    LONG64 iValue = 0;
    TCHAR szProgressText[16] = {0};

    u16 wPostInsNo = strFileMsgGet->wPostInsNum;
    CProgressUI* pProgress =  pFrame->m_pProgress;

    // Server�ˣ��������滭����ʾ;
    iValue = 100 * m_fileInfo.filePacketIndex / m_fileInfo.filePacketNum;
    wsprintf(szProgressText, L"Progress(%ld%%)", iValue);
    pProgress->SetValue((int)iValue);
    pProgress->SetText(szProgressText);

    m_fileInfo.filePacketIndex++;
    //ZeroMemory(m_recBuffer, sizeof(BYTE)*(MAX_FILE_PACKET + 1));

    // �յ��İ�Ϊ���һ����;
    if (m_fileInfo.filePacketIndex == m_fileInfo.filePacketNum)
    {
        if (StoreFilePacket(strFileMsgGet) == TRUE)
        {
            SendFilePacketEcho(FILE_PACKET_END, wPostInsNo);

            //�������ö�������ļ�����;
            pProgress->SetValue(100);
            pProgress->SetText(L"Progress(100%)");

            // �ļ�У�鲢��ʾ���; --TODO
            ::MessageBox(NULL, _T("������ļ����䣡�ļ�����OK!!"), _T("�ļ�������"), NULL);
            return;
        }

        OspPrintf(TRUE, FALSE, "Server: Store file packet fail!\r\n");
        SendFilePacketEcho(FILE_PACKET_ERROR, wPostInsNo);
        return;
    }
    // �յ��İ�Ϊ���������һ��к�����;
    else if (m_fileInfo.filePacketIndex < m_fileInfo.filePacketNum)
    {
        if (StoreFilePacket(strFileMsgGet) == TRUE)
        {
            SendFilePacketEcho(FILE_PACKET_OK, wPostInsNo);
            return;
        }

        OspPrintf(TRUE, FALSE, "Server: Store file packet fail!\r\n");
        SendFilePacketEcho(FILE_PACKET_ERROR, wPostInsNo);
        return;
    }
    else
    {
        OspPrintf(TRUE, FALSE, "Server: Not current packet to save!\r\n");
        SendFilePacketEcho(FILE_PACKET_ERROR, wPostInsNo);
    }
    return;
}

// InstanceEntry:event ��Ϣ����������;
//////////////////////////////////////////////

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

// server���������instance�ظ�������;
void InsRetFunc(CMessage *const pMsg)
{
    // ��Ϣ������ȡ;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];

    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    CPublic::g_uInsNum = atoi(strMsgGet);

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
void FileInfoInit()
{
    // ��ʼ���ļ���Ϣ����;
    m_fileInfo.filePacketNum = 0;
    m_fileInfo.filePacketIndex = 0;
    m_fileInfo.nFilePacketBuff = 0;

    m_fileInfo.fileStart  = 0;
    m_fileInfo.fileSize   = 0;
    m_fileInfo.lastStart  = 0;
    m_fileInfo.lastSize   = 0;
    m_fileInfo.fileLength = 0;

    ZeroMemory(m_fileInfo.strFileName, MAX_FILE_NAME + 1);
}

void FileMsgInit(CMessage *const pMsg)
{
    // �ļ���Ϣ��ʼ��;
    FileInfoInit();

    // ��ȡ�յ�����Ϣ����;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];
    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    // ��ȡ�ļ�����Ϣ;
    memcpy_s(m_fileInfo.strFileName, MAX_FILE_NAME, strMsgGet, MsgLen);
    m_fileInfo.strFileName[MsgLen] = '\0';

    // �ļ��洢·������;
    USES_CONVERSION;
    ZeroMemory(g_strFilePath, MAX_PATH);
    lstrcat(g_strFilePath, g_strFolderPath);
    lstrcat(g_strFilePath, L"\\");
    lstrcat(g_strFilePath, A2W(m_fileInfo.strFileName));
}

void FileLenInit(CMessage *const pMsg)
{
    // ��ȡ�յ�����Ϣ����;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];
    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    // ��ȡ�ļ�������Ϣ;
    m_fileInfo.fileLength = atoi(strMsgGet);
    m_length = m_fileInfo.fileLength;

    // �����ļ��ְ���Ŀ;
    m_fileInfo.filePacketNum = m_fileInfo.fileLength/(MAX_FILE_PACKET - 4 - 2*sizeof(s32) - sizeof(u16)) + 1;
    OspPrintf(TRUE, FALSE, "server received file attribute, name is : %s, length : %d, packageNum : %d\n", m_fileInfo.strFileName, m_fileInfo.fileLength, m_fileInfo.filePacketNum);
   
}

void CDemoInstance::InstanceEntry(CMessage *const pMsg)
{
    /*�õ���ǰ��Ϣ������;*/
    u16 wCurEvent = pMsg->event;

    switch (wCurEvent)
    {
        // ���ݲ�ͬ����Ϣ���ͽ��д���;
    case EVENT_ACK:
        InsRetFunc(pMsg);
        NextState(STATE_WORK);				//���״̬��Ծ;
        //SetTimer(EVENT_TIMEOUT, 1000);      //���ж�ʱ����;
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
    u16 wIndex = 1;
    CInstance *pCInst = NULL;

    // ��������instance��Ѱ�ҵ�һ�����е�instance������;
    for (wIndex; wIndex <= MAX_INS_NO; wIndex++)
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
    }

    return wIndex;
}

// �����instance����;
void ServerInsAllot(CMessage *const pcMsg, CApp* pcApp)
{
    CHAR strInsNo[4];
    CPublic::g_uInsNum = GetIdleInsID(pcApp);
    OspPrintf(TRUE, FALSE, "Server:Get a idle instance, ID: %d.\r\n", CPublic::g_uInsNum);
    CPublic::g_uNodeNum = pcMsg->srcnode;

    ZeroMemory(strInsNo, 4);
    sprintf(strInsNo, "%d", CPublic::g_uInsNum);

    // �ظ���������뵽�Ŀ���instance�������͸��ͻ���;
    s32 nPostRet = OspPost(MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), EVENT_ACK, strInsNo, strlen(strInsNo), CPublic::g_uNodeNum);
    
    return;
}

// �ͻ���instance����;
void ClientInsAllot(CMessage *const pcMsg, CApp* pcApp)
{
    s32 nIndex = 0;
    for (nIndex; nIndex < MAX_FILE_POST_INS; nIndex++)
    {
        if (g_uInsNo[nIndex].uCliInsNum == 0)
        {
            g_uInsNo[nIndex].uCliInsNum = GetIdleInsID(pcApp);
            g_uInsNo[nIndex].nFlag = 0;
            OspPrintf(TRUE, FALSE, "Client:Get a idle instance, ID: %d.\r\n", g_uInsNo[nIndex].uCliInsNum);
            break;
        }
    }

    if (nIndex == MAX_FILE_POST_INS)
    {
        OspPrintf(TRUE, FALSE, "Client:have none idle instance.\r\n");
    }

    return;
}


void CDemoInstance::DaemonInstanceEntry(CMessage *const pcMsg, CApp* pcApp)
{
    /*�õ���ǰ��Ϣ������;*/
    u16 wCurEvent = pcMsg->event;

    switch (wCurEvent)
    {
    case EVENT_SERVER_INS_ALLOT:
        ServerInsAllot(pcMsg, pcApp);
        NextState(STATE_WORK);
        break;
    case EVENT_CLIENT_INS_ALLOT:
        ClientInsAllot(pcMsg, pcApp);
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