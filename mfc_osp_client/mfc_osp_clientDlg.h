
// mfc_osp_clientDlg.h : header file
//

#pragma once


// Cmfc_osp_clientDlg dialog
class Cmfc_osp_clientDlg : public CDialogEx
{
// Construction
public:
	Cmfc_osp_clientDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MFC_OSP_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton4();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedButton3();
};
