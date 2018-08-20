#pragma once

#include "kdvtype.h"
#include "osp.h"

#define MAX_POST_MSG_LEN 255
#define MAX_POST_FILE_PACK_LEN 1024
#define MAX_INS 10
#define MAX_FILE_PACKET 10240 * 2
#define MAX_FILE_NAME 128

enum EVENT_TYPE
{
    EVENT_REQ = 1,
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

enum INS_STATE_TYPE
{
    IDLE_STATE = 0,
    STATE_ACK,
    STATE_TERM,
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
    // ��־����;
    char fileHead[4];
    // ���������������ļ��е���ʼλ�ñ�־;
    int fileStart;
    // �������ݵĳ���;
    int fileSize;
    // ������������ļ�����;
    BYTE filePacket[MAX_FILE_PACKET - 4 - 2*sizeof(int)];
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
    long fileLength;
    // �ļ�����;
    char strFileName[MAX_FILE_NAME + 1];
};

// ��Ŵ�������ļ�;
BYTE *m_fileRdBuff = NULL;
// ��Ŵ������ļ��ĳ���;
UINT m_length = 0;
// ��¼�ļ���������е������Ϣ;
FILEINFO m_fileInfo = {0};
// ��Ž��ܵ����ļ�����;
BYTE *m_fileWrtBuff = NULL;

BYTE m_sendBuffer[MAX_FILE_PACKET + 1];   // ����������buffer;
UINT m_nLength = 0;                       // ���������ݳ���;
BYTE m_recBuffer[MAX_FILE_PACKET + 1];    // �ļ�������ջ���;
BYTE m_filePacket[MAX_FILE_PACKET + 1];   // �����ļ�������;

// client read;
CString g_strFilePath = _T("");
CString g_strFileName = _T("");
CString strFilePath2W = _T("");
char strFileLen[10] = {0};

ULONGLONG g_ustrLen = 0;

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
    static u32 g_ustrLen;
    
    static int m_lastStart;
    static int m_lastSize;
    static int m_errorIndex;
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
typedef zTemplate<CDemoInstance, MAX_INS> CDemoApp;
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
int CPublic::m_lastStart = 0;
int CPublic::m_lastSize  = 0;
int CPublic::m_errorIndex = 0;

// CDemoInstance �ඨ�弰��ʼ��;
void MsgPostFunc(CMessage *const pMsg)
{
    // ��ô���ָ��
    CWnd *pBoxOne = AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT4);

    // ��ȡ���е�Instance;
    //if (pMsg->dstid == MAKEIID(1, CInstance::PENDING))
    if (0)
    {
        u16 i = 1;
        for (; i != MAX_INS; i++)
        {
            //CInstance *pDIns = g_cDemoApp.GetInstance(i);
            //g_cDemoApp.

        }
        CPublic::g_uInsNum = i;
    }

    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];
    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);
    OspPrintf(TRUE, FALSE, "message content is: %s, length is: %d.\n", strMsgGet, pMsg->length);

    // ��ֵ;
    CString cMsgContent(strMsgGet);
    pBoxOne->SetWindowText(cMsgContent);

    // ��ȡԴ����;
    CPublic::g_uNodeNum = pMsg->srcnode;

    // �ͷſռ�;
    delete [] strMsgGet;
    strMsgGet = NULL;
}

void CreateFileBuff(CMessage *const pMsg)
{
    if (m_fileWrtBuff != NULL)
    {
        delete[] m_fileWrtBuff;
        m_fileWrtBuff = NULL;
    }

    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];
    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);
    long length = atoi(strMsgGet);
    delete [] strMsgGet;
    strMsgGet = NULL;

    m_fileInfo.fileLength = length;       // �ڱ���m_fileInfo�б����ļ�������Ϣ;
    if (m_fileInfo.fileLength != 0)        // �����ļ�����ռ�;
    {
        if (m_fileInfo.fileLength < 10)
        {
            // ע���˴������ڴ��Сʱ�����DAMAGE after Normal block����;
            m_fileWrtBuff = new BYTE[16];
        }
        else
        {
            m_fileWrtBuff = new BYTE[m_fileInfo.fileLength+1];
        }

        m_fileInfo.filePacketIndex = 0;
        m_fileInfo.fileStart = 0;
        m_fileInfo.lastStart = 0;
        m_fileInfo.lastSize = 0;
        m_fileInfo.nFilePacketBuff = MAX_FILE_PACKET - 4 - 2*sizeof(int);
        m_fileInfo.filePacketNum = m_fileInfo.fileLength/m_fileInfo.nFilePacketBuff + 1;
        // ���ǵ��ļ��ְ����ܳ���ֻ��һ��������������� m_fileInfo.fileSize�ڳ�ʼ����ʱ��Ҫ�����������;
        if (m_fileInfo.filePacketNum == 1)
        {
            m_fileInfo.fileSize  = m_fileInfo.fileLength + 4 + 2*sizeof(int) + 1;
        }
        else
        {
            m_fileInfo.fileSize = MAX_FILE_PACKET+1;
        }
        memset(m_fileWrtBuff,0,sizeof(m_fileWrtBuff));
    }
}

