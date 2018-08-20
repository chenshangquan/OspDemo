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

// 包的结构体;
struct FILEMESSAGE
{
    // 标志作用;
    char fileHead[4];
    // 包中数据在整个文件中的起始位置标志;
    int fileStart;
    // 包中数据的长度;
    int fileSize;
    // 包中所传输的文件数据;
    BYTE filePacket[MAX_FILE_PACKET - 4 - 2*sizeof(int)];
};

// 文件信息结构体;
struct FILEINFO
{
    // 文件分包数;
    int filePacketNum;
    // 包的索引，即为第几个包;
    int filePacketIndex;
    // 实际用于传输文件数据的大小;
    int nFilePacketBuff;
    // 当前包的偏移位置;
    int fileStart;
    // 当前包的大小;
    int fileSize;
    // 上一次包的偏移位置;
    int lastStart;
    // 上一次包的大小;
    int lastSize;
    // 文件总长度;
    long fileLength;
    // 文件名称;
    char strFileName[MAX_FILE_NAME + 1];
};

// 存放待传输的文件;
BYTE *m_fileRdBuff = NULL;
// 存放待传输文件的长度;
UINT m_length = 0;
// 记录文件传输过程中的相关信息;
FILEINFO m_fileInfo = {0};
// 存放接受到的文件数据;
BYTE *m_fileWrtBuff = NULL;

BYTE m_sendBuffer[MAX_FILE_PACKET + 1];   // 待传送数据buffer;
UINT m_nLength = 0;                       // 待传送数据长度;
BYTE m_recBuffer[MAX_FILE_PACKET + 1];    // 文件传输接收缓存;
BYTE m_filePacket[MAX_FILE_PACKET + 1];   // 接收文件包缓存;

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
    // 自定义全局变量或函数;
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
    // Instance中的用户数据;
public:
    void InstanceEntry(CMessage *const pMsg);    
    void DaemonInstanceEntry(CMessage *const pcMsg, CApp* pcApp);
    // Instance中的用户函数;
    //void OnMyReceive(CMessage *const pMsg);
public:
    CDemoInstance()
    {
        //m_lastStart = 0;
        //m_lastSize = 0;
        //m_errorIndex = 0;
        // Instance中的用户数据初始化等;
    };
    ~CDemoInstance();
};

// typedef zTemplate<CDemoInstance, 20, CDemoAppData, 20> CDemoApp;
typedef zTemplate<CDemoInstance, MAX_INS> CDemoApp;
CDemoApp g_cDemoApp;

// CPublic 类定义及初始化;
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

// CDemoInstance 类定义及初始化;
void MsgPostFunc(CMessage *const pMsg)
{
    // 获得窗口指针
    CWnd *pBoxOne = AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT4);

    // 获取空闲的Instance;
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

    // 赋值;
    CString cMsgContent(strMsgGet);
    pBoxOne->SetWindowText(cMsgContent);

    // 获取源结点号;
    CPublic::g_uNodeNum = pMsg->srcnode;

    // 释放空间;
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

    m_fileInfo.fileLength = length;       // 在变量m_fileInfo中保存文件长度信息;
    if (m_fileInfo.fileLength != 0)        // 申请文件保存空间;
    {
        if (m_fileInfo.fileLength < 10)
        {
            // 注：此处申请内存过小时会出现DAMAGE after Normal block错误;
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
        // 考虑到文件分包可能出现只传一个包的情况，所以 m_fileInfo.fileSize在初始化的时候要考虑两种情况;
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
    // 定义用于发送文件数据的文件包，fileHead定义为“FFF”;
    strFileMsg.fileHead[0] = 'F';
    strFileMsg.fileHead[1] = 'F';
    strFileMsg.fileHead[2] = 'F';
    strFileMsg.fileHead[3] = '\0';
    strFileMsg.fileStart   = 0;
    strFileMsg.fileSize    = 0;

    OspPrintf(TRUE, FALSE, "enter sendFileInfo\n");

    // 是否基于上次分包的偏移位置;
    if (fStart == CPublic::m_lastStart && fSize == CPublic::m_lastSize)
    {
        // 最后一个包发送完成;
        //if (fStart == 0 && fSize == 0 && !strcmp(fHead, "END"))
        if (!strcmp(fHead, "END"))
        {
            OspPrintf(TRUE, FALSE, "enter END\n");
            CString temp=_T("");
            //GetDlgItemText(IDC_EDIT_CLIENT,temp);
            temp += _T("\r\n");
            temp += _T("文件传输完毕");
            //SetDlgItemText(IDC_EDIT_CLIENT,temp);
            return;
        }
        // 发送第一个包;
        else if (fStart == 0 && fSize == 0 && !strcmp(fHead,"0"))
        {
            // 传输出错标志;
            CPublic::m_errorIndex = 0;
            // 考虑只有一个包的情况;
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
        // 接收到正常确认包的时候;
        else if (!strcmp(fHead,"OK!"))
        {
            OspPrintf(TRUE, FALSE, "next package\n");
            CPublic::m_errorIndex = 0;
            // 当要发送最后一个包的时候;
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
        // 确认包表示接收到的包有错误，则重新发送上一个包;
        else if (!strcmp(fHead,"ERR"))   
        {
            CPublic::m_errorIndex++;
            strFileMsg.fileStart = CPublic::m_lastStart;
            strFileMsg.fileSize = CPublic::m_lastSize;
        }
        // 确认包表示用户取消了文件传输;
        else if (!strcmp(fHead,"CCL"))
        {
            CString temp=_T("");
            //GetDlgItemText(IDC_EDIT_CLIENT,temp);
            temp += _T("\r\n");
            temp += _T("对方取消了文件传输");
            //SetDlgItemText(IDC_EDIT_CLIENT,temp);
            return;
        }
        // 接收到的信息不正常时默认出错，重新发送上一个包;
        else
        {
            CPublic::m_errorIndex++;
            strFileMsg.fileStart = CPublic::m_lastStart;
            strFileMsg.fileSize = CPublic::m_lastSize;
        }
    }
    // 接收到的信息不正常时默认出错，重新发送上一个包;
    else 
    {
        CPublic::m_errorIndex++;
        strFileMsg.fileStart = CPublic::m_lastStart;
        strFileMsg.fileSize = CPublic::m_lastSize;
    }

    // 这里可以进行出错处理;
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
    // 读取一个包的内容;
    //CFile mFile(g_strFilePath, CFile::modeRead | CFile::typeBinary);
    CFile mFile(g_strFilePath, CFile::modeRead);
    mFile.Seek(strFileMsg.fileStart, CFile::begin);
    mFile.Read(strFileMsg.filePacket, strFileMsg.fileSize);

    mFile.Close();
    //memcpy(strFileMsg.filePacket, m_fileRdBuff + strFileMsg.fileStart, strFileMsg.fileSize);
    //strFileMsg.filePacket[strFileMsg.fileSize] = '\0';

    BYTE* filePacket = (BYTE*)&strFileMsg;
    // 将待发送包的内容填充至发送缓存区;
    memcpy(m_sendBuffer, filePacket, strFileMsg.fileSize + 4 + 2*sizeof(int));
    m_nLength = strFileMsg.fileSize + 4 + 2*sizeof(int);

    OspPrintf(TRUE, FALSE, "start post the file message to server\n");
    // 发送包到服务端;
    OspPost(MAKEIID(1, CPublic::g_uInsNum), EVENT_FILE_POST2S, m_sendBuffer, m_nLength, CPublic::g_uNodeNum, MAKEIID(2, 1));
}

// 处理的server端的回复包，决定下一次的文件发送;
void OnClientReceive(CMessage *const pMsg)
{
    // 获取收到的消息内容;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];
    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                

    // 解析包数据;
    char cFileHead[4];
    FILEMESSAGE *strFileMsg;
    strFileMsg = (FILEMESSAGE*)strMsgGet;
    cFileHead[0] = strFileMsg->fileHead[0];
    cFileHead[1] = strFileMsg->fileHead[1];                                                                                                                                                                                                                                                                     
    cFileHead[2] = strFileMsg->fileHead[2];
    cFileHead[3] = '\0';
    int m_fileStart = strFileMsg->fileStart;
    int m_fileSize  = strFileMsg->fileSize;

    // 提取消息发送处理;
    sendFileInfo(m_fileStart, m_fileSize, cFileHead);

    delete [] strMsgGet;
    strMsgGet = NULL;
}

//void ReceiveFilePacket(int nErrorCode)     //从Socket的接收缓冲区中读取数据;
//void SendFilePacketEcho(int nFlag);        //向Client发送文件响应信息;
//bool StoreFilePacket();                    //保存接收到的文件包;
//bool IsFilePacket();                       //判断并保证文件包完整;

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

void SendFilePacketEcho(int nFlag)    //向Client发送确认包，m_fileMsgSend是CMySocket的变量;
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
        //这时全局byte指针m_fileBuffer中存放了全部的文件信息，进行一些保存工作即可;
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
    
    // 发送包到客户端
    OspPost(MAKEIID(2, 1), EVENT_FILE_POST2C, newPacket, len, CPublic::g_uNodeNum, MAKEIID(1, CPublic::g_uInsNum));
#if 0
    if(m_fileInfo.filePacketIndex == m_fileInfo.filePacketNum-1) //定义下一次接收数据的大小;
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

    // 收到的包为最后一个包;
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
    // 收到的包为正常包，且还有后续包;
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
    // 获取收到的消息内容;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];
    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    FILEMESSAGE *strFileMsgGet = (FILEMESSAGE *)strMsgGet;

    // 正常接受包，做接受处理工作，并回复一个确认报告诉client继续发送下一个包;
    if (!strncmp(strFileMsgGet->fileHead, "FFF", 3))
    {
        ReceiveFilePacket(strFileMsgGet);
    }
#if 0
    // 接收到最后一个包，就发送一个确认包告诉Client结束发送;
    else if (!strncmp(strFileMsgGet->fileHead, "END", 3))
    {
        SendFilePacketEcho(FILE_PACKET_END);
    }

    // 用户取消传输，就发送一个确认包告诉Client取消发送;
    else if (!strncmp(strFileMsgGet->fileHead, "CCL", 3))
    {
        SendFilePacketEcho(FILE_PACKET_CANCEL);
    }
#endif
    // 接收client发送的错误信息，中止文件传输;
    else if (!strncmp(strFileMsgGet->fileHead, "ERR", 3))
    {
        // -- TODO:删除文件;
        SendFilePacketEcho(FILE_PACKET_CANCEL);
    }

    strFileMsgGet = NULL;
    delete [] strMsgGet;
    strMsgGet = NULL;


}

void FileMsgInit(CMessage *const pMsg)
{
    // 获取收到的消息内容;
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
    // 获取收到的消息内容;
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
    /*得到当前消息的类型;*/

    switch (wCurEvent)
    {
        // 根据不同的消息类型进行处理;
    case EVENT_REQ:
        // ReqFuction(pMsg);
        NextState(STATE_ACK);				//完成状态跳跃;
        SetTimer(EVENT_TIMEOUT, 1000); 		//进行定时操作;
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
        //回复EVENT_QUIT消息
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

