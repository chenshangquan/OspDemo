#include "StdAfx.h"
#include "FilePostOptDlg.h"


CFilePostOptDlg::CFilePostOptDlg(void)
{
    m_pBtnYES = NULL;
    m_pBtnNO  = NULL;
}


CFilePostOptDlg::~CFilePostOptDlg(void)
{
}

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
    return _T("");
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
			Close(IDOK);
		}
		else if (msg.pSender->GetName() == _T("nobtn"))
		{
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