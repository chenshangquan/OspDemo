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
	CEditUI* m_pEditMsg;        // 信息窗口;
    CEditUI* m_pEditIPAddr;     // ip地址;
    CEditUI* m_pEditPort;       // 监听port;
    CEditUI* m_pEditPost;       // 待发送内容;
    CEditUI* m_pEditRecv;       // 待接收显示内容;
    CEditUI* m_pEditFolderSel;  // 输出选择的文件路径;
    
    CButtonUI* m_pBtnConfig;        // 服务器配置下发;
    CButtonUI* m_pBtnDisConnect;    // 断开服务器连接;
    CButtonUI* m_pBtnRecPosSel;     // 选择文件接收位置;
    CButtonUI* m_pBtnFileRec;       // 接收文件;
    CButtonUI* m_pBtnFileRecStp;    // 暂停文件接收;
    CButtonUI* m_pBtnFileRecCcl;    // 取消文件接收;
    //CButtonUI* m_pBtnFilePost;    // 发送文件;
    CButtonUI* m_pBtnPost;          // 发送消息;
    CButtonUI* m_pBtnClose;         // 退出;

    //CProgressUI* m_pProgress;       //进度条;

    // list
    CListUI* m_pList;               // 列表;
	CListHeaderUI* m_pListHeader;	// 列表头;
};