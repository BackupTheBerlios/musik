#pragma once

#include "MusikListCtrl.h"

class CMusikLibrary;

class CMusikSelectionCtrl : public CMusikListCtrl
{
	DECLARE_DYNAMIC(CMusikSelectionCtrl)

public:
	CMusikSelectionCtrl( CMusikLibrary* library, int type );
	virtual ~CMusikSelectionCtrl();

protected:
	CMusikLibrary* m_Library;
	int m_Type;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


