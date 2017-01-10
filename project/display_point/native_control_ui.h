#pragma once
#include "stdafx.h"

class NativeControlUI : public CControlUI
{
public:
	NativeControlUI() : m_idControl(0) {};
	~NativeControlUI() {};

public:		// ´æÈ¡Æ÷
	PCTSTR GetClass() const override { return _T("NativeControlUI"); }
	void SetID(int id) { m_idControl = id; }
	UINT GetID() const { return m_idControl; }

public:
	virtual LPVOID GetInterface(LPCTSTR pstrName) override;
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
	virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;
	virtual void SetVisible(bool bVisible) override;
	virtual void SetInternVisible(bool bVisible = true) override;

protected:
	UINT m_idControl;
};

