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
    
    // 定义控件指针;
    CEditUI* m_pEditIPAddr;     // ip地址;
    CEditUI* m_pEditPort;       // 监听port;
    CEditUI* m_pEditPost;       // 待发送内容;
    CEditUI* m_pEditRecv;       // 待接收显示内容;
    CEditUI* m_pEditSelFile;    // 输出选择的文件路径;
    
    CButtonUI* m_pBtnConnect;       // 连接服务器;
    CButtonUI* m_pBtnDisConnect;    // 断开服务器连接;
    CButtonUI* m_pBtnSelFile2Pst;   // 选择待发送文件;
    CButtonUI* m_pBtnFilePost;      // 发送文件;
    CButtonUI* m_pBtnFilePostStp;   // 暂停文件发送;
    CButtonUI* m_pBtnFilePostCcl;   // 取消文件发送;
    //CButtonUI* m_pBtnFileRecv;      // 接收文件;
    CButtonUI* m_pBtnPost;          // 发送消息;
    CButtonUI* m_pBtnClose;         // 退出;

    CProgressUI* m_pProgress;       //进度条;


};