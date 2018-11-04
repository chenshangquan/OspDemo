#include "StdAfx.h"
#include "FilePostOptDlg.h"

//BEGIN_MESSAGE_MAP(CFilePostOptDlg, WindowImplBase)
//	ON_MESSAGE(WM_IS_FILE_OVERWRITTEN, &CFilePostOptDlg::OnFilePost)
//END_MESSAGE_MAP()

CFilePostOptDlg::CFilePostOptDlg()
{
    m_pBtnYES = NULL;
    m_pBtnNO  = NULL;
}


CFilePostOptDlg::~CFilePostOptDlg()
{
}

//LRESULT CFilePostOptDlg::OnFilePost(WPARAM wParam, LPARAM lParam)
//{
//	return 0;
//}

LPCTSTR CFilePostOptDlg::GetWindowClassName() const
{
    return _T("FilePostOptDlg");
}

void CFilePostOptDlg::OnFinalMessage(HWND hWnd)
{
    CWindowWnd::OnFinalMessage(hWnd);
}
CDuiString CFilePostOptDlg::GetSkinFile()
{
    return _T("FilePostOptDlg.xml");
}

CDuiString  CFilePostOptDlg::GetSkinFolder()
{
	return _T("");
}

LRESULT CFilePostOptDlg::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (wParam == VK_ESCAPE)
	{
		//Close();
		return TRUE;
	}

	return FALSE;
}

UILIB_RESOURCETYPE CFilePostOptDlg::GetResourceType() const
{
	return UILIB_FILE;
}

CControlUI* CFilePostOptDlg::CreateControl(LPCTSTR pstrClassName)
{
	return NULL;
}

void CFilePostOptDlg::InitWindow()
{
	m_pBtnYES = static_cast<COptionUI *>(m_PaintManager.FindControl(_T("yesbtn")));
	m_pBtnNO = static_cast<COptionUI *>(m_PaintManager.FindControl(_T("nobtn")));
	//m_pExitbtn = static_cast<COptionUI *>(m_PaintManager.FindControl(TEXT("ExitDirectbtn")));
	//m_pMoveToTraybtn = static_cast<COptionUI *>(m_PaintManager.FindControl(TEXT("MoveInTraybtn")));
	//m_pchkOption =  static_cast<CCheckBoxUI*>(m_PaintManager.FindControl(TEXT("chkOption")));
}

void CFilePostOptDlg::OnPrepare()
{

}

void CFilePostOptDlg::Notify(TNotifyUI& msg)
{
	if( msg.sType == TEXT("windowinit") ) OnPrepare();

	if(msg.sType == TEXT("click"))
	{
		if(msg.pSender->GetName()==TEXT("closebtn"))
		{
			Close(IDCLOSE);
		}
		else if(msg.pSender->GetName() == TEXT("yesbtn"))
		{
			//SendMessage(GetParent(), WM_IS_FILE_OVERWRITTEN, (WPARAM)EM_FILE_OVERWRITTEN, 0);
			Close(IDOK);
		}
		else if (msg.pSender->GetName() == _T("nobtn"))
		{
			//SendMessage(GetParent(), WM_IS_FILE_OVERWRITTEN, (WPARAM)EM_FILE_NOT_OVERWRITTEN, 0);
			Close(IDCANCEL);
		}
	}
	WindowImplBase::Notify(msg);
}

void CFilePostOptDlg::OnClick(TNotifyUI& msg)
{
	__super::OnClick(msg);
}

//禁用双击标题栏窗口最大化;
LRESULT CFilePostOptDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;

	switch( uMsg ) 
	{
	case WM_NCLBUTTONDBLCLK:
		//不做处理;
		{
			return 0;
		}
		break;
	default:
		bHandled = FALSE;
	}
	return WindowImplBase::HandleMessage(uMsg,wParam,lParam);
}

LRESULT CFilePostOptDlg::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	//return WindowImplBase::MessageHandler(uMsg, wParam, lParam, bHandled);
	bHandled = FALSE;
	return 0;
}