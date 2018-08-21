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
                ::MessageBox(NULL, _T("���ǰ�ť"), _T("����˰�ť"), NULL);
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
            CControlUI* pRoot = builder.Create(_T("duilib.xml"), (UINT)0, NULL, &m_PaintManager);   // duilib.xml��Ҫ�ŵ�exeĿ¼��;
            ASSERT(pRoot && "Failed to parse XML");

            m_PaintManager.AttachDialog(pRoot);
            m_PaintManager.AddNotifier(this);   // ���ӿؼ�����Ϣ��Ӧ��������Ϣ�ͻᴫ�ﵽduilib����Ϣѭ�������ǿ�����Notify����������Ϣ����;
            return lRes;
        }

        // ����3����ϢWM_NCACTIVATE��WM_NCCALCSIZE��WM_NCPAINT��������ϵͳ������;
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

            pActiveXUI->SetDelayCreate(false);              // �൱�ڽ�����������DelayCreate���Ը�ΪFALSE����duilib�Դ���FlashDemo����Կ���������ΪTRUE  ;           
            pActiveXUI->CreateControl(CLSID_WebBrowser);    // �൱�ڽ�����������Clsid����������{8856F961-340A-11D0-A96B-00C04FD705A2}��������CLSID_WebBrowser������뿴��Ӧ��ֵ�����<ExDisp.h>;
            pActiveXUI->GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);

            if( pWebBrowser != NULL ) 
            {
                //pWebBrowser->Navigate(L"https://code.google.com/p/duilib/",NULL,NULL,NULL,NULL);  
                pWebBrowser->Navigate(L"http://www.baidu.com/",NULL,NULL,NULL,NULL);  // ���ڹȸ�ʱ��ʱ��ǽ�����Ի��ɷ�Ӧ�����վ;
                pWebBrowser->Release();
            }
        }
    }

};


#endif

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath());   // ������Դ��Ĭ��·�����˴�����Ϊ��exe��ͬһĿ¼��;
    HRESULT Hr = ::CoInitialize(NULL);
    if( FAILED(Hr) ) return 0;

    CDuiFrameWnd duiFrame;
    duiFrame.Create(NULL, _T("DUIWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    duiFrame.CenterWindow();
    duiFrame.ShowModal();

    ::CoUninitialize();
    return 0;
}