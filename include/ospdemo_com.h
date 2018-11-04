#pragma once

#include "osp.h"

// 断链检测参数;
#define HB_PERIOD               10          // 链路检测周期;
#define MAX_HB_NUM              3           // 最大失败检测次数;

// 通讯消息参数;
#define INS_MSG_POST_NO         1           // 默认通讯消息发送的Instance号;
#define MAX_POST_MSG_LEN        255         // 最大通讯消息长度;

// 文件传输参数;
#define FILE_PACKET_LEN         10240 * 2   // 文件分包大小;
#define MAX_FILE_NAME           128         // 最大文件名长度;

// 其它相关;
#define MAX_STR_LEN             32          // 最大字符串长度;
#define DEMO_APP_SERVER_NO (u16)1           // server APP ID;
#define DEMO_APP_CLIENT_NO (u16)2           // client APP ID;
#define DEMO_APP_PRIO           100         // APP 创建的优先级;
#define DEMO_APP_QUE_SIZE       (200 << 10) // APP 创建的栈大小;
#define DEMO_POST_TIMEOUT       2000        // 消息发送超时时间;

// InstanceEntry event;
enum EM_EVENT_TYPE
{
    EVENT_MSG_POST = 11200,                     // 普通消息发送事件;
    EVENT_SERVER_FILE_POST_INS_ALLOT_ACK,       // 服务端文件发送实例申请回复事件;
    EVENT_FILE_POST2C,                          // 客户端文件包接收响应事件;
    EVENT_FILE_POST2S,                          // 服务端文件包接收响应事件;

	// 文件传输进度绘图;
    EVENT_LIST_UI_PAINT,						// 列表信息绘制;
	EVENT_DONE_LIST_UI_PAINT,				    // 完成列表信息绘制;
	EVENT_FIRST_PROGRESS_UI_PAINT,				// 第一个包的进度条绘制;
    EVENT_NORMAL_PROGRESS_UI_PAINT,				// 后续正常包的进度条绘制;
	EVENT_LAST_PROGRESS_UI_PAINT,				// 传送完毕的进度条绘制;

	EVENT_CLIENT_FILE_POST_INS_RELEASE_BF,      // 客户端文件发送实例释放事件;

    EVENT_NONE
};

// DaemonInstanceEntry event;
enum EM_DAEM_EVENT_TYPE
{
    EVENT_CLIENT_FILE_POST_INS_ALLOT = 12200,     // 客户端文件发送实例申请事件;
    EVENT_SERVER_FILE_POST_INS_ALLOT,             // 服务端文件发送实例申请事件;
    EVENT_SERVER_IS_FILE_EXIST,                   // 客户端待发送文件在服务端是否存在事件;
    EVENT_SERVER_IS_FILE_EXIST_ACK,               // 服务端文件是否存在消息回复事件;

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

// 包的结构体;
typedef struct tagFileMessage
{
    s8  fileHead[4];       // 标志作用;
    u32 fileStart;         // 包中数据在整个文件中的起始位置标志;
    u32 fileSize;          // 包中数据的长度;
    u8  filePacket[FILE_PACKET_LEN - 4 - 2*sizeof(u32)];       // 包中所传输的文件数据;
}TFileMessage;

// 文件信息结构体;
typedef struct tagFileInfo
{
    u32 filePacketNum;						// 文件分包数;			
    u32 fileLength;							// 文件总长度;
    s8  strFileName[MAX_FILE_NAME + 1];	    // 文件名称;
}TFileInfo;

// 文件信息结构体extand;
typedef struct tagFileInfoExt
{
	BOOL32 bIsFileCovered;                  // 同名文件是否覆盖标志位;
	u32 filePacketNum;						// 文件分包数;			
	u32 fileLength;							// 文件总长度;
	s8 strFileName[MAX_FILE_NAME + 1];	    // 文件名称;
}TFileInfoExt;

// 打印信息;
API void help();

API void prt( u8 byLevel );

API void showdev();

API void showsize();

API void mdver();

API void showver();

API void datastatus();

API void cpuadjust();

API void screen( u32 byScreen );
