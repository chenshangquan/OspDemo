#pragma once
#include "../duilib/Utils/WinImplBase.h"
#include "osp.h"

class CFilePostOptDlg :
    public WindowImplBase
{
public:
    CFilePostOptDlg(void);
    ~CFilePostOptDlg(void);
public:
    virtual LPCTSTR GetWindowClassName() const;
    virtual void OnFinalMessage(HWND hWnd);
    virtual CDuiString GetSkinFile();
    virtual CDuiString GetSkinFolder();
    virtual UILIB_RESOURCETYPE GetResourceType() const;
    virtual CControlUI* CreateControl(LPCTSTR pstrClassName);
    virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam); 
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
    virtual void Notify(TNotifyUI& msg);
    virtual void OnClick(TNotifyUI& msg);
    virtual void InitWindow();
    void OnPrepare();
protected:
	//LRESULT OnFilePost(WPARAM wParam, LPARAM lParam);
	//DECLARE_MESSAGE_MAP()
public:

    CButtonUI  *m_pBtnYES;
    CButtonUI  *m_pBtnNO;
    CButtonUI  *m_pExitbtn;
    //COptionUI  *m_pMoveToTraybtn;
    //CCheckBoxUI*m_pchkOption;

};

