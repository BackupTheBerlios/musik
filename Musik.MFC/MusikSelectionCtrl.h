#pragma once

#include "MusikListCtrl.h"

class CMusikSelectionCtrl : public CMusikListCtrl
{
	DECLARE_DYNAMIC(CMusikSelectionCtrl)

public:
	CMusikSelectionCtrl();
	virtual ~CMusikSelectionCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


