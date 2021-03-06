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
    EVENT_CLIENT_FILE_POST_INS_ALLOT,
	EVENT_SERVER_FILE_POST_INS_RELEASE,
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

// 包的结构体;
struct FILEMESSAGE
{
    s8  fileHead[4];       // 标志作用;
    u32 fileStart;         // 包中数据在整个文件中的起始位置标志;
    u32 fileSize;          // 包中数据的长度;
    u16 wCliPostInsNum;    // 包中记录客户端的发送instanceID;
    u16 wSerPostInsNum;    // 包中记录服务端的发送instanceID;
    u16 wIndex;            // 包中记录客户端的发送instance索引号;
    u8  filePacket[MAX_FILE_PACKET - 4 - 2*sizeof(int)];       // 包中所传输的文件数据;
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
    u32 fileLength;
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

// CDemoListContainerElementUI类函数声明;
class CDemoListContainerElementUI : public CListContainerElementUI
{
public:
	void SetPos(RECT rc);
	CListHeaderUI *m_pHeader;
};

// CDemoListContainerElementUI类函数定义;
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

// CDemoInstance类函数声明;
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

// 临时全局变量;
typedef struct tagInStatus
{
    u16 uInsNum;
    s32 nUsedFlag;
    s32 nLastStart;
    s32 nLastSize;
    s32 nPktIndex;
    s64 dnProgValve;
    s32 nErrorIndex;
	TCHAR strFilePath[MAX_PATH];
	s32 m_nPuase;			// 文件停止发送标志;
	s32 m_nCancel;			// 文件取消发送标志;
    TFileInfo m_tFileInfo;
}TInStatus;
vector<TInStatus> g_tInsNo;

void sendFileInfo(s32 fStart,s32 fSize,char *fHead, u16 wCliPostInsNo, u16 wSerPostInsNo, u16 wIndex)
{
    OspPrintf(TRUE, FALSE, "sendFileInfo.\r\n");
    u16 wListIndex = 0;
    u16 wListItemNum = 0;
	s8 achProgress[MAX_FILE_NAME] = {0};
	s8 achBtnName[MAX_STR_LEN]    = {0};
	s8 achPrgName[MAX_STR_LEN]    = {0};
    s8 achTmpName[MAX_FILE_NAME]  = {0};
    CDuiString strListTag;
	CProgressUI* pcPrg = NULL;

    // 文件传输进度数值;
    g_tInsNo[wIndex].dnProgValve = 100 * g_tInsNo[wIndex].nPktIndex / g_tInsNo[wIndex].m_tFileInfo.filePacketNum;

	// 获取列表窗口指针;
	CListUI* pcLoadList = pFrame->m_pList;
	
	u32 nFilePacketBuff = MAX_FILE_PACKET - 4 - 2*sizeof(s32) - 3*sizeof(u16);

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
    if (fStart == g_tInsNo[wIndex].nLastStart && fSize == g_tInsNo[wIndex].nLastSize)
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
			// 获取控件进度条指针;
#if 0   // 取消定位;
			pcPrg = static_cast<CProgressUI*>(pFrame->m_pm.FindControl(CA2W(achPrgName)));

			// 进度条赋值;
			ZeroMemory(achProgress, MAX_FILE_NAME);
			sprintf(achProgress, "%s(%d%%)", g_tInsNo[wIndex].m_tFileInfo.strFileName, 100);
			pcPrg->SetValue(100);
			pcPrg->SetText(CA2W(achProgress));

			pcLoadList->RemoveAt(wIndex);
#endif
            return;
        }
        // 发送第一个包;
        else if (fStart == 0 && fSize == 0 && !strcmp(fHead,"0"))
        {
            // 传输出错标志、包索引置0;
            g_tInsNo[wIndex].nErrorIndex = 0;
            g_tInsNo[wIndex].nPktIndex   = 0;

			// 绘图;
			pcLoadList->RemoveAll();    // 清除重绘;
			for (wListIndex = 0; wListIndex < g_tInsNo.size(); wListIndex++)
			{
				CDemoListContainerElementUI* pcListContainer = new CDemoListContainerElementUI;
				pcListContainer->m_pHeader = pFrame->m_pListHeader;
				CProgressUI* pcProgress = new CProgressUI;
				CHorizontalLayoutUI* pcHorizontalLayout = new CHorizontalLayoutUI;
				CButtonUI* pcButtonStp = new CButtonUI;
				CButtonUI* pcButtonCcl = new CButtonUI;

				pcListContainer->ApplyAttributeList(_T("height=\"25\" align=\"right\""));

				// 进度条控件添加;
                ZeroMemory(achPrgName, MAX_STR_LEN);
                sprintf(achPrgName, "DemoProgress%u", wListIndex);
				pcProgress->ApplyAttributeList(_T("width=\"200\" height=\"20\" foreimage=\"OspDemoSkins\\progress_fore.png\"\
												  min=\"0\" max=\"100\" hor=\"true\" align=\"center\""));
				pcProgress->SetName(CA2W(achPrgName));
				ZeroMemory(achProgress, MAX_FILE_NAME);
				sprintf(achProgress, "%s(%ld%%)", g_tInsNo[wListIndex].m_tFileInfo.strFileName, g_tInsNo[wListIndex].dnProgValve);
				pcProgress->SetValue((int)g_tInsNo[wListIndex].dnProgValve);
				pcProgress->SetText(CA2W(achProgress));
				pcListContainer->Add(pcProgress);

				//pcHorizontalLayout->ApplyAttributeList(_T(""));

				// 暂停按键控件添加;
				ZeroMemory(achBtnName, MAX_STR_LEN);
				sprintf(achBtnName, "FileStpButton%u", wListIndex);
				pcButtonStp->ApplyAttributeList(_T("text=\"S\" width=\"25\" height=\"20\""));
				pcButtonStp->SetName(CA2W(achBtnName));
				pcHorizontalLayout->Add(pcButtonStp);

				// 取消按键控件添加;
				pcButtonCcl->ApplyAttributeList(_T("name=\"FileCclButton\" text=\"C\" width=\"25\" height=\"20\""));
				pcHorizontalLayout->Add(pcButtonCcl);

				pcListContainer->Add(pcHorizontalLayout);
				pcLoadList->Add(pcListContainer);

				//delete pcListContainer;
				//delete pcProgress;
				//delete pcHorizontalLayout;
				//delete pcButtonStp;
				//delete pcButtonCcl;
			}

            // 考虑只有一个包的情况;
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
        // 接收到server端回复的正常确认包;
        else if (!strcmp(fHead,"OK!"))
        {
            ZeroMemory(achPrgName, MAX_STR_LEN);
            sprintf(achPrgName, "DemoProgress%u", wIndex);  //need to replace;
            pcPrg = static_cast<CProgressUI*>(pFrame->m_pm.FindControl(CA2W(achPrgName)));

			// 进度条赋值;
			ZeroMemory(achProgress, MAX_FILE_NAME);
			sprintf(achProgress, "%s(%ld%%)", g_tInsNo[wIndex].m_tFileInfo.strFileName, g_tInsNo[wIndex].dnProgValve);
			pcPrg->SetValue((s32)g_tInsNo[wIndex].dnProgValve);
			pcPrg->SetText(CA2W(achProgress));

            // 传输出错标志置0;
            g_tInsNo[wIndex].nErrorIndex = 0;
            // 包索引进行累加;
            g_tInsNo[wIndex].nPktIndex++;

            // 当要发送最后一个包的时候;
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
        // 确认包表示接收到的包有错误，则重新发送上一个包;
        else if (!strcmp(fHead, "ERR"))   
        {
            OspPrintf(TRUE, FALSE, "Error packet. Index(%d)\r\n",  g_tInsNo[wIndex].nPktIndex);
            g_tInsNo[wIndex].nErrorIndex++;
            strFileMsg.fileStart = g_tInsNo[wIndex].nLastStart;
            strFileMsg.fileSize  = g_tInsNo[wIndex].nLastSize;
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
            OspPrintf(TRUE, FALSE, "Repeat the previous packet. Index(%d)\r\n",  g_tInsNo[wIndex].nPktIndex);
            g_tInsNo[wIndex].nErrorIndex++;
            strFileMsg.fileStart = g_tInsNo[wIndex].nLastStart;
            strFileMsg.fileSize  = g_tInsNo[wIndex].nLastSize;
        }
    }
    // 接收到的信息不正常时默认出错，重新发送上一个包;
    else 
    {
        OspPrintf(TRUE, FALSE, "Other error, Repeat the previous packet. Index(%d)\r\n",  g_tInsNo[wIndex].nPktIndex);
        g_tInsNo[wIndex].nErrorIndex++;
        strFileMsg.fileStart = g_tInsNo[wIndex].nLastStart;
        strFileMsg.fileSize  = g_tInsNo[wIndex].nLastSize;
    }

    // 这里可以进行出错处理;
    if (g_tInsNo[wIndex].nErrorIndex > 10)       
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
    OspPrintf(TRUE, FALSE, "Client: Receive the response package\r\n");
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
    while(g_tInsNo[wIndex].m_nPuase != 0)
    {
        Sleep(1000);
    }

    OspPrintf(TRUE, FALSE, "Client: Next send, wCliPostInsNo: %d, wSerPostInsNo: %d, wIndex: %d\r\n", wCliPostInsNo, wSerPostInsNo, wIndex);
    // 提取消息发送处理;
    sendFileInfo(nFileStart, nFileSize, cFileHead, wCliPostInsNo, wSerPostInsNo, wIndex);

    delete [] strMsgGet;
    strMsgGet = NULL;
}

// 根据intance ID，找到对应的全局变量索引;
u16 FindInsIndex(u16 wSerPostInsNo)
{
    u16 wIndex = 0;

    // 根据server端文件发送的instance ID，找到对应索引，获取文件写入路径信息;
	for (wIndex = 0; wIndex < g_tInsNo.size(); wIndex++)
	{
		if (g_tInsNo[wIndex].uInsNum == wSerPostInsNo)
        {
            return wIndex;
        }
    }

    return MAX_FILE_POST_INS;
}

void InsRelease(u16 wCliPostInsNo, u16 wSerPostInsNo)
{
	s8 achCliPostInsNoStr[4] = {0};
	s8 achSerPostInsNoStr[4] = {0};

	// 释放Client端文件发送的instance;
	sprintf(achCliPostInsNoStr, "%d", wCliPostInsNo);
	OspPost(MAKEIID(DEMO_APP_CLIENT_NO, CInstance::DAEMON), EVENT_CLIENT_FILE_POST_INS_RELEASE,
		achCliPostInsNoStr, strlen(achCliPostInsNoStr),	CPublic::g_uNodeNum, MAKEIID(DEMO_APP_SERVER_NO, CPublic::g_uInsNum), 0, DEMO_POST_TIMEOUT);

	// 释放Server端文件发送的instance;
	sprintf(achSerPostInsNoStr, "%d", wCliPostInsNo);
	OspPost(MAKEIID(DEMO_APP_SERVER_NO, CInstance::DAEMON), EVENT_SERVER_FILE_POST_INS_RELEASE,
		achSerPostInsNoStr, strlen(achSerPostInsNoStr), 0, MAKEIID(DEMO_APP_SERVER_NO, CPublic::g_uInsNum), 0, DEMO_POST_TIMEOUT);
}

// 将接受到的包内容写入文件;
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

    g_tInsNo[wIndex].m_tFileInfo.fileStart = strFileMsgGet->fileStart;
    g_tInsNo[wIndex].m_tFileInfo.fileSize = strFileMsgGet->fileSize;
    
    // 创建文件并写入文件;
    HANDLE mFileW = CreateFile(g_tInsNo[wIndex].strFilePath, GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
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
    g_tInsNo[wIndex].m_tFileInfo.lastStart = g_tInsNo[wIndex].m_tFileInfo.fileStart;
    g_tInsNo[wIndex].m_tFileInfo.lastSize  = g_tInsNo[wIndex].m_tFileInfo.fileSize;

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
    if (wSerIndex == MAX_FILE_POST_INS)
    {
        OspPrintf(TRUE, FALSE, "can't find the instance index.\r\n");
        return;
    }

    strFileMsgAck.fileStart      = g_tInsNo[wSerIndex].m_tFileInfo.lastStart;
    strFileMsgAck.fileSize       = g_tInsNo[wSerIndex].m_tFileInfo.lastSize;
    strFileMsgAck.wCliPostInsNum = wCliPostInsNo;
	strFileMsgAck.wSerPostInsNum = wSerPostInsNo;
	strFileMsgAck.wIndex = wIndex;

    ZeroMemory(strFileMsgAck.filePacket, MAX_FILE_PACKET - 4 - 2*sizeof(s32) - 3*sizeof(u16));
    BYTE* newPacket = (BYTE*)&strFileMsgAck;
    u16 uLen = 4 + 2*sizeof(s32) + 3*sizeof(u16);
    
    OspPrintf(TRUE, FALSE, "Server: Response package.\r\n");
    // 发送包到客户端
    OspPost(MAKEIID(DEMO_APP_CLIENT_NO, wCliPostInsNo), EVENT_FILE_POST2C,
        newPacket, uLen, CPublic::g_uNodeNum, MAKEIID(DEMO_APP_SERVER_NO, wSerPostInsNo));
}

// 接受包处理函数;
void ReceiveFilePacket(FILEMESSAGE *strFileMsgGet)
{
    LONG64 iValue = 0;
    TCHAR szProgressText[16] = {0};
    u16 wSerIndex = 0;

	u16 wCliPostInsNo = strFileMsgGet->wCliPostInsNum;
    u16 wSerPostInsNo = strFileMsgGet->wSerPostInsNum;
	u16 wIndex = strFileMsgGet->wIndex;

    //CProgressUI* pProgress =  pFrame->m_pProgress;
#if 0
    // Server端，进度条绘画并显示;
    iValue = 100 * m_fileInfo.filePacketIndex / m_fileInfo.filePacketNum;
    wsprintf(szProgressText, L"Progress(%ld%%)", iValue);
    pProgress->SetValue((int)iValue);
    pProgress->SetText(szProgressText);

    //m_fileInfo.filePacketIndex++;
#endif

    // 根据instanceID，获取instance索引号;
    wSerIndex = FindInsIndex(wSerPostInsNo);
    OspPrintf(TRUE, FALSE, "Server: FindInsIndex: %d.\r\n", wSerIndex);

    if (wSerIndex == MAX_FILE_POST_INS)
    {
        OspPrintf(TRUE, FALSE, "can't find the instance index.\r\n");
        return;
    }

    g_tInsNo[wSerIndex].m_tFileInfo.filePacketIndex++;
    // 收到的包为最后一个包;
    //if (m_fileInfo.filePacketIndex == m_fileInfo.filePacketNum)
    if (g_tInsNo[wSerIndex].m_tFileInfo.filePacketIndex == g_tInsNo[wSerIndex].m_tFileInfo.filePacketNum)
    {
        if (StoreFilePacket(strFileMsgGet, wSerPostInsNo) == TRUE)
        {
            OspPrintf(TRUE, FALSE, "Server: End package.\r\n");
            SendFilePacketEcho(FILE_PACKET_END, wCliPostInsNo, wSerPostInsNo, wIndex);
#if 0
            //进度条置顶，完成文件传输;
            pProgress->SetValue(100);
            pProgress->SetText(L"Progress(100%)");

            // 文件校验并显示结果; --TODO
            ::MessageBox(NULL, _T("已完成文件传输！文件检验OK!!"), _T("文件传输结果"), NULL);
#endif
			InsRelease(wCliPostInsNo, wSerPostInsNo);
            return;
        }

        OspPrintf(TRUE, FALSE, "Server: Store file packet fail!\r\n");
        SendFilePacketEcho(FILE_PACKET_ERROR, wCliPostInsNo, wSerPostInsNo, wIndex);
        return;
    }
    // 收到的包为正常包，且还有后续包;
    //else if (m_fileInfo.filePacketIndex < m_fileInfo.filePacketNum)
    else if (g_tInsNo[wSerIndex].m_tFileInfo.filePacketIndex < g_tInsNo[wSerIndex].m_tFileInfo.filePacketNum)
    {
        if (StoreFilePacket(strFileMsgGet, wSerPostInsNo) == TRUE)
        {
            OspPrintf(TRUE, FALSE, "Server: Next package.\r\n");
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

// CDemoInstance 类定义及初始化;
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

// Server端申请文件发送的instance回复处理函数;
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
        if (g_tInsNo[wIndex].uInsNum != 0 && g_tInsNo[wIndex].nUsedFlag == 0)
        {
            // 文件信息初始化;
            FileInfoInit(wIndex);

            // 获取文件名信息;
            memcpy_s(g_tInsNo[wIndex].m_tFileInfo.strFileName, MAX_FILE_NAME, strMsgGet, MsgLen);
            g_tInsNo[wIndex].m_tFileInfo.strFileName[MsgLen] = '\0';

            // 文件存储路径处理;
            USES_CONVERSION;
            ZeroMemory(g_strFilePath, MAX_PATH);
            lstrcat(g_strFilePath, g_strFolderPath);
            lstrcat(g_strFilePath, L"\\");
            lstrcat(g_strFilePath, A2W(g_tInsNo[wIndex].m_tFileInfo.strFileName));
            //g_tInsNo[wIndex].nFlag = 1;     //使用时置1;

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

    // 获取收到的消息内容;
    u16 MsgLen = pMsg->length;
    char *strMsgGet = new char[MsgLen + 1];
    ZeroMemory(strMsgGet, MsgLen + 1);
    memcpy_s(strMsgGet, MsgLen, pMsg->content, MsgLen);

    // 将文件信息写入对应的instance 节点;
    for (wIndex = 0; wIndex < g_tInsNo.size(); wIndex++)
    {
        if (g_tInsNo[wIndex].uInsNum != 0 && g_tInsNo[wIndex].nUsedFlag == 0)
        {
			g_tInsNo[wIndex].nUsedFlag = 1;     //使用时置1;
            // 获取文件长度信息;
            g_tInsNo[wIndex].m_tFileInfo.fileLength = atoi(strMsgGet);

            // 计算文件分包数目;
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
    /*得到当前消息的类型;*/
    u16 wCurEvent = pMsg->event;

    // 根据不同的消息类型进行处理;
    switch (wCurEvent)
    {
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
    TInStatus tInStatus = {0};
    s8 achInsNo[MAX_INS_STR_NO] = {0};

    // 判断用于文件传输的instance是否达到限值;
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

    ZeroMemory(achInsNo, MAX_INS_STR_NO);
    sprintf(achInsNo, "%d", tInStatus.uInsNum);

    // 回复服务端申请到的空闲instance，并发送给客户端;
    s32 nPostRet = OspPost(MAKEIID(DEMO_APP_CLIENT_NO, INS_MSG_POST_NO),
        EVENT_FILE_POST_INS_ALLOT_ACK, achInsNo, strlen(achInsNo), CPublic::g_uNodeNum);

    return;
}

// 客户端文件发送instance分配;
void ClientFilePostInsAllot(CMessage *const pcMsg, CApp* pcApp)
{
    s32 nIndex = 0;
    TInStatus tInStatus = {0};

    // 判断用于文件传输的instance是否达到限值;
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

// 服务端文件发送instance释放;
void SerFilePostInsRelease(CMessage *const pcMsg, CApp* pcApp)
{
	u16 wInsid = 0;
	u16 wIndex = 0;
	CInstance *pCInst = NULL;

	// 获取收到的消息内容;
	u16 wMsgLen = pcMsg->length;
	char *pchMsgGet = new char[wMsgLen + 1];
	ZeroMemory(pchMsgGet, wMsgLen + 1);
	memcpy_s(pchMsgGet, wMsgLen, pcMsg->content, wMsgLen);

	wInsid = atoi(pchMsgGet);

	// 释放全局变量;
    vector<TInStatus>::iterator itIndex;
    for (itIndex = g_tInsNo.begin(); itIndex != g_tInsNo.end(); itIndex++)
	{
		if (itIndex->uInsNum == wInsid)
		{
			g_tInsNo.erase(itIndex);
            break;
		}
	}

	// 释放instance资源;
	pCInst = pcApp->GetInstance(wInsid);
	pCInst->m_curState = IDLE_STATE;
	pCInst->m_lastState = STATE_WORK;


}

// 客户端文件发送instance释放;
void CliFilePostInsRelease(CMessage *const pcMsg, CApp* pcApp)
{
	u16 wInsid = 0;
	u16 wIndex = 0;
	CInstance *pCInst = NULL;

	// 获取收到的消息内容;
	u16 wMsgLen = pcMsg->length;
	char *pchMsgGet = new char[wMsgLen + 1];
	ZeroMemory(pchMsgGet, wMsgLen + 1);
	memcpy_s(pchMsgGet, wMsgLen, pcMsg->content, wMsgLen);

	wInsid = atoi(pchMsgGet);

    // 释放全局变量;
    vector<TInStatus>::iterator itIndex;
    for (itIndex = g_tInsNo.begin(); itIndex != g_tInsNo.end(); itIndex++)
    {
        if (itIndex->uInsNum == wInsid)
        {
            g_tInsNo.erase(itIndex);
            break;
        }
    }

	// 释放instance资源;
	pCInst = pcApp->GetInstance(wInsid);
	pCInst->m_curState = IDLE_STATE;
	pCInst->m_lastState = STATE_WORK;


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
	case EVENT_SERVER_FILE_POST_INS_RELEASE:
		SerFilePostInsRelease(pcMsg, pcApp);
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