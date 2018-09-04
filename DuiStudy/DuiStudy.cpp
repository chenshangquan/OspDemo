#pragma once
#include "stdafx.h"
#include "DuiStudy.h"

#if 0
class CDuiFrameWnd : public CWindowWnd, public INotifyUI
{
public:
    virtual LPCTSTR GetWindowClassName() const { return _T("DUIMainFrame"); }
    virtual void    Notify(TNotifyUI& msg) 
    {
        if(msg.sType == _T("click"))
        {
            if(msg.pSender->GetName() == _T("btnHello")) 
            {
                ::MessageBox(NULL, _T("我是按钮"), _T("点击了按钮"), NULL);
            }
            if (msg.pSender->GetName() == _T("closebtn"))
            {
                Close();
            }
        }
    }

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        LRESULT lRes = 0;

        if( uMsg == WM_CREATE ) 
        {
            m_PaintManager.Init(m_hWnd);

            CDialogBuilder builder;
            CControlUI* pRoot = builder.Create(_T("duilib.xml"), (UINT)0, NULL, &m_PaintManager);   // duilib.xml需要放到exe目录下;
            ASSERT(pRoot && "Failed to parse XML");

            m_PaintManager.AttachDialog(pRoot);
            m_PaintManager.AddNotifier(this);   // 添加控件等消息响应，这样消息就会传达到duilib的消息循环，我们可以在Notify函数里做消息处理;
            return lRes;
        }

        // 以下3个消息WM_NCACTIVATE、WM_NCCALCSIZE、WM_NCPAINT用于屏蔽系统标题栏;
        else if( uMsg == WM_NCACTIVATE ) 
        {
            if( !::IsIconic(m_hWnd) ) 
            {
                return (wParam == 0) ? TRUE : FALSE;
            }
        }
        else if( uMsg == WM_NCCALCSIZE ) 
        {
            return 0;
        }
        else if( uMsg == WM_NCPAINT ) 
        {
            return 0;
        }

        if( m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes) ) 
        {
            return lRes;
        }

        return __super::HandleMessage(uMsg, wParam, lParam);
    }

protected:
    CPaintManagerUI m_PaintManager;
};
#else
class CDuiFrameWnd : public WindowImplBase
{
public:
    virtual LPCTSTR    GetWindowClassName() const   {   return _T("DUIMainFrame");  }
    virtual CDuiString GetSkinFile()                {   return _T("duilib.xml");  }
    virtual CDuiString GetSkinFolder()              {   return _T("");  }

    virtual void       InitWindow()
    {
        CActiveXUI* pActiveXUI = static_cast<CActiveXUI*>(m_PaintManager.FindControl(_T("ActiveXDemo1")));

        if( pActiveXUI ) 
        {
            IWebBrowser2* pWebBrowser = NULL;

            pActiveXUI->SetDelayCreate(false);              // 相当于界面设计器里的DelayCreate属性改为FALSE，在duilib自带的FlashDemo里可以看到此属性为TRUE  ;           
            pActiveXUI->CreateControl(CLSID_WebBrowser);    // 相当于界面设计器里的Clsid属性里填入{8856F961-340A-11D0-A96B-00C04FD705A2}，建议用CLSID_WebBrowser，如果想看相应的值，请见<ExDisp.h>;
            pActiveXUI->GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);

            if( pWebBrowser != NULL ) 
            {
                //pWebBrowser->Navigate(L"https://code.google.com/p/duilib/",NULL,NULL,NULL,NULL);  
                pWebBrowser->Navigate(L"http://www.baidu.com/",NULL,NULL,NULL,NULL);  // 由于谷歌时不时被墙，所以换成反应快的网站;
                pWebBrowser->Release();
            }
        }

        CDuiString str;
        CListUI* pcLoadList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("ListDemo1")));

        // 添加List列表内容，必须先Add(pListElement)，再SetText
        for (int i = 0; i < 5; i++)
        {
#if 0
            CListTextElementUI* pListElement = new CListTextElementUI;
            pListElement->SetTag(i);
            pList->Add(pListElement);

            str.Format(_T("%d"), i);
            pListElement->SetText(0, str);
            pListElement->SetText(1, _T("haha"));
#else
            CListContainerElementUI* pcListContainer = new CListContainerElementUI;
            //CLabelUI* pcLable = new CLabelUI;
            //CListTextElementUI* pcListText = new CListTextElementUI;
            CProgressUI* pcProgress = new CProgressUI;

            pcListContainer->ApplyAttributeList(_T("height=\"25\" align=\"right\""));
            pcProgress->ApplyAttributeList(_T("text=\"Progress\" width=\"59\" height=\"20\" foreimage=\"OspDemoSkins\\progress_fore.png\"\
                min=\"0\" max=\"100\" value=\"50\" hor=\"true\" align=\"center\""));
            //pcListContainer->Add(pcProgress);
            //pcLable->ApplyAttributeList(_T("text=\"打开\" mouse=\"false\""));
            pcListContainer->Add(pcProgress);
            pcLoadList->Add(pcListContainer);
#endif
        }
    }

#if 1
    virtual void Notify( TNotifyUI& msg )
    {
        if(msg.sType == _T("selectchanged"))
        {
            CDuiString    strName     = msg.pSender->GetName();
            CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tabTest")));

            if(strName == _T("OptionDemo1"))
                pControl->SelectItem(0);
            else if(strName == _T("OptionDemo2"))
                pControl->SelectItem(1);
            else if(strName == _T("OptionDemo3"))
                pControl->SelectItem(2);
        }

        __super::Notify(msg);
    }
#endif
};


#endif

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());   // 设置资源的默认路径（此处设置为和exe在同一目录）;
    HRESULT Hr = ::CoInitialize(NULL);
    if( FAILED(Hr) ) return 0;

    CDuiFrameWnd duiFrame;
    duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    duiFrame.CenterWindow();
    duiFrame.ShowModal();

    ::CoUninitialize();
    return 0;
}
