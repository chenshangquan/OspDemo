#pragma once

#include "../osplib/include/kdvtype.h"
#include "../osplib/include/osp.h"

CFrameWindowWnd* pFrame = NULL;
//#include "atlstr.h"

#define MAX_POST_MSG_LEN 255
#define MAX_POST_FILE_PACK_LEN 1024
#define MAX_INS 10
#define MAX_FILE_PACKET 10240 * 2
#define MAX_FILE_NAME 128
#define MAX_THREADNO 3

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
    DWORD fileLength;
    // 文件名称;
    char strFileName[MAX_FILE_NAME + 1];
};

// 记录文件传输过程中的相关信息;
FILEINFO m_fileInfo = {0};
// 存放待传输文件的长度;
DWORD m_length = 0;

BYTE m_sendBuffer[MAX_FILE_PACKET + 1];   // 待传送数据buffer;
UINT m_nLength = 0;                       // 待传送数据长度;

// client read;
TCHAR g_strFilePath[MAX_PATH] = _T("");
TCHAR g_strFileName[MAX_FILE_NAME] = _T("");
TCHAR g_strFolderPath[MAX_PATH] = _T("F:\\2");
char strFileLen[16] = {0};

typedef void (*func)(s32, s32, const char*);
HANDLE g_hTask;
s32 g_PauseFlag;

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

#if 1 // 全局变量初始化;
s32 CPublic::m_lastStart = 0;
s32 CPublic::m_lastSize  = 0;
s32 CPublic::m_PktIndex  = 0;
s32 CPublic::m_errorIndex = 0;
#endif
// CDemoInstance 类定义及初始化;


void MsgPostFunc(CMessage *const pMsg)
{
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
    USES_CONVERSION;
    wchar_t *lpMsg = A2W(strMsgGet);
    CEditUI* pEditRecv =  pFrame->m_pEditRecv;
    pEditRecv->SetText(lpMsg);

    // 获取源结点号;
    CPublic::g_uNodeNum = pMsg->srcnode;

    // 释放空间;
    delete [] strMsgGet;
    strMsgGet = NULL;
}

void sendFileInfo(s32 fStart,s32 fSize,char *fHead)
{
    LONG64 iValue = 0;
    TCHAR szProgress[16] = {0};
    CProgressUI* pProgress =  pFrame->m_pProgress;

    // Client端，进度条绘画并显示;
    iValue = 100 * CPublic::m_PktIndex / m_fileInfo.filePacketNum;
    wsprintf(szProgress, L"Progress(%ld%%)", iValue);
    pProgress->SetValue((int)iValue);
    pProgress->SetText(szProgress);

    UINT nFilePacketBuff = MAX_FILE_PACKET - 4 - 2*sizeof(int);
    FILEMESSAGE strFileMsg;
    // 定义用于发送文件数据的文件包，fileHead定义为“FFF”;
    strFileMsg.fileHead[0] = 'F';
    strFileMsg.fileHead[1] = 'F';
    strFileMsg.fileHead[2] = 'F';
    strFileMsg.fileHead[3] = '\0';
    strFileMsg.fileStart   = 0;
    strFileMsg.fileSize    = 0;

    // 是否基于上次分包的偏移位置;
    if (fStart == CPublic::m_lastStart && fSize == CPublic::m_lastSize)
    {
        // 最后一个包发送完成;
        //if (fStart == 0 && fSize == 0 && !strcmp(fHead, "END"))
        if (!strcmp(fHead, "END"))
        {
            OspPrintf(TRUE, FALSE, "Enter END.\r\n");
            CloseHandle(g_hTask);
            //进度条置顶，完成文件传输;
            pProgress->SetValue(100);
            pProgress->SetText(L"Progress(100%)");

            // 文件校验并显示结果; --TODO
            ::MessageBox(NULL, _T("已完成文件传输！文件检验OK!!"), _T("文件传输结果"), NULL);
            return;
        }
        // 发送第一个包;
        else if (fStart == 0 && fSize == 0 && !strcmp(fHead,"0"))
        {
            // 传输出错标志、包索引置0;
            CPublic::m_errorIndex = 0;
            CPublic::m_PktIndex  = 0;

            // 考虑只有一个包的情况;
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
        // 接收到server端回复的正常确认包;
        else if (!strcmp(fHead,"OK!"))
        {
            // 传输出错标志置0;
            CPublic::m_errorIndex = 0;
            // 包索引进行累加;
            CPublic::m_PktIndex++;

            // 当要发送最后一个包的时候;
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
        // 确认包表示接收到的包有错误，则重新发送上一个包;
        else if (!strcmp(fHead, "ERR"))   
        {
            OspPrintf(TRUE, FALSE, "Error packet. Index(%d)\r\n", CPublic::m_PktIndex);
            CPublic::m_errorIndex++;
            strFileMsg.fileStart = CPublic::m_lastStart;
            strFileMsg.fileSize = CPublic::m_lastSize;
        }
        // 确认包表示用户取消了文件传输;
        else if (!strcmp(fHead, "CCL"))
        {
            string temp=("");
            //GetDlgItemText(IDC_EDIT_CLIENT,temp);
            //temp += _T("\r\n");
            //temp += _T("对方取消了文件传输");
            //SetDlgItemText(IDC_EDIT_CLIENT,temp);
            return;
        }
        // 暂停包表示用户停止文件传送;
        else if (!strcmp(fHead, "STP"))
        {
            OspPrintf(TRUE, FALSE, "stop the packet send.\r\n\r\n\r\n");
            if (OspTaskSuspend(g_hTask) == -1)
            {
                OspPrintf(TRUE, FALSE, "OspTaskSuspend Failed.\r\n\r\n\r\n");
            }
            Sleep(1);
        }
        // 接收到的信息不正常时默认出错，重新发送上一个包;
        else
        {
            OspPrintf(TRUE, FALSE, "Repeat the previous packet. Index(%d)\r\n", CPublic::m_PktIndex);
            CPublic::m_errorIndex++;
            strFileMsg.fileStart = CPublic::m_lastStart;
            strFileMsg.fileSize = CPublic::m_lastSize;
        }
    }
    // 接收到的信息不正常时默认出错，重新发送上一个包;
    else 
    {
        OspPrintf(TRUE, FALSE, "Other error, Repeat the previous packet. Index(%d)\r\n", CPublic::m_PktIndex);
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

    // 读取一个包的内容;
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
    // 将待发送包的内容填充至发送缓存区;
    memcpy(m_sendBuffer, filePacket, strFileMsg.fileSize + 4 + 2*sizeof(int));
    m_nLength = strFileMsg.fileSize + 4 + 2*sizeof(int);

    OspPrintf(TRUE, FALSE, "Start to post the file message to server.\r\n");
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

    // 暂停发送;
    while(g_PauseFlag != 0)
    {
        Sleep(1000);
    }

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
    DWORD nByte;

    OspPrintf(TRUE, FALSE, "Server: Start to store the packet\r\n");
    m_fileInfo.fileStart = strFileMsgGet->fileStart;
    m_fileInfo.fileSize = strFileMsgGet->fileSize;

    // 创建文件并写入文件;
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

    // 记录本次偏移位置;
    m_fileInfo.lastStart = m_fileInfo.fileStart;
    m_fileInfo.lastSize = m_fileInfo.fileSize;

    return TRUE;
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
    u16 uLen = 4 + 2*sizeof(int);
    
    // 发送包到客户端
    OspPost(MAKEIID(2, 1), EVENT_FILE_POST2C, newPacket, uLen, CPublic::g_uNodeNum, MAKEIID(1, CPublic::g_uInsNum));
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
    LONG64 iValue = 0;
    TCHAR szProgressText[16] = {0};
    CProgressUI* pProgress =  pFrame->m_pProgress;

    // Server端，进度条绘画并显示;
    iValue = 100 * m_fileInfo.filePacketIndex / m_fileInfo.filePacketNum;
    wsprintf(szProgressText, L"Progress(%ld%%)", iValue);
    pProgress->SetValue((int)iValue);
    pProgress->SetText(szProgressText);

    m_fileInfo.filePacketIndex++;
    //ZeroMemory(m_recBuffer, sizeof(BYTE)*(MAX_FILE_PACKET + 1));

    // 收到的包为最后一个包;
    if (m_fileInfo.filePacketIndex == m_fileInfo.filePacketNum)
    {
        if (StoreFilePacket(strFileMsgGet) == TRUE)
        {
            SendFilePacketEcho(FILE_PACKET_END);

            //进度条置顶，完成文件传输;
            pProgress->SetValue(100);
            pProgress->SetText(L"Progress(100%)");

            // 文件校验并显示结果; --TODO
            ::MessageBox(NULL, _T("已完成文件传输！文件检验OK!!"), _T("文件传输结果"), NULL);
            return;
        }

        OspPrintf(TRUE, FALSE, "Server: Store file packet fail!\r\n");
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

// 文件信息初始化;
void FileInfoInit()
{
    // 初始化文件信息内容;
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
    // 文件信息初始化;
    FileInfoInit();

    // 获取收到的消息内容;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];
    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    // 获取文件名信息;
    memcpy_s(m_fileInfo.strFileName, MAX_FILE_NAME, strMsgGet, MsgLen);
    m_fileInfo.strFileName[MsgLen] = '\0';

    // 文件存储路径处理;
    USES_CONVERSION;
    ZeroMemory(g_strFilePath, MAX_PATH);
    lstrcat(g_strFilePath, g_strFolderPath);
    lstrcat(g_strFilePath, L"\\");
    lstrcat(g_strFilePath, A2W(m_fileInfo.strFileName));
}

void FileLenInit(CMessage *const pMsg)
{
    // 获取收到的消息内容;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];
    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    // 获取文件长度信息;
    m_fileInfo.fileLength = atoi(strMsgGet);
    m_length = m_fileInfo.fileLength;

    // 计算文件分包数目;
    m_fileInfo.filePacketNum = m_fileInfo.fileLength/(MAX_FILE_PACKET - 4 - 2*sizeof(int)) + 1;
    OspPrintf(TRUE, FALSE, "server received file attribute, name is : %s, length : %d, packageNum : %d\n", m_fileInfo.strFileName, m_fileInfo.fileLength, m_fileInfo.filePacketNum);
   
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
        OspPrintf(TRUE, FALSE, "received a message.\n");
        MsgPostFunc(pMsg);
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

