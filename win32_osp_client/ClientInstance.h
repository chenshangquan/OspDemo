#pragma once

#include "../osplib/include/kdvtype.h"
#include "../osplib/include/osp.h"
#include "../include/ospdemo_com.h"

#define MAX_INS_NO 10

class CClientInstance :
    public CInstance
{
public:
    // Instance�е��û�����;
    //u32 m_uNodeNum;
    //u16 m_uCliInsNum;
    //u16 m_uSerInsNum;
    u16 m_nUsedFlag;

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
    void SendFileInfo(s32 fStart,s32 fSize,char *fHead, u16 wCliPostInsNo, u16 wSerPostInsNo, u16 wIndex);
    void ListUI2Paint();
    //void OnBnClickedFilePst();
public:
    CClientInstance(void);
    ~CClientInstance(void);
};


// CDemoListContainerElementUI�ຯ������;
#if 1
class CDemoListContainerElementUI : public CListContainerElementUI
{
public:
    void SetPos(RECT rc);
    CListHeaderUI *m_pHeader;
};
#endif