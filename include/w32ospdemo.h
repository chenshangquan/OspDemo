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

// 包的结构体;
struct FILEMESSAGE
{
    CHAR fileHead[4];       // 标志作用;
    s32  fileStart;         // 包中数据在整个文件中的起始位置标志;
    s32  fileSize;          // 包中数据的长度;
    u16  wCliPostInsNum;    // 包中记录客户端的发送instanceID;
    u16  wSerPostInsNum;    // 包中记录服务端的发送instanceID;
    u16  wIndex;            // 包中记录客户端的发送instance索引号;
    BYTE filePacket[MAX_FILE_PACKET - 4 - 2*sizeof(int)];       // 包中所传输的文件数据;
};

// 文件信息结构体;
typedef struct tagFileInfo
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
}TFileInfo;

// 记录文件传输过程中的相关信息;
//FILEINFO m_fileInfo = {0};
// 存放待传输文件的长度;
DWORD m_length = 0;

BYTE m_sendBuffer[MAX_FILE_PACKET + 1];   // 待传送数据buffer;
UINT m_nLength = 0;                       // 待传送数据长度;

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
    // 自定义全局变量或函数;
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
typedef zTemplate<CDemoInstance, MAX_INS_NO> CDemoApp;
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
u16 CPublic::m_swTmpNum  = 0;

#if 0 // 全局变量初始化;
s32 CPublic::m_lastStart = 0;
s32 CPublic::m_lastSize  = 0;
s32 CPublic::m_PktIndex  = 0;
s32 CPublic::m_errorIndex = 0;
#endif
// CDemoInstance 类定义及初始化;


// 临时全局变量;
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
    // Client端，进度条绘画并显示;
    iValue = 100 * g_uInsNo[wIndex].nPktIndex / m_fileInfo.filePacketNum;
    wsprintf(szProgress, L"Progress(%ld%%)", iValue);
    pProgress->SetValue((int)iValue);
    pProgress->SetText(szProgress);
#endif
    UINT nFilePacketBuff = MAX_FILE_PACKET - 4 - 2*sizeof(s32) - 3*sizeof(u16);

    FILEMESSAGE strFileMsg;
    // 定义用于发送文件数据的文件包，fileHead定义为“FFF”;
    strFileMsg.fileHead[0] = 'F';
    strFileMsg.fileHead[1] = 'F';
    strFileMsg.fileHead[2] = 'F';
    strFileMsg.fileHead[3] = '\0';

    // 包中记录客户端/服务端的发送instance ID;
    strFileMsg.wCliPostInsNum = wCliPostInsNo;
	strFileMsg.wSerPostInsNum = wSerPostInsNo;

	// 包中记录客户端的发送instance 索引值;
	strFileMsg.wIndex = wIndex;

    // 初始化;
    strFileMsg.fileStart   = 0;
    strFileMsg.fileSize    = 0;

    // 是否基于上次分包的偏移位置;
    if (fStart == g_uInsNo[wIndex].nLastStart && fSize == g_uInsNo[wIndex].nLastSize)
    {
        // 最后一个包发送完成;
        //if (fStart == 0 && fSize == 0 && !strcmp(fHead, "END"))
        if (!strcmp(fHead, "END"))
        {
            OspPrintf(TRUE, FALSE, "Enter END.\r\n");
#if 0
            //进度条置顶，完成文件传输;
            pProgress->SetValue(100);
            pProgress->SetText(L"Progress(100%)");

            // 文件校验并显示结果; --TODO
            ::MessageBox(NULL, _T("已完成文件传输！文件检验OK!!"), _T("文件传输结果"), NULL);
#endif
            return;
        }
        // 发送第一个包;
        else if (fStart == 0 && fSize == 0 && !strcmp(fHead,"0"))
        {
            // 传输出错标志、包索引置0;
            g_uInsNo[wIndex].nErrorIndex = 0;
            g_uInsNo[wIndex].nPktIndex   = 0;

            // 考虑只有一个包的情况;
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
        // 接收到server端回复的正常确认包;
        else if (!strcmp(fHead,"OK!"))
        {
            // 传输出错标志置0;
            g_uInsNo[wIndex].nErrorIndex = 0;
            // 包索引进行累加;
            g_uInsNo[wIndex].nPktIndex++;

            // 当要发送最后一个包的时候;
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
        // 确认包表示接收到的包有错误，则重新发送上一个包;
        else if (!strcmp(fHead, "ERR"))   
        {
            OspPrintf(TRUE, FALSE, "Error packet. Index(%d)\r\n",  g_uInsNo[wIndex].nPktIndex);
            g_uInsNo[wIndex].nErrorIndex++;
            strFileMsg.fileStart = g_uInsNo[wIndex].nLastStart;
            strFileMsg.fileSize  = g_uInsNo[wIndex].nLastSize;
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
            Sleep(1);
        }
        // 接收到的信息不正常时默认出错，重新发送上一个包;
        else
        {
            OspPrintf(TRUE, FALSE, "Repeat the previous packet. Index(%d)\r\n",  g_uInsNo[wIndex].nPktIndex);
            g_uInsNo[wIndex].nErrorIndex++;
            strFileMsg.fileStart = g_uInsNo[wIndex].nLastStart;
            strFileMsg.fileSize  = g_uInsNo[wIndex].nLastSize;
        }
    }
    // 接收到的信息不正常时默认出错，重新发送上一个包;
    else 
    {
        OspPrintf(TRUE, FALSE, "Other error, Repeat the previous packet. Index(%d)\r\n",  g_uInsNo[wIndex].nPktIndex);
        g_uInsNo[wIndex].nErrorIndex++;
        strFileMsg.fileStart = g_uInsNo[wIndex].nLastStart;
        strFileMsg.fileSize  = g_uInsNo[wIndex].nLastSize;
    }

    // 这里可以进行出错处理;
    if (g_uInsNo[wIndex].nErrorIndex > 10)       
    {
        strFileMsg.fileHead[0] = 'E';
        strFileMsg.fileHead[1] = 'R';
        strFileMsg.fileHead[2] = 'R';
        strFileMsg.fileHead[3] = '\0';
    }

    ZeroMemory(m_sendBuffer, sizeof(BYTE)*(MAX_FILE_PACKET) + 1);
    ZeroMemory(strFileMsg.filePacket, sizeof(BYTE)*(MAX_FILE_PACKET) - 4 - 2*sizeof(s32) - 3*sizeof(u16));

    // 读取一个包的内容;
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
    // 将待发送包的内容填充至发送缓存区;
    memcpy(m_sendBuffer, filePacket, strFileMsg.fileSize + 4 + 2*sizeof(s32) + 3*sizeof(u16));
    //m_nLength = strFileMsg.fileSize + 4 + 2*sizeof(s32) + 3*sizeof(u16);

    OspPrintf(TRUE, FALSE, "Start to post the file message to server. InsNo:%d\r\n", wCliPostInsNo);

    // 发送包到服务端;
    OspPost(MAKEIID(DEMO_APP_SERVER_NO, wSerPostInsNo), EVENT_FILE_POST2S, m_sendBuffer,
        (strFileMsg.fileSize + 4 + 2*sizeof(s32) + 3*sizeof(u16)), CPublic::g_uNodeNum, MAKEIID(DEMO_APP_CLIENT_NO, wCliPostInsNo));
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
    s32 nFileStart = strFileMsg->fileStart;
    s32 nFileSize  = strFileMsg->fileSize;
	u16 wCliPostInsNo = strFileMsg->wCliPostInsNum;
    u16 wSerPostInsNo = strFileMsg->wSerPostInsNum;
	u16 wIndex     = strFileMsg->wIndex;

    // 暂停发送;
    while(g_PauseFlag != 0)
    {
        Sleep(1000);
    }

    // 提取消息发送处理;
    sendFileInfo(nFileStart, nFileSize, cFileHead, wCliPostInsNo, wSerPostInsNo, wIndex);

    delete [] strMsgGet;
    strMsgGet = NULL;
}

//void ReceiveFilePacket(int nErrorCode)     //从Socket的接收缓冲区中读取数据;
//void SendFilePacketEcho(int nFlag);        //向Client发送文件响应信息;
//bool StoreFilePacket();                    //保存接收到的文件包;
//bool IsFilePacket();                       //判断并保证文件包完整;

// 
u16 FindInsIndex(u16 wSerPostInsNo)
{
    u16 wIndex = 0;

    // 根据server端文件发送的instance ID，找到对应索引，获取文件写入路径信息;
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

    // 根据instanceID，获取instance索引号;
    wIndex = FindInsIndex(wSerPostInsNo);
    if (wIndex == MAX_FILE_POST_INS)
    {
        OspPrintf(TRUE, FALSE, "can't find the instance index.\r\n");
        return FALSE;
    }

    g_uInsNo[wIndex].m_tFileInfo.fileStart = strFileMsgGet->fileStart;
    g_uInsNo[wIndex].m_tFileInfo.fileSize = strFileMsgGet->fileSize;
    

    // 创建文件并写入文件;
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

    // 记录本次偏移位置;
    //m_fileInfo.lastStart = m_fileInfo.fileStart;
    //m_fileInfo.lastSize = m_fileInfo.fileSize;

    g_uInsNo[wIndex].m_tFileInfo.lastStart = g_uInsNo[wIndex].m_tFileInfo.fileStart;
    g_uInsNo[wIndex].m_tFileInfo.lastSize  = g_uInsNo[wIndex].m_tFileInfo.fileSize;

    return TRUE;
}

void SendFilePacketEcho(s32 nFlag, u16 wCliPostInsNo, u16 wSerPostInsNo, u16 wIndex)    //向Client发送确认包，m_fileMsgSend是CMySocket的变量;
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

    // 根据instanceID，获取instance索引号;
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
    
    // 发送包到客户端
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
    // Server端，进度条绘画并显示;
    iValue = 100 * m_fileInfo.filePacketIndex / m_fileInfo.filePacketNum;
    wsprintf(szProgressText, L"Progress(%ld%%)", iValue);
    pProgress->SetValue((int)iValue);
    pProgress->SetText(szProgressText);

    m_fileInfo.filePacketIndex++;
#endif

    // 根据instanceID，获取instance索引号;
    wSerIndex = FindInsIndex(wSerPostInsNo);
    if (wSerIndex == MAX_FILE_POST_INS)
    {
        OspPrintf(TRUE, FALSE, "can't find the instance index.\r\n");
        return;
    }

    g_uInsNo[wSerIndex].m_tFileInfo.filePacketIndex++;
    // 收到的包为最后一个包;
    //if (m_fileInfo.filePacketIndex == m_fileInfo.filePacketNum)
    if (g_uInsNo[wSerIndex].m_tFileInfo.filePacketIndex == g_uInsNo[wSerIndex].m_tFileInfo.filePacketNum)
    {
        if (StoreFilePacket(strFileMsgGet, wSerPostInsNo) == TRUE)
        {
            SendFilePacketEcho(FILE_PACKET_END, wCliPostInsNo, wSerPostInsNo, wIndex);
#if 0
            //进度条置顶，完成文件传输;
            pProgress->SetValue(100);
            pProgress->SetText(L"Progress(100%)");

            // 文件校验并显示结果; --TODO
            ::MessageBox(NULL, _T("已完成文件传输！文件检验OK!!"), _T("文件传输结果"), NULL);
#endif
            return;
        }

        OspPrintf(TRUE, FALSE, "Server: Store file packet fail!\r\n");
        SendFilePacketEcho(FILE_PACKET_ERROR, wCliPostInsNo, wSerPostInsNo, wIndex);
        return;
    }
    // 收到的包为正常包，且还有后续包;
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

// InstanceEntry:event 消息处理函数定义;
//////////////////////////////////////////////

// server端申请消息发送的instance回复处理函数;
void MsgPostInsAllotAck(CMessage *const pMsg)
{
    // 消息内容提取;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];

    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    CPublic::g_uInsNum = atoi(strMsgGet);

}

// server端申请文件发送的instance回复处理函数;
void FilePostInsAllotAck(CMessage *const pMsg)
{
    // 消息内容提取;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];

    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    // 获取Instance ID;
    CPublic::m_swTmpNum = atoi(strMsgGet);

}

// 消息接收处理函数;
void MsgPostFunc(CMessage *const pMsg)
{
    // 消息内容提取;
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
    // 接收client发送的错误信息，中止文件传输;
    else if (!strncmp(strFileMsgGet->fileHead, "ERR", 3))
    {
        // -- TODO:删除文件;
        //SendFilePacketEcho(FILE_PACKET_CANCEL);
    }

    strFileMsgGet = NULL;
    delete [] strMsgGet;
    strMsgGet = NULL;


}

// 文件信息初始化;
void FileInfoInit(u16 wIndex)
{
    // 初始化文件信息内容;
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
    

    // 获取收到的消息内容;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];
    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    
    //memcpy_s(m_fileInfo.strFileName, MAX_FILE_NAME, strMsgGet, MsgLen);
    //m_fileInfo.strFileName[MsgLen] = '\0';

    // 将文件信息写入对应的instance 节点;
    for (wIndex = 0; wIndex < MAX_FILE_POST_INS; wIndex++)
    {
        if (g_uInsNo[wIndex].uSerInsNum != 0 && g_uInsNo[wIndex].nFlag == 0)
        {
            // 文件信息初始化;
            FileInfoInit(wIndex);

            // 获取文件名信息;
            memcpy_s(g_uInsNo[wIndex].m_tFileInfo.strFileName, MAX_FILE_NAME, strMsgGet, MsgLen);
            g_uInsNo[wIndex].m_tFileInfo.strFileName[MsgLen] = '\0';

            // 文件存储路径处理;
            USES_CONVERSION;
            ZeroMemory(g_strFilePath, MAX_PATH);
            lstrcat(g_strFilePath, g_strFolderPath);
            lstrcat(g_strFilePath, L"\\");
            lstrcat(g_strFilePath, A2W(g_uInsNo[wIndex].m_tFileInfo.strFileName));
            //g_uInsNo[wIndex].nFlag = 1;     //使用置0, 默认时;

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

    // 获取收到的消息内容;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];
    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    // 将文件信息写入对应的instance 节点;
    for (wIndex = 0; wIndex < MAX_FILE_POST_INS; wIndex++)
    {
        if (g_uInsNo[wIndex].uSerInsNum != 0 && g_uInsNo[wIndex].nFlag == 0)
        {
            // 获取文件长度信息;
            g_uInsNo[wIndex].m_tFileInfo.fileLength = atoi(strMsgGet);

            // 计算文件分包数目;
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
    /*得到当前消息的类型;*/
    u16 wCurEvent = pMsg->event;

    switch (wCurEvent)
    {
        // 根据不同的消息类型进行处理;
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


// DaemonInstanceEntry:event 消息处理函数定义;
//////////////////////////////////////////////

// 获取空闲的instanceID;
u16 GetIdleInsID(CApp* pcApp)
{
    u16 wIndex = 0;
    CInstance *pCInst = NULL;

    // 遍历所有instance，寻找到一个空闲的instance并返回;
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

    // 当前没有空闲的instance;
    if (wIndex > MAX_INS_NO)
    {
        OspPrintf(TRUE, FALSE, "Current have none idle instance.\r\n");
        return 0;
    }

    return wIndex;
}

// 服务端消息发送instance分配;
void ServerMsgPostInsAllot(CMessage *const pcMsg, CApp* pcApp)
{
    s8 achInsNo[4];
    CPublic::g_uInsNum = GetIdleInsID(pcApp);
    OspPrintf(TRUE, FALSE, "Server:Get a idle instance, ID: %d.\r\n", CPublic::g_uInsNum);
    CPublic::g_uNodeNum = pcMsg->srcnode;

    ZeroMemory(achInsNo, 4);
    sprintf(achInsNo, "%d", CPublic::g_uInsNum);

    // 回复服务端申请到的空闲instance，并发送给客户端;
    s32 nPostRet = OspPost(MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), EVENT_MSG_POST_INS_ALLOT_ACK, achInsNo, strlen(achInsNo), CPublic::g_uNodeNum);
    
    return;
}

// 服务端文件发送instance分配;
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

    // 回复服务端申请到的空闲instance，并发送给客户端;
    s32 nPostRet = OspPost(MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO), EVENT_FILE_POST_INS_ALLOT_ACK, achInsNo, strlen(achInsNo), CPublic::g_uNodeNum);

    return;
}

// 客户端文件发送instance分配;
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
    /*得到当前消息的类型;*/
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

// 自定义其他封装函数;
//////////////////////////////////////////////

// 判断IP字符串的合法性;
BOOL IsIpFormatRight(LPCTSTR pIpAddr)
{
    s32 dwCount = 0;
    s32 i = 0;
    s32 dwA, dwB, dwC, dwD;

    // 检查是否只包含点和数字;
    for(i = 0; pIpAddr[i] != '\0'; i++)
    {
        if(!isdigit(pIpAddr[i]) && pIpAddr[i] != '.')
            return FALSE;
    }

    // 检查形式是否为X.X.X.X;
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

    // 检查区间的合法性;
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