#include "stdafx.h"
#include "CDxWnd.h"


CDxWnd::CDxWnd()
{
}


CDxWnd::~CDxWnd()
{
}

void CDxWnd::doRender(void)
{
	m_dxRender11.render();
}


BOOL CDxWnd::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	BOOL bRet = CWnd::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, lpParam);
	m_dxRender11.init(m_hWnd, 1280, 720);
	return bRet;
}


BOOL CDxWnd::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_dxRender11.uninit();
	return CWnd::DestroyWindow();
}
BEGIN_MESSAGE_MAP(CDxWnd, CWnd)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


void CDxWnd::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CWnd::OnClose();
}
