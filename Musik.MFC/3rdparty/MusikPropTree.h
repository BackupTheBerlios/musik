///////////////////////////////////////////////////
//
// PropTree.h
//
//  Copyright (C) 1998-2001 Scott Ramsay
//	sramsay@gonavi.com
//	http://www.gonavi.com
//
//  This material is provided "as is", with absolutely no warranty expressed
//  or implied. Any use is at your own risk.
// 
//  Permission to use or copy this software for any purpose is hereby granted 
//  without fee, provided the above notices are retained on all copies.
//  Permission to modify the code and to distribute modified code is granted,
//  provided the above notices are retained, and a notice that the code was
//  modified is included with the above copyright notice.
// 
//	If you use this code, drop me an email.  I'd like to know if you find the code
//	useful.
//
//  Modified heavily by Casey Langen for Musik MFC port...
//    - Rather than linking to a dll or external lib, it compiles in.
//    - PropTree and PropTreeList combined into one file
//    - Mouse hover tracking
//    - Drawing enhancements
//    - Various Musik specific functions
//
//  The Musik team thanks Scott Ramsay for creating this great
//  class and making it open source. Cheers.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "MusikPropTreeItem.h"

#include "../Musik.Core/include/MusikLibrary.h"

///////////////////////////////////////////////////

class CMusikPropTree;
class CMusikPropTreeItem;

class CMusikLibrary;
class CMusikPrefs;

///////////////////////////////////////////////////

enum 
{
	MUSIK_SOURCES_TYPE_NOWPLAYING = MUSIK_PLAYLIST_TYPE_COUNT,
	MUSIK_SOURCES_TYPE_LIBRARY
};

///////////////////////////////////////////////////

typedef BOOL (CALLBACK* ENUMPROPITEMPROC)(CMusikPropTree*, CMusikPropTreeItem*, LPARAM);

///////////////////////////////////////////////////

// CMusikPropTree window styles

#define PTS_NOTIFY					0x00000001

///////////////////////////////////////////////////

// CMusikPropTree HitTest return codes

#define HTPROPFIRST					50
#define HTLABEL						(HTPROPFIRST + 0)
#define HTEXPAND					(HTPROPFIRST + 2)
#define HTATTRIBUTE					(HTPROPFIRST + 3)

///////////////////////////////////////////////////

// CMusikPropTree WM_NOTIFY notification structure

typedef struct _NMPROPTREE
{
    NMHDR			hdr;
	CMusikPropTreeItem*	pItem;
} NMPROPTREE, *PNMPROPTREE, FAR *LPNMPROPTREE;

///////////////////////////////////////////////////

// CMusikPropTree specific Notification Codes

#define PTN_FIRST					(0U-1100U)
#define PTN_INSERTITEM				(PTN_FIRST-1)
#define PTN_DELETEITEM				(PTN_FIRST-2)
#define PTN_DELETEALLITEMS			(PTN_FIRST-3)
#define PTN_ITEMCHANGED				(PTN_FIRST-4)
#define PTN_ITEMBUTTONCLICK			(PTN_FIRST-5)
#define PTN_SELCHANGE				(PTN_FIRST-6)
#define PTN_ITEMEXPANDING			(PTN_FIRST-7)
#define PTN_PROPCLICK				(PTN_FIRST-8)

///////////////////////////////////////////////////

class CMusikPropTree : public CWnd
{
	// so they can access the musik stuff
	friend class CMusikPropTreeItem;

public:

	// construct / destruct
	CMusikPropTree( CMusikPrefs* prefs, CMusikLibrary* library, UINT dropid );
	virtual ~CMusikPropTree();

	// overrides
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	// fonts
	static CFont* GetNormalFont();
	static CFont* GetBoldFont();

	// item gets
	CMusikPropTreeItem* GetRootItem();
	CMusikPropTreeItem* GetFocusedItem();
	CMusikPropTreeItem* GetHoveredItem();

	// item sets
	void SetFocusedItem(CMusikPropTreeItem* pItem);
	void SetHoveredItem(CMusikPropTreeItem* pItem);

