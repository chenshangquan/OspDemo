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
            m_PaintManager.AddNotifier(this);   // ��ӿؼ�����Ϣ��Ӧ��������Ϣ�ͻᴫ�ﵽduilib����Ϣѭ�������ǿ�����Notify����������Ϣ����;
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

        CDuiString str;
        CListUI* pcLoadList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("ListDemo1")));

        // ���List�б����ݣ�������Add(pListElement)����SetText
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
            //pcLable->ApplyAttributeList(_T("text=\"��\" mouse=\"false\""));
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
