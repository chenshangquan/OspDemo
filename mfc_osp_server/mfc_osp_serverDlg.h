
// mfc_osp_serverDlg.h : header file
//

#pragma once


// Cmfc_osp_serverDlg dialog
class Cmfc_osp_serverDlg : public CDialogEx
{
// Construction
public:
	Cmfc_osp_serverDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MFC_OSP_SERVER_DIALOG };

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
};