void sendFileInfo(int fStart,int fSize,char *fHead)
{
    UINT nFilePacketBuff = MAX_FILE_PACKET - 4 - 2*sizeof(int);
    FILEMESSAGE strFileMsg;
    // �������ڷ����ļ����ݵ��ļ�����fileHead����Ϊ��FFF��;
    strFileMsg.fileHead[0] = 'F';
    strFileMsg.fileHead[1] = 'F';
    strFileMsg.fileHead[2] = 'F';
    strFileMsg.fileHead[3] = '\0';
    strFileMsg.fileStart   = 0;
    strFileMsg.fileSize    = 0;

    OspPrintf(TRUE, FALSE, "enter sendFileInfo\n");

    // �Ƿ�����ϴηְ���ƫ��λ��;
    if (fStart == CPublic::m_lastStart && fSize == CPublic::m_lastSize)
    {
        // ���һ�����������;
        //if (fStart == 0 && fSize == 0 && !strcmp(fHead, "END"))
        if (!strcmp(fHead, "END"))
        {
            OspPrintf(TRUE, FALSE, "enter END\n");
            CString temp=_T("");
            //GetDlgItemText(IDC_EDIT_CLIENT,temp);
            temp += _T("\r\n");
            temp += _T("�ļ��������");
            //SetDlgItemText(IDC_EDIT_CLIENT,temp);
            return;
        }
        // ���͵�һ����;
        else if (fStart == 0 && fSize == 0 && !strcmp(fHead,"0"))
        {
            // ��������־;
            CPublic::m_errorIndex = 0;
            // ����ֻ��һ���������;
            if (m_length <= nFilePacketBuff)
            {
                strFileMsg.fileStart = 0;
                strFileMsg.fileSize  = m_length;
                CPublic::m_lastStart = 0;
                CPublic::m_lastSize  = m_length;
            }
            else
            {
                strFileMsg.fileStart = 0;
                strFileMsg.fileSize  = nFilePacketBuff;
                CPublic::m_lastStart = 0;
                CPublic::m_lastSize  = nFilePacketBuff;
            }
        }
        // ���յ�����ȷ�ϰ���ʱ��;
        else if (!strcmp(fHead,"OK!"))
        {
            OspPrintf(TRUE, FALSE, "next package\n");
            CPublic::m_errorIndex = 0;
            // ��Ҫ�������һ������ʱ��;
            if ((m_length - fStart - nFilePacketBuff) <= nFilePacketBuff)
            {
                strFileMsg.fileStart = fStart + nFilePacketBuff;
                //strFileMsg.fileSize  = m_length % nFilePacketBuff;
                strFileMsg.fileSize  = m_length - strFileMsg.fileStart;
                OspPrintf(TRUE, FALSE, "END package length is :%d\n", strFileMsg.fileSize);
                CPublic::m_lastStart = strFileMsg.fileStart;
                CPublic::m_lastSize  = strFileMsg.fileSize;
            }
            else
            {
                strFileMsg.fileStart = fStart + nFilePacketBuff;
                strFileMsg.fileSize  = nFilePacketBuff;
                CPublic::m_lastStart = strFileMsg.fileStart;
                CPublic::m_lastSize  = strFileMsg.fileSize;
            }
        }
        // ȷ�ϰ���ʾ���յ��İ��д��������·�����һ����;
        else if (!strcmp(fHead,"ERR"))   
        {
            CPublic::m_errorIndex++;
            strFileMsg.fileStart = CPublic::m_lastStart;
            strFileMsg.fileSize = CPublic::m_lastSize;
        }
        // ȷ�ϰ���ʾ�û�ȡ�����ļ�����;
        else if (!strcmp(fHead,"CCL"))
        {
            CString temp=_T("");
            //GetDlgItemText(IDC_EDIT_CLIENT,temp);
            temp += _T("\r\n");
            temp += _T("�Է�ȡ�����ļ�����");
            //SetDlgItemText(IDC_EDIT_CLIENT,temp);
            return;
        }
        // ���յ�����Ϣ������ʱĬ�ϳ������·�����һ����;
        else
        {
            CPublic::m_errorIndex++;
            strFileMsg.fileStart = CPublic::m_lastStart;
            strFileMsg.fileSize = CPublic::m_lastSize;
        }
    }
    // ���յ�����Ϣ������ʱĬ�ϳ������·�����һ����;
    else 
    {
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
    ZeroMemory(strFileMsg.filePacket, sizeof(BYTE)*(MAX_FILE_PACKET) - 4 - 2*sizeof(int));

    OspPrintf(TRUE, FALSE, "start read the file message\n");
    // ��ȡһ����������;
    //CFile mFile(g_strFilePath, CFile::modeRead | CFile::typeBinary);
    CFile mFile(g_strFilePath, CFile::modeRead);
    mFile.Seek(strFileMsg.fileStart, CFile::begin);
    mFile.Read(strFileMsg.filePacket, strFileMsg.fileSize);

    mFile.Close();
    //memcpy(strFileMsg.filePacket, m_fileRdBuff + strFileMsg.fileStart, strFileMsg.fileSize);
    //strFileMsg.filePacket[strFileMsg.fileSize] = '\0';

    BYTE* filePacket = (BYTE*)&strFileMsg;
    // �������Ͱ���������������ͻ�����;
    memcpy(m_sendBuffer, filePacket, strFileMsg.fileSize + 4 + 2*sizeof(int));
    m_nLength = strFileMsg.fileSize + 4 + 2*sizeof(int);

    OspPrintf(TRUE, FALSE, "start post the file message to server\n");
    // ���Ͱ��������;
    OspPost(MAKEIID(1, CPublic::g_uInsNum), EVENT_FILE_POST2S, m_sendBuffer, m_nLength, CPublic::g_uNodeNum, MAKEIID(2, 1));
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
    int m_fileStart = strFileMsg->fileStart;
    int m_fileSize  = strFileMsg->fileSize;

    // ��ȡ��Ϣ���ʹ���;
    sendFileInfo(m_fileStart, m_fileSize, cFileHead);

    delete [] strMsgGet;
    strMsgGet = NULL;
}

//void ReceiveFilePacket(int nErrorCode)     //��Socket�Ľ��ջ������ж�ȡ����;
//void SendFilePacketEcho(int nFlag);        //��Client�����ļ���Ӧ��Ϣ;
//bool StoreFilePacket();                    //������յ����ļ���;
//bool IsFilePacket();                       //�жϲ���֤�ļ�������;

bool StoreFilePacket(FILEMESSAGE *strFileMsgGet)
{
    m_fileInfo.fileStart = strFileMsgGet->fileStart;
    m_fileInfo.fileSize = strFileMsgGet->fileSize;

    //CFile mFileW(strFilePath2W, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::typeBinary);
    CFile mFileW(strFilePath2W, CFile::modeReadWrite | CFile::modeCreate | CFile::modeNoTruncate);
    if (mFileW.GetLength() <= m_fileInfo.fileLength - strFileMsgGet->fileSize)
    if (1)
    {
        mFileW.SeekToEnd();
        mFileW.Flush();
        mFileW.Write(strFileMsgGet->filePacket, strFileMsgGet->fileSize);
        mFileW.Flush();
        mFileW.Close();

        m_fileInfo.lastStart = m_fileInfo.fileStart;
        m_fileInfo.lastSize = m_fileInfo.fileSize;

        return TRUE;
    }

    mFileW.Flush();
    mFileW.Close();

    return FALSE;
}

void SendFilePacketEcho(int nFlag)    //��Client����ȷ�ϰ���m_fileMsgSend��CMySocket�ı���;
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
        //��ʱȫ��byteָ��m_fileBuffer�д����ȫ�����ļ���Ϣ������һЩ���湤������;
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
    strFileMsgAck.fileStart = m_fileInfo.lastStart; 
    strFileMsgAck.fileSize = m_fileInfo.lastSize;
    ZeroMemory(strFileMsgAck.filePacket, MAX_FILE_PACKET - 4 - 2*sizeof(int));
    BYTE* newPacket = (BYTE*)&strFileMsgAck;
    int len = 4 + 2*sizeof(int);
    
    // ���Ͱ����ͻ���
    OspPost(MAKEIID(2, 1), EVENT_FILE_POST2C, newPacket, len, CPublic::g_uNodeNum, MAKEIID(1, CPublic::g_uInsNum));
#if 0
    if(m_fileInfo.filePacketIndex == m_fileInfo.filePacketNum-1) //������һ�ν������ݵĴ�С;
    {
        m_fileInfo.fileSize = m_fileInfo.fileLength%m_fileInfo.nFilePacketBuff + 4 + 2*sizeof(int);
    }
    else
    {
        m_fileInfo.fileSize = MAX_FILE_PACKET;
    }

#endif
}

