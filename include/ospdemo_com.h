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
    EVENT_MSG_POST = 1,                         // ��ͨ��Ϣ�����¼�;
    EVENT_SERVER_FILE_POST_INS_ALLOT_ACK,       // ������ļ�����ʵ������ظ��¼�;
    EVENT_FILE_POST2C,                          // �ͻ����ļ���������Ӧ�¼�;
    EVENT_FILE_POST2S,                          // ������ļ���������Ӧ�¼�;

	// �ļ�������Ȼ�ͼ;
	EVENT_LIST_UI_PAINT,						// �б���Ϣ����;
	EVENT_FIRST_PROGRESS_UI_PAINT,				// ��һ�����Ľ���������;
    EVENT_NORMAL_PROGRESS_UI_PAINT,				// �����������Ľ���������;
	EVENT_LAST_PROGRESS_UI_PAINT,				// ������ϵĽ���������;

    EVENT_NONE
};

enum EM_DAEM_EVENT_TYPE
{
    EVENT_CLIENT_FILE_POST_INS_ALLOT = 1,       // �ͻ����ļ�����ʵ�������¼�;
    EVENT_SERVER_FILE_POST_INS_ALLOT,           // ������ļ�����ʵ�������¼�;
    EVENT_CLIENT_FILE_POST_INS_RELEASE,         // �ͻ����ļ�����ʵ���ͷ��¼�;
    EVENT_SERVER_FILE_POST_INS_RELEASE,         // ������ļ�����ʵ���ͷ��¼�;

    EVENT_END
};

enum INS_STATE_TYPE
{
    IDLE_STATE = 0,
    //STATE_ACK,
    //STATE_TERM,
    STATE_WORK
};

// ���Ľṹ��;
typedef struct tagFileMessage
{
    s8  fileHead[4];       // ��־����;
    u32 fileStart;         // ���������������ļ��е���ʼλ�ñ�־;
    u32 fileSize;          // �������ݵĳ���;
    u16 wCliPostInsNum;    // ���м�¼�ͻ��˵ķ���instanceID;
    u16 wSerPostInsNum;    // ���м�¼����˵ķ���instanceID;
    u16 wIndex;            // ���м�¼�ͻ��˵ķ���instance������;
    u8  filePacket[MAX_FILE_PACKET - 4 - 2*sizeof(int)];       // ������������ļ�����;
}TFileMessage;

// �ļ���Ϣ�ṹ��;
typedef struct tagFileInfo
{
    u32 filePacketNum;						// �ļ��ְ���;			
    u32 fileLength;							// �ļ��ܳ���;
    s8 strFileName[MAX_FILE_NAME + 1];	    // �ļ�����;
}TFileInfo;

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
    //u32 m_uNodeNum;
    //u16 m_uCliInsNum;
    //u16 m_uSerInsNum;
    //u16 m_nUsedFlag;

    s32 m_nLastStart;                   // ��һ��������ʼλ��;
    s32 m_nLastSize;                    // ��һ�����Ĵ�С;
    s32 m_nPktIndex;                    // ���Ͱ�������;
    s64 m_dnProgValve;                  // ���Ͱ��Ľ���ֵ;
    s32 m_nErrorPktNum;                 // ���������Ŀ;
    TCHAR m_strFilePath[MAX_PATH];      // �ļ���·����Ϣ;

    s32 m_nStart;                       // �ļ���ʼ���ͱ�־;
    s32 m_nPuase;			            // �ļ�ֹͣ���ͱ�־;
    s32 m_nCancel;			            // �ļ�ȡ�����ͱ�־;

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

// ��ʱȫ�ֱ���;
CFrameWindowWnd* pFrame = NULL;
TCHAR g_strFilePath[MAX_PATH] = _T("");
TCHAR g_strFileName[MAX_FILE_NAME] = _T("");
TCHAR g_strFolderPath[MAX_PATH] = _T("F:\\2");

u16 g_wNodeNum;
