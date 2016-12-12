
// MeruCabsBookingApplicationDlg.h : header file
//

#pragma once
#include "OpenCVKinect.h"

// CMeruCabsBookingApplicationDlg dialog
class CMeruCabsBookingApplicationDlg : public CDialogEx
{

	CStatic* mPictureCtrl;
	OpenCVKinect dataCapture;
	bool isCapturing;

// Construction
public:
	CMeruCabsBookingApplicationDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MERUCABSBOOKINGAPPLICATION_DIALOG };

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
	static UINT __cdecl captureThreadFunc(LPVOID pParam);
	UINT captureThread();

	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();
};
