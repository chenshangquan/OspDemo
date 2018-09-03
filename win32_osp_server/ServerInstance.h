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
    // Instance�е��û�����;
    u32 m_dwNodeNum;					// Node ID;
    u16 m_wCliInsNum;					// �ͻ���InsNo;
    //u16 m_uSerInsNum;
	HANDLE m_hFile;						// �ļ����;
    u16 m_nUsedFlag;					// �Ƿ񴫰���;

    s32 m_nLastStart;                   // ��һ��������ʼλ��;
    s32 m_nLastSize;                    // ��һ�����Ĵ�С;
    u32 m_dwPktIndex;                    // ���Ͱ�������;
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

