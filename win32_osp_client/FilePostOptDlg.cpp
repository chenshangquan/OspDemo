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