///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "MusikListCtrl.h"

#include "../Musik.Core/include/StdString.h"
#include "../Musik.Core/include/MusikArrays.h"

///////////////////////////////////////////////////

class CMusikLibrary;
class CMusikPrefs;

///////////////////////////////////////////////////

class CMusikSelectionCtrl : public CMusikListCtrl
{
	static bool m_Updating;

public:

	// construct / destruct
	CMusikSelectionCtrl( CFrameWnd* parent, CMusikLibrary* library, CMusikPrefs* prefs, int type, int ctrl_id );
	virtual ~CMusikSelectionCtrl();

	// gets
	int GetCtrlID	(){ return m_ID; }
	CString			GetTypeDB();
	CString			GetTypeStr();
	int				GetType(){ return m_Type; }
	void			GetSelItems( CStdStringArray& items, bool format_query = true );
	CStdString		GetSelQuery();

	// queries
	bool			IsItemSelected( int item );
	bool			IsParent(){ return m_ParentBox; }

	// sets
	void SetParent( bool parent = true ){ m_ParentBox = parent; }
	static void SetUpdating( bool updating = true ){ m_Updating = updating; }

	// misc
	void			RescaleColumn();
	void UpdateV( bool update_count = false );
	void UpdateV( CStdString query, bool update_count = false );

	// message maps
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();

protected:

	// inits
	void InitFonts();

	// core
	CFrameWnd* m_Parent;
	CMusikLibrary* m_Library;
	CStdStringArray m_Items;
	int m_Type;
	int m_ID;
	bool m_ParentBox;

	// prefs
	CMusikPrefs *m_Prefs;

	// fonts
	CFont m_Regular;
	CFont m_Bold;

	// macros
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CMusikSelectionCtrl)
};

///////////////////////////////////////////////////

