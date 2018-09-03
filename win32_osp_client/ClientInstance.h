#pragma once

#include "../osplib/include/kdvtype.h"
#include "../osplib/include/osp.h"
#include "../include/ospdemo_com.h"

#define MAX_INS_NO 10

class CClientInstance :
    public CInstance
{
public:
    // Instance中的用户数据;
    //u32 m_uNodeNum;
    //u16 m_uCliInsNum;
    u16 m_wSerInsNum;
	HANDLE m_hFile;
    u16 m_nUsedFlag;

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
    //void OnMyReceive(CMessage *const pMsg);
	void ClientFilePostInsAllot(CMessage *const pcMsg, CApp* pcApp);
	void ClientFilePostInsAllotAck(CMessage *const pMsg);
    void SendFileInfo(s32 fStart,s32 fSize,char *fHead);
	void OnClientReceive(CMessage *const pMsg);
	void CliFilePostInsRelease(u32 dwInsNo);
    void ListUI2Paint();
    //void OnBnClickedFilePst();
public:
    CClientInstance(void);
    ~CClientInstance(void);
};


// CDemoListContainerElementUI类函数声明;
#if 1
class CDemoListContainerElementUI : public CListContainerElementUI
{
public:
    void SetPos(RECT rc);
    CListHeaderUI *m_pHeader;
};
#endif