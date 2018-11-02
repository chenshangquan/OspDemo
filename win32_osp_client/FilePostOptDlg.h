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
    virtual void Notify(TNotifyUI& msg);
    virtual void OnClick(TNotifyUI& msg);
    virtual void InitWindow();
    void OnPrepare();

public:

    COptionUI  *m_pBtnYES;
    COptionUI  *m_pBtnNO;
    //COptionUI  *m_pExitbtn;
    //COptionUI  *m_pMoveToTraybtn;
    //CCheckBoxUI*m_pchkOption;

};

