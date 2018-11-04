#pragma once

#include "osp.h"

// ����������;
#define HB_PERIOD               10          // ��·�������;
#define MAX_HB_NUM              3           // ���ʧ�ܼ�����;

// ͨѶ��Ϣ����;
#define INS_MSG_POST_NO         1           // Ĭ��ͨѶ��Ϣ���͵�Instance��;
#define MAX_POST_MSG_LEN        255         // ���ͨѶ��Ϣ����;

// �ļ��������;
#define FILE_PACKET_LEN         10240 * 2   // �ļ��ְ���С;
#define MAX_FILE_NAME           128         // ����ļ�������;

// �������;
#define MAX_STR_LEN             32          // ����ַ�������;
#define DEMO_APP_SERVER_NO (u16)1           // server APP ID;
#define DEMO_APP_CLIENT_NO (u16)2           // client APP ID;
#define DEMO_APP_PRIO           100         // APP ���������ȼ�;
#define DEMO_APP_QUE_SIZE       (200 << 10) // APP ������ջ��С;
#define DEMO_POST_TIMEOUT       2000        // ��Ϣ���ͳ�ʱʱ��;

// InstanceEntry event;
enum EM_EVENT_TYPE
{
    EVENT_MSG_POST = 11200,                     // ��ͨ��Ϣ�����¼�;
    EVENT_SERVER_FILE_POST_INS_ALLOT_ACK,       // ������ļ�����ʵ������ظ��¼�;
    EVENT_FILE_POST2C,                          // �ͻ����ļ���������Ӧ�¼�;
    EVENT_FILE_POST2S,                          // ������ļ���������Ӧ�¼�;

	// �ļ�������Ȼ�ͼ;
    EVENT_LIST_UI_PAINT,						// �б���Ϣ����;
	EVENT_DONE_LIST_UI_PAINT,				    // ����б���Ϣ����;
	EVENT_FIRST_PROGRESS_UI_PAINT,				// ��һ�����Ľ���������;
    EVENT_NORMAL_PROGRESS_UI_PAINT,				// �����������Ľ���������;
	EVENT_LAST_PROGRESS_UI_PAINT,				// ������ϵĽ���������;

	EVENT_CLIENT_FILE_POST_INS_RELEASE_BF,      // �ͻ����ļ�����ʵ���ͷ��¼�;

    EVENT_NONE
};

// DaemonInstanceEntry event;
enum EM_DAEM_EVENT_TYPE
{
    EVENT_CLIENT_FILE_POST_INS_ALLOT = 12200,     // �ͻ����ļ�����ʵ�������¼�;
    EVENT_SERVER_FILE_POST_INS_ALLOT,             // ������ļ�����ʵ�������¼�;
    EVENT_SERVER_IS_FILE_EXIST,                   // �ͻ��˴������ļ��ڷ�����Ƿ�����¼�;
    EVENT_SERVER_IS_FILE_EXIST_ACK,               // ������ļ��Ƿ������Ϣ�ظ��¼�;

    EVENT_END
};

// Instance state type;
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
    u8  filePacket[FILE_PACKET_LEN - 4 - 2*sizeof(u32)];       // ������������ļ�����;
}TFileMessage;

// �ļ���Ϣ�ṹ��;
typedef struct tagFileInfo
{
    u32 filePacketNum;						// �ļ��ְ���;			
    u32 fileLength;							// �ļ��ܳ���;
    s8  strFileName[MAX_FILE_NAME + 1];	    // �ļ�����;
}TFileInfo;

// �ļ���Ϣ�ṹ��extand;
typedef struct tagFileInfoExt
{
	BOOL32 bIsFileCovered;                  // ͬ���ļ��Ƿ񸲸Ǳ�־λ;
	u32 filePacketNum;						// �ļ��ְ���;			
	u32 fileLength;							// �ļ��ܳ���;
	s8 strFileName[MAX_FILE_NAME + 1];	    // �ļ�����;
}TFileInfoExt;

// ��ӡ��Ϣ;
API void help();

API void prt( u8 byLevel );

API void showdev();

API void showsize();

API void mdver();

API void showver();

API void datastatus();

API void cpuadjust();

API void screen( u32 byScreen );