void ReceiveFilePacket(FILEMESSAGE *strFileMsgGet)
{
    //int RecFilePkgSize = strFileMsgGet->fileSize;
    m_fileInfo.filePacketIndex++;
    //ZeroMemory(m_recBuffer, sizeof(BYTE)*(MAX_FILE_PACKET + 1));

    // �յ��İ�Ϊ���һ����;
    if (m_fileInfo.filePacketIndex == m_fileInfo.filePacketNum)
    {
        if (StoreFilePacket(strFileMsgGet) == TRUE)
        {
            SendFilePacketEcho(FILE_PACKET_END);
            return;
        }

        SendFilePacketEcho(FILE_PACKET_ERROR);
        return;
    }
    // �յ��İ�Ϊ���������һ��к�����;
    else if (m_fileInfo.filePacketIndex < m_fileInfo.filePacketNum)
    {
        if (StoreFilePacket(strFileMsgGet) == TRUE)
        {
            SendFilePacketEcho(FILE_PACKET_OK);
            return;
        }

        SendFilePacketEcho(FILE_PACKET_ERROR);
        return;
    }
    else
    {
        SendFilePacketEcho(FILE_PACKET_ERROR);
    }
    return;
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
#if 0
    // ���յ����һ�������ͷ���һ��ȷ�ϰ�����Client��������;
    else if (!strncmp(strFileMsgGet->fileHead, "END", 3))
    {
        SendFilePacketEcho(FILE_PACKET_END);
    }

    // �û�ȡ�����䣬�ͷ���һ��ȷ�ϰ�����Clientȡ������;
    else if (!strncmp(strFileMsgGet->fileHead, "CCL", 3))
    {
        SendFilePacketEcho(FILE_PACKET_CANCEL);
    }
#endif
    // ����client���͵Ĵ�����Ϣ����ֹ�ļ�����;
    else if (!strncmp(strFileMsgGet->fileHead, "ERR", 3))
    {
        // -- TODO:ɾ���ļ�;
        SendFilePacketEcho(FILE_PACKET_CANCEL);
    }

    strFileMsgGet = NULL;
    delete [] strMsgGet;
    strMsgGet = NULL;


}

void FileMsgInit(CMessage *const pMsg)
{
    // ��ȡ�յ�����Ϣ����;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];
    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    ZeroMemory(m_fileInfo.strFileName, MAX_FILE_NAME + 1);
    strncpy(m_fileInfo.strFileName, strMsgGet, MsgLen);
    m_fileInfo.strFileName[MsgLen] = '\0';
    m_fileInfo.filePacketIndex = 0;
}

void FileLenInit(CMessage *const pMsg)
{
    // ��ȡ�յ�����Ϣ����;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];
    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    m_fileInfo.fileLength = atoi(strMsgGet);
    m_length = m_fileInfo.fileLength;
    m_fileInfo.filePacketNum = m_fileInfo.fileLength/(MAX_FILE_PACKET - 4 - 2*sizeof(int)) + 1;
    OspPrintf(TRUE, FALSE, "server received file attribute, name is : %s, length : %d, packageNum : %d\n", m_fileInfo.strFileName, m_length, m_fileInfo.filePacketNum);
    m_fileInfo.fileStart = 0;
    m_fileInfo.fileSize = 0;
    m_fileInfo.lastStart = 0;
    m_fileInfo.lastSize = 0;
    m_fileInfo.filePacketIndex = 0;
}

void CDemoInstance::InstanceEntry(CMessage *const pMsg)
{
    u16 wCurEvent = pMsg->event;
    /*�õ���ǰ��Ϣ������;*/

    switch (wCurEvent)
    {
        // ���ݲ�ͬ����Ϣ���ͽ��д���;
    case EVENT_REQ:
        // ReqFuction(pMsg);
        NextState(STATE_ACK);				//���״̬��Ծ;
        SetTimer(EVENT_TIMEOUT, 1000); 		//���ж�ʱ����;
        break;
    case EVENT_MSG_POST:
        MsgPostFunc(pMsg);
        OspPrintf(TRUE, FALSE, "received a message.\n");
        NextState(IDLE_STATE);
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

void CDemoInstance::DaemonInstanceEntry(CMessage *const pcMsg, CApp* pcApp)
{
    //do something
    OspPrintf(TRUE, FALSE, "i am indoor\n");
    CPublic::g_uInsNum = 2;
    return;
}

CDemoInstance::~CDemoInstance()
{

}

