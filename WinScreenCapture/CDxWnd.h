#pragma once
#include <afxwin.h>
#include "CDx11Render.h"

class CDxWnd :
	public CWnd
{
public:
	CDxWnd();
	virtual ~CDxWnd();

	void doRender(void);

private:
	CDx11Render m_dxRender11;
public:
	virtual BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam = NULL);
	virtual BOOL DestroyWindow();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClose();
};