	// items actions
	CMusikPropTreeItem* InsertItem(CMusikPropTreeItem* pItem, CMusikPropTreeItem* pParent = NULL);
	void DeleteItem(CMusikPropTreeItem* pItem);
	void DeleteAllItems();
	void EnsureVisible(CMusikPropTreeItem* pItem);
	virtual CMusikPropTreeItem* FindItem(const POINT& pt);
	CMusikPropTreeItem* FindItem(UINT nCtrlID);

	// item checks
	BOOL IsItemVisible(CMusikPropTreeItem* pItem);

	// functions used by CMusikPropTreeItem...
	// should not call these directly
	void AddToVisibleList(CMusikPropTreeItem* pItem);
	void ClearVisibleList();
	void SetOriginOffset(LONG nOffset);
	void UpdatedItems();
	void UpdateMoveAllItems();
	void RefreshItems(CMusikPropTreeItem* pItem = NULL);
	void SelectItems(CMusikPropTreeItem* pItem, BOOL bSelect = TRUE);

	// misc
	CMusikPropTreeItem* GetVisibleList();
	const int& GetOrigin();
	void UpdateResize();

	// item focus controls
	CMusikPropTreeItem *FocusFirst();
	CMusikPropTreeItem *FocusLast();
	CMusikPropTreeItem *FocusPrev();
	CMusikPropTreeItem *FocusNext();

	// Enumerates an item and all its child items
	BOOL EnumItems(CMusikPropTreeItem* pItem, ENUMPROPITEMPROC proc, LPARAM lParam = 0L);

	// do a hit test on the control 
	// HitTest returns a HTxxxx code
	// HitTextEx returns the item
	LONG HitTest(const POINT& pt);
	CMusikPropTreeItem* HitTestEx(const POINT& pt);

	// non-mfc or custom messages
	LRESULT SendNotify(UINT nNotifyCode, CMusikPropTreeItem* pItem = NULL);

	// mfc message maps
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

protected:

	// musik prefs for themes
	// musik library for dnd playlists
	CMusikPrefs* m_Prefs;
	CMusikLibrary* m_Library;

	// drag and drop related stuff
	UINT m_DropID;
	bool m_IsWinNT;

	// bitmap back buffer for flicker free drawing
	CBitmap	m_BackBuffer;
	CSize m_BackBufferSize;

	// Initialize global resources, brushes, 
	// fonts, etc.
	void InitGlobalResources();

	// Free global resources, brushes, fonts, etc.
	void FreeGlobalResources();

	// Recursive version of DeleteItem
	void Delete(CMusikPropTreeItem* pItem);

	// Root level tree item
	CMusikPropTreeItem m_Root;

	// Linked list of visible items
	CMusikPropTreeItem* m_pVisbleList;

	// Pointer to the focused item (selected)
	CMusikPropTreeItem* m_pFocus;

	// Pointer to hovered item (mouse over)
	CMusikPropTreeItem* m_pHovered;

	// PropTree scroll position
	int m_ScrollPos;

	// auto generated last created ID
	UINT m_nLastUID;

	// Number of CMusikPropTree controls in the current application
	static UINT s_nInstanceCount;

	// fonts
	static CFont* s_pNormalFont;
	static CFont* s_pBoldFont;

	// Used for enumeration
	static CMusikPropTreeItem*	s_pFound;

	// misc internal functions
	void RecreateBackBuffer(int cx, int cy);
	void CheckVisibleFocus();

	// protected mfc message maps
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnSysColorChange();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();

	// macros
	DECLARE_MESSAGE_MAP()

private:

	// callbacks
	static BOOL CALLBACK EnumFindItem(CMusikPropTree* pProp, CMusikPropTreeItem* pItem, LPARAM lParam);
	static BOOL CALLBACK EnumSelectAll(CMusikPropTree*, CMusikPropTreeItem* pItem, LPARAM lParam);
	static BOOL CALLBACK EnumMoveAll(CMusikPropTree*, CMusikPropTreeItem* pItem, LPARAM);
	static BOOL CALLBACK EnumRefreshAll(CMusikPropTree*, CMusikPropTreeItem* pItem, LPARAM);
};

///////////////////////////////////////////////////
