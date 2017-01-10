#include "native_control_ui.h"


LPVOID NativeControlUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcscmp(pstrName, _T("NativeControl")) == 0)
		return static_cast<NativeControlUI *>(this);

	return CControlUI::GetInterface(pstrName);
}

void NativeControlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcscmp(pstrName, _T("id")) == 0)
		m_idControl = _tstol(pstrValue);

	CControlUI::SetAttribute(pstrName, pstrValue);
}

void NativeControlUI::SetPos(RECT rc, bool bNeedInvalidate/* = true*/)
{
	CControlUI::SetPos(rc);
	CPaintManagerUI* pm = GetManager();
	if (pm)
	{
		HWND hwnd = ::GetDlgItem(pm->GetPaintWindow(), m_idControl);
		if (hwnd)
			::SetWindowPos(hwnd, 0, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, \
				m_rcItem.bottom - m_rcItem.top, SWP_NOZORDER);
	}
}

void NativeControlUI::SetVisible(bool bVisible)
{
	CPaintManagerUI* pm = GetManager();
	if (pm)
	{
		HWND hwnd = ::GetDlgItem(pm->GetPaintWindow(), m_idControl);
		if (hwnd)
		{
			UINT flag = SWP_NOZORDER | (bVisible ? SWP_SHOWWINDOW : SWP_HIDEWINDOW);
			::SetWindowPos(hwnd, 0, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left,
				m_rcItem.bottom - m_rcItem.top, flag);
		}
	}
	CControlUI::SetVisible(bVisible);
}

void NativeControlUI::SetInternVisible(bool bVisible /* = ture */)
{
	CPaintManagerUI* pm = GetManager();
	if (pm)
	{
		HWND hwnd = GetDlgItem(pm->GetPaintWindow(), m_idControl);
		if (hwnd)
		{
			UINT flag = SWP_NOZORDER | (bVisible ? SWP_SHOWWINDOW : SWP_HIDEWINDOW);
			::SetWindowPos(hwnd, 0, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left,
				m_rcItem.bottom - m_rcItem.top, flag);
		}
	}
	CControlUI::SetInternVisible(bVisible);
}