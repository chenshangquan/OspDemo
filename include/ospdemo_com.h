#pragma once

#include "../osplib/include/kdvtype.h"
#include "../osplib/include/osp.h"

#define MAX_POST_MSG_LEN 255
#define MAX_POST_FILE_PACK_LEN 1024
#define MAX_INS_STR_NO 16
#define MAX_STR_LEN 16
#define MAX_FILE_PACKET 10240 * 2
#define MAX_FILE_NAME 128
#define MAX_THREADNO 3
#define MAX_FILE_POST_INS 5

#define INS_MSG_POST_NO 1

#define DEMO_APP_SERVER_NO 1
#define DEMO_APP_CLIENT_NO 2
#define DEMO_APP_PRIO 100
#define DEMO_APP_QUE_SIZE (200 << 10)

#define DEMO_POST_TIMEOUT 2000

enum EM_EVENT_TYPE
{
    EVENT_MSG_POST = 1,                         // 普通消息发送事件;
    EVENT_SERVER_FILE_POST_INS_ALLOT_ACK,       // 服务端文件发送实例申请回复事件;
    EVENT_FILE_POST2C,                          // 客户端文件包接收响应事件;
    EVENT_FILE_POST2S,                          // 服务端文件包接收响应事件;

	// 文件传输进度绘图;
	EVENT_LIST_UI_PAINT,						// 列表信息绘制;
	EVENT_FIRST_PROGRESS_UI_PAINT,				// 第一个包的进度条绘制;
    EVENT_NORMAL_PROGRESS_UI_PAINT,				// 后续正常包的进度条绘制;
	EVENT_LAST_PROGRESS_UI_PAINT,				// 传送完毕的进度条绘制;

    EVENT_NONE
};

enum EM_DAEM_EVENT_TYPE
{
    EVENT_CLIENT_FILE_POST_INS_ALLOT = 1,       // 客户端文件发送实例申请事件;
    EVENT_SERVER_FILE_POST_INS_ALLOT,           // 服务端文件发送实例申请事件;
    EVENT_CLIENT_FILE_POST_INS_RELEASE,         // 客户端文件发送实例释放事件;
    EVENT_SERVER_FILE_POST_INS_RELEASE,         // 服务端文件发送实例释放事件;

    EVENT_END
};

enum INS_STATE_TYPE
{
    IDLE_STATE = 0,
    //STATE_ACK,
    //STATE_TERM,
    STATE_WORK
};

// 包的结构体;
typedef struct tagFileMessage
{
    s8  fileHead[4];       // 标志作用;
    u32 fileStart;         // 包中数据在整个文件中的起始位置标志;
    u32 fileSize;          // 包中数据的长度;
    u16 wCliPostInsNum;    // 包中记录客户端的发送instanceID;
    u16 wSerPostInsNum;    // 包中记录服务端的发送instanceID;
    u16 wIndex;            // 包中记录客户端的发送instance索引号;
    u8  filePacket[MAX_FILE_PACKET - 4 - 2*sizeof(int)];       // 包中所传输的文件数据;
}TFileMessage;

// 文件信息结构体;
typedef struct tagFileInfo
{
    u32 filePacketNum;						// 文件分包数;			
    u32 fileLength;							// 文件总长度;
    s8 strFileName[MAX_FILE_NAME + 1];	    // 文件名称;
}TFileInfo;

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
public:
    // Instance中的用户数据;
    //u32 m_uNodeNum;
    //u16 m_uCliInsNum;
    //u16 m_uSerInsNum;
    //u16 m_nUsedFlag;

    s32 m_nLastStart;                   // 上一个包的起始位置;
    s32 m_nLastSize;                    // 上一个包的大小;
    s32 m_nPktIndex;                    // 发送包的索引;
    s64 m_dnProgValve;                  // 发送包的进度值;
    s32 m_nErrorPktNum;                 // 错误包的数目;
    TCHAR m_strFilePath[MAX_PATH];      // 文件的路径信息;

    s32 m_nStart;                       // 文件开始发送标志;
    s32 m_nPuase;			            // 文件停止发送标志;
    s32 m_nCancel;			            // 文件取消发送标志;

    TFileInfo m_tFileInfo;
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

// 临时全局变量;
CFrameWindowWnd* pFrame = NULL;
TCHAR g_strFilePath[MAX_PATH] = _T("");
TCHAR g_strFileName[MAX_FILE_NAME] = _T("");
TCHAR g_strFolderPath[MAX_PATH] = _T("F:\\2");

u16 g_wNodeNum;
