#pragma once

#include "MusikListCtrl.h"

#include "../Musik.Core/include/StdString.h"
#include "../Musik.Core/include/MusikArrays.h"

class CMusikLibrary;

class CMusikSelectionCtrl : public CMusikListCtrl
{
	DECLARE_DYNAMIC(CMusikSelectionCtrl)

public:
	CMusikSelectionCtrl( CFrameWnd* parent, CMusikLibrary* library, int type );
	virtual ~CMusikSelectionCtrl();

protected:
	CFrameWnd* m_Parent;
	CMusikLibrary* m_Library;
	CStdStringArray m_Items;
	int m_Type;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
};


