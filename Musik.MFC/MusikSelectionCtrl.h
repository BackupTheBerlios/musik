#pragma once

#include "MusikListCtrl.h"

#include "../Musik.Core/include/StdString.h"
#include "../Musik.Core/include/MusikArrays.h"

class CMusikLibrary;

class CMusikSelectionCtrl : public CMusikListCtrl
{
	DECLARE_DYNAMIC(CMusikSelectionCtrl)

public:
	CMusikSelectionCtrl( CFrameWnd* parent, CMusikLibrary* library, int type, int ctrl_id );
	virtual ~CMusikSelectionCtrl();

	int GetCtrlID(){ return m_ID; }
	void SetUpdating( bool updating = true ){ m_Updating = updating; }
	CString GetTypeDB();

protected:
	CFrameWnd* m_Parent;
	CMusikLibrary* m_Library;
	CStdStringArray m_Items;
	int m_Type;
	int m_ID;
	bool m_Updating;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
};


