#pragma once

#include "resource.h"

class CFrameWindowWnd : public CWindowWnd, public INotifyUI
{
public:
    CFrameWindowWnd(void);
    ~CFrameWindowWnd(void);

    void Init();
    LPCTSTR GetWindowClassName() const;
    UINT GetClassStyle() const;
    void OnFinalMessage(HWND /*hWnd*/);
    void Notify(TNotifyUI& msg);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    CPaintManagerUI m_pm;
    
    // ����ؼ�ָ��;
	CEditUI* m_pEditMsg;        // ��Ϣ����;
    CEditUI* m_pEditIPAddr;     // ip��ַ;
    CEditUI* m_pEditPort;       // ����port;
    CEditUI* m_pEditPost;       // ����������;
    CEditUI* m_pEditRecv;       // ��������ʾ����;
    CEditUI* m_pEditFolderSel;  // ���ѡ����ļ�·��;
    
    CButtonUI* m_pBtnConfig;        // �����������·�;
    CButtonUI* m_pBtnDisConnect;    // �Ͽ�����������;
    CButtonUI* m_pBtnRecPosSel;     // ѡ���ļ�����λ��;
    CButtonUI* m_pBtnFileRec;       // �����ļ�;
    CButtonUI* m_pBtnFileRecStp;    // ��ͣ�ļ�����;
    CButtonUI* m_pBtnFileRecCcl;    // ȡ���ļ�����;
    //CButtonUI* m_pBtnFilePost;    // �����ļ�;
    CButtonUI* m_pBtnPost;          // ������Ϣ;
    CButtonUI* m_pBtnClose;         // �˳�;

    //CProgressUI* m_pProgress;       //������;

    // list
    CListUI* m_pList;               // �б�;
	CListHeaderUI* m_pListHeader;	// �б�ͷ;
};