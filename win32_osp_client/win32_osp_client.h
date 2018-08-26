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
    
    // edit
    CEditUI* m_pEditIPAddr;     // ip��ַ;
    CEditUI* m_pEditPort;       // ����port;
    CEditUI* m_pEditPost;       // ����������;
    CEditUI* m_pEditRecv;       // ��������ʾ����;
    CEditUI* m_pEditSelFile;    // ���ѡ����ļ�·��;
    
    //  button
    CButtonUI* m_pBtnConnect;       // ���ӷ�����;
    CButtonUI* m_pBtnDisConnect;    // �Ͽ�����������;
    CButtonUI* m_pBtnSelFile2Pst;   // ѡ��������ļ�;
    CButtonUI* m_pBtnFilePost;      // �����ļ�;
    CButtonUI* m_pBtnFilePostStp;   // ��ͣ�ļ�����;
    CButtonUI* m_pBtnFilePostCcl;   // ȡ���ļ�����;
    //CButtonUI* m_pBtnFileRecv;      // �����ļ�;
    CButtonUI* m_pBtnPost;          // ������Ϣ;
    CButtonUI* m_pBtnClose;         // �˳�;

    // progress
    //CProgressUI* m_pProgress;       //������;

    // list
    CListUI* m_pList;               // �б�;
	CListHeaderUI* m_pListHeader;	// �б�ͷ;


};