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
	// TODO: �ڴ����ר�ô����/����û���
	BOOL bRet = CWnd::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, lpParam);
	m_dxRender11.init(m_hWnd, 1280, 720);
	return bRet;
}


BOOL CDxWnd::DestroyWindow()
{
	// TODO: �ڴ����ר�ô����/����û���
	m_dxRender11.uninit();
	return CWnd::DestroyWindow();
}
BEGIN_MESSAGE_MAP(CDxWnd, CWnd)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


void CDxWnd::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CWnd::OnClose();
}
