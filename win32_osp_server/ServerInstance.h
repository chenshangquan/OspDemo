#pragma once

#include "../osplib/include/kdvtype.h"
#include "../osplib/include/osp.h"
#include "../include/ospdemo_com.h"

#define MAX_INS_NO 20

enum FILE_PACKET_TYPE
{
    FILE_PACKET_OK = 1,
    FILE_PACKET_ERROR,
    FILE_PACKET_END,
    FILE_PACKET_CANCEL
};

class CServerInstance :
    public CInstance
{
public:
    // Instance中的用户数据;
    u32 m_dwNodeNum;					// Node ID;
    u16 m_wCliInsNum;					// 客户端InsNo;
    //u16 m_uSerInsNum;
	HANDLE m_hFile;						// 文件句柄;
    u16 m_nUsedFlag;					// 是否传包中;

    s32 m_nLastStart;                   // 上一个包的起始位置;
    s32 m_nLastSize;                    // 上一个包的大小;
    u32 m_dwPktIndex;                    // 发送包的索引;
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
	void MsgPostFunc(CMessage *const pMsg);
	void ServerFilePostInsAllot(CMessage *const pcMsg, CApp* pcApp);
	void OnServerReceive(CMessage *const pMsg);
	void ReceiveFilePacket(TFileMessage *strFileMsgGet, CMessage *const pMsg);
	void SendFilePacketEcho(s32 nFlag);
	bool StoreFilePacket(TFileMessage *strFileMsgGet);
	void InsRelease(CMessage *const pMsg);
	void SerFilePostInsRelease(u32 dwInsNo);
    //void OnMyReceive(CMessage *const pMsg);
    //void SendFileInfo(s32 fStart,s32 fSize,char *fHead, u16 wCliPostInsNo, u16 wSerPostInsNo, u16 wIndex);
    //void ListUI2Paint();
    //void OnBnClickedFilePst();
public:
    CServerInstance(void);
    ~CServerInstance(void);
};

