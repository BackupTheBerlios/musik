///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikListCtrl.h"
#include "musikEditInPlace.h"
#include "musikDockBar.h"

#include "../musikCore/include/StdString.h"
#include "../musikCore/include/musikArrays.h"

///////////////////////////////////////////////////

class CmusikLibrary;
class CmusikPrefs;
class CmusikSelectionCtrl;

///////////////////////////////////////////////////

// CmusikSelectionBar

///////////////////////////////////////////////////

#ifndef baseCmusikSelectionBar
	#define baseCmusikSelectionBar CmusikDockBar
#endif

///////////////////////////////////////////////////

class CmusikSelectionBar : public baseCmusikSelectionBar
{
public:

	// construct / destruct
	CmusikSelectionBar( CFrameWnd* parent, CmusikLibrary* library, CmusikPrefs* prefs, int type, int ctrlid, int dropid );
	virtual ~CmusikSelectionBar();

	// gets
	CmusikSelectionCtrl* GetCtrl(){ return m_wndChild; }

	// showing options menu
	virtual void OnOptions();

protected:

	// child
	CmusikSelectionCtrl* m_wndChild;
	CFont m_Font;

	// mfc message maps
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize(UINT nType, int cx, int cy);

	// macros
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

// CmusikSelectionCtrl

///////////////////////////////////////////////////

class CmusikSelectionCtrl : public CmusikListCtrl
{
	static bool m_Updating;

public:

	// construct / destruct
	CmusikSelectionCtrl( CFrameWnd* parent, CmusikLibrary* library, CmusikPrefs* prefs, int type, int ctrlid, int dropid );
	virtual ~CmusikSelectionCtrl();

	// gets
	int GetCtrlID (){ return m_ID; }
	CString GetTypeDB();
	CString GetTypeStr();
	int	GetType(){ return m_Type; }
	void GetSelItems( CStdStringArray& items, bool format_query = true );
	CStdString GetSelQuery();
	CStdString GetEditCommitStr();

	// sets
	void SetParent( bool parent = true ){ m_ParentBox = parent; }
	static void SetUpdating( bool updating = true ){ m_Updating = updating; }

	// misc
	void RescaleColumn();
	void UpdateV( bool update_count = false );
	void UpdateV( CStdString query, bool update_count = false );

	// queries
	bool IsItemSelected( int item );
	bool IsParent(){ return m_ParentBox; }

	// showing the dropdown menu
	void ShowMenu();

	// mfc message maps
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLvnMarqueeBegin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	// custom message maps
	LRESULT OnEditCommit( WPARAM wParam, LPARAM lParam );
	LRESULT OnEditCancel( WPARAM wParam, LPARAM lParam );

	// custom message maps
	afx_msg LRESULT OnMouseLeave( WPARAM wParam, LPARAM lParam );

protected:

	// inits
	void InitFonts();

	// drag / drop id
	bool m_IsWinNT;
	UINT m_DropID;

	// tracking mouse
	bool m_MouseTrack;

	// F2...
	CmusikEditInPlace m_EditInPlace;
	CStdString m_CommitStr;

	// core
	CFrameWnd* m_Parent;
	CmusikLibrary* m_Library;
	CStdStringArray m_Items;
	int m_Type;
	int m_ID;
	bool m_ParentBox;

	// prefs
	CmusikPrefs *m_Prefs;

	// fonts
	CFont m_Regular;
	CFont m_Bold;

	// macros
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CmusikSelectionCtrl)
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

///////////////////////////////////////////////////

