///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2003
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey@bak.rr.com
//   Dustin Carter  : Developer, pain in the ass
//   Simon Windmill : Developer, the most helpful person thats immediately available ;)
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Class(es): 
//
//   CmusikSelectionCtrl, CmusikSelectionBar
//
// Filename(s): 
//
//   musikSelectionCtrl.h, musikSelectionCtrl.cpp
//
// Information:
//
//   musikCube's artist / album / genre / etc selection control
// 
// Usage: 
//
//   Void.
//
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
	friend class CmusikSelectionCtrl;

public:

	// construct / destruct
	CmusikSelectionBar( CFrameWnd* parent, CmusikLibrary* library, CmusikPrefs* prefs, int type, int ctrlid, UINT dropid_l, UINT dropid_r );
	virtual ~CmusikSelectionBar();

	// gets
	CmusikSelectionCtrl* GetCtrl(){ return m_wndChild; }

	// mfc message maps
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelectionboxRename();
	afx_msg void OnSelectionboxRemove();
	afx_msg void OnSelectionboxAddnew();
	afx_msg void OnChangetypeArtist();
	afx_msg void OnChangetypeAlbum();
	afx_msg void OnChangetypeYear();
	afx_msg void OnChangetypeGenre();
	afx_msg void OnChangetypeTracknumber();
	afx_msg void OnChangetypeTimeadded();
	afx_msg void OnChangetypeLastplayed();
	afx_msg void OnChangetypeFormat();
	afx_msg void OnChangetypeRating();
	afx_msg void OnChangetypeTimesplayed();
	afx_msg void OnChangetypeBitrate();

	// misc
	void ShowOptionsMenu();
	void ShowContextMenu();
	void ReqSelBoxUpdate();

	// showing options menu
	virtual void OnOptions();

protected:

	// child
	CmusikSelectionCtrl* m_wndChild;
	CFont m_Font;

	// macros
	DECLARE_DYNAMIC(CmusikSelectionBar)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDeleteFromlibrary();
	afx_msg void OnDeleteFromcomputer();
};

///////////////////////////////////////////////////

// CmusikSelectionCtrl

///////////////////////////////////////////////////

class CmusikSelectionCtrl : public CmusikListCtrl
{
	friend class CmusikSelectionBar;
	static bool m_Updating;
	static int m_Count;

public:

	// construct / destruct
	CmusikSelectionCtrl( CFrameWnd* parent, CmusikLibrary* library, CmusikPrefs* prefs, int type, int ctrlid, UINT dropid_l, UINT dropid_r );
	virtual ~CmusikSelectionCtrl();

	// gets
	int GetCtrlID (){ return m_ID; }
	CString GetTypeDB();
	CString GetTypeStr();
	int	GetType(){ return m_Type; }
	void GetSelItems( CmusikStringArray& items, bool format_query = true );
	CmusikString GetSelQuery();
	CmusikString GetEditCommitStr();

	// sets
	void SetParent( bool parent = true ){ m_ParentBox = parent; }
	static void SetUpdating( bool updating = true ){ m_Updating = updating; }
	void SetType( int type, bool update = true );

	// order
	int GetChildOrder() { return m_ChildOrder; }
	void ResetOrder() { m_ChildOrder = -1; }

	static void IncChildOrder() { m_Count++; }
	static void ResetChildOrder() { m_Count = -1; }

	// misc
	void RescaleColumn();
	void UpdateV();
	void UpdateV( CmusikString query );
	void RenameSel();
	void DelSel( bool from_file = false );

	// queries
	bool IsItemSelected( int item );
	bool IsParent(){ return m_ParentBox; }

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
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBeginrdrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydown(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	// custom message maps
	LRESULT OnEditCommit( WPARAM wParam, LPARAM lParam );
	LRESULT OnEditCancel( WPARAM wParam, LPARAM lParam );

	// custom message maps
	afx_msg LRESULT OnMouseLeave( WPARAM wParam, LPARAM lParam );

protected:

	int m_ChildOrder;

	// misc
	void InitFonts();
	void SetWindowCap();
	bool IsPlaylistSel();

	// drag / drop id
	bool m_IsWinNT;
	UINT m_DropID_L;
	UINT m_DropID_R;
	void BeginDrag( bool right_button );

	bool m_IsFocused;
	bool m_Updated;

	// bg color if child
	void InitColors();
	COLORREF clrChild;

	// F2...
	CmusikEditInPlace m_EditInPlace;
	CmusikString m_CommitStr;

	// core
	CFrameWnd* m_Parent;
	CmusikLibrary* m_Library;
	CmusikStringArray m_Items;
	int m_Type;
	int m_ID;
	bool m_ParentBox;

	CmusikPrefs *m_Prefs;

	// fonts
	CFont m_Regular;
	CFont m_Bold;
	CFont m_StarFont;

	// macros
	DECLARE_DYNAMIC(CmusikSelectionCtrl)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

