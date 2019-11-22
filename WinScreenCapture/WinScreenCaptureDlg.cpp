
// WinScreenCaptureDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "WinScreenCapture.h"
#include "WinScreenCaptureDlg.h"
#include "afxdialogex.h"
#include "CDxWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWinScreenCaptureDlg 对话框



CWinScreenCaptureDlg::CWinScreenCaptureDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WINSCREENCAPTURE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWinScreenCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWinScreenCaptureDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CWinScreenCaptureDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CWinScreenCaptureDlg 消息处理程序

BOOL CWinScreenCaptureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	return TRUE;  //除非将焦点设置到控件，否则返回 TRUE
}

void CWinScreenCaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWinScreenCaptureDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWinScreenCaptureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CWinScreenCaptureDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CWinScreenCaptureDlg::OnBnClickedButton1()
{
	CDxWnd* pWnd = new CDxWnd();
	//BOOL bRet = pWnd->CreateEx(0, L"Dx11Wnd", L"Dx11Wnd", WS_SYSMENU | WS_VISIBLE | WS_CHILD, { 0, 0, 1280, 720 }, CWnd::FromHandle(m_hWnd), 0);
// 	BOOL bRet = pWnd->CreateEx(WS_EX_APPWINDOW, // Make a client edge label.
// 		::AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, ::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW + 1), NULL),
// 		_T("Hi"), WS_VISIBLE | WS_SYSMENU | WS_POPUP, { 0, 0, 1280, 720 }, NULL, 1112);
	CString className = AfxRegisterWndClass(CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW),
		NULL, AfxGetApp()->LoadIcon(IDR_MAINFRAME));
	BOOL bRet = pWnd->CreateEx(WS_EX_APPWINDOW, className, L"dx!!!",
		WS_CAPTION | WS_VISIBLE | WS_SYSMENU | WS_POPUP | WS_BORDER, { 0, 0, 1280,720 }, NULL, NULL);
	DWORD dwErr = 0;
	if (!bRet)
	{
		dwErr = GetLastError();
	}
	pWnd->ShowWindow(SW_SHOW);
	// 	MSG msg;
	// 	while (true)
	// 	{
	// 		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	// 		{
	// 			if (msg.hwnd == pWnd->m_hWnd)
	// 			{
	// 				wchar_t wcBuffer[256] = { 0 };
	// 				swprintf_s(wcBuffer, 256, L"message=%d\n", msg.message);
	// 				OutputDebugStringW(wcBuffer);
	// 				//break;
	// 			}
	// 			if (msg.message == WM_QUIT)
	// 				break;
	// 			TranslateMessage(&msg);
	// 			DispatchMessage(&msg);
	// 		}
	// 		else
	// 		{
	// 			pWnd->doRender();
	// 		}
	// 	}

		//HACCEL hAccelTable = LoadAccelerators(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (msg.hwnd == pWnd->m_hWnd)
		{
			wchar_t wcBuffer[256] = { 0 };
			swprintf_s(wcBuffer, 256, L"message=%d\n", msg.message);
			OutputDebugStringW(wcBuffer);
			//break;
		}
		if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		pWnd->doRender();
	}

	pWnd->DestroyWindow();
	delete pWnd;
}
