
// MeruCabsBookingApplicationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MeruCabsBookingApplication.h"
#include "MeruCabsBookingApplicationDlg.h"
#include "afxdialogex.h"

#include "NiTEHandTracker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DIMX 550
#define DIMY 400

using namespace cv;
using namespace std;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMeruCabsBookingApplicationDlg dialog

CMeruCabsBookingApplicationDlg::CMeruCabsBookingApplicationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMeruCabsBookingApplicationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMeruCabsBookingApplicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMeruCabsBookingApplicationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_START, &CMeruCabsBookingApplicationDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &CMeruCabsBookingApplicationDlg::OnBnClickedBtnStop)
END_MESSAGE_MAP()


// CMeruCabsBookingApplicationDlg message handlers

BOOL CMeruCabsBookingApplicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	isCapturing  = false;

	dataCapture.setMode(C_MODE_COLOR);

	///////////////////////////////////////////////
	if(!dataCapture.init())
	{
		cout << "Error initializing" << endl;
		return false;
	}
	//////////////////////////////////////////////

	//////////////////////////////////////////////
	//NiTEHandTracker handTracker;
	//try
	//{
	//	handTracker.openDeviceForTracking();
	//	handTracker.init();
	//}
	//catch(const char* err)
	//{
	//	CStringW errorMsg(err);
	//	AfxMessageBox(errorMsg, MB_OK | MB_ICONSTOP);
	//	PostMessage(WM_CLOSE);
	//}
	/////////////////////////////////////////////

	mPictureCtrl = (CStatic *)GetDlgItem(IDC_STATIC_PICTURE);

	/*vector<Mat> dataStream;
	OpenCVKinect dataCapture;

	dataCapture.setMode(C_MODE_COLOR);

	if(!dataCapture.init())
	{
	cout << "Error initializing" << endl;
	return 1;
	}

	char ch = ' ';

	while(ch != 27)
	{
	dataStream = dataCapture.getData();
	cout << dataStream.size() << endl;

	imshow("Captured Frames", dataStream[C_COLOR_STREAM]);
	ch = waitKey(10);
	}  */

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMeruCabsBookingApplicationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.

void CMeruCabsBookingApplicationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
// the minimized window.
HCURSOR CMeruCabsBookingApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

UINT CMeruCabsBookingApplicationDlg::captureThread() 
{
	CImage frame;
	vector<Mat> dataStream;

	isCapturing = true;
	dataCapture.faceCount = 1;

	CPoint *pt = new CPoint(DIMX, DIMY);

	while(isCapturing)
	{
		dataStream = dataCapture.getData();
		cout << dataStream.size() << endl;

		dataCapture.convertMatToImage(&dataStream[C_COLOR_STREAM], frame);
		
		CBitmap resizedBitmap; 
		CDC *screenDC = GetDC();

		dataCapture.getResizedImage(screenDC, pt, frame, resizedBitmap);

		mPictureCtrl->SetBitmap((HBITMAP)resizedBitmap.Detach());
		mPictureCtrl->Invalidate();
		ReleaseDC(screenDC);
		frame.Destroy();
	}

	mPictureCtrl->SetBitmap(NULL);
	delete pt;

	return 1;
}

UINT __cdecl CMeruCabsBookingApplicationDlg::captureThreadFunc(LPVOID pParam)
{
    CMeruCabsBookingApplicationDlg *dlgApp = reinterpret_cast<CMeruCabsBookingApplicationDlg*>(pParam);
	UINT retCode = 0;

	if(!dlgApp->isCapturing)
    {
		retCode = dlgApp->captureThread();
	}

    return retCode;
}

void CMeruCabsBookingApplicationDlg::OnBnClickedBtnStart()
{
	dataCapture.initFaceRecognizer();
	AfxBeginThread(captureThreadFunc, this);  
}

void CMeruCabsBookingApplicationDlg::OnBnClickedBtnStop()
{
	isCapturing = false;
}

