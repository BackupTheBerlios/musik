///////////////////////////////////////////////////
// PropTree.h : header file
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
//  Modified by Casey Langen for Musik MFC port...
//  Rather than linking to a dll or external lib, it compiles in.
//  Changes default look and behavior as well. Much thanks to the
//  Author Scott Ramsay.
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#define IDS_TRUE		100
#define IDS_FALSE		101
#define IDS_NOITEMSEL	102
#define IDS_SELFORINFO	103
#define IDC_SPLITTER	1000
#define IDC_FPOINT		1001

///////////////////////////////////////////////////

#include "PropTreeList.h"
#include "PropTreeItem.h"

///////////////////////////////////////////////////

class CPropTree;

///////////////////////////////////////////////////

typedef BOOL (CALLBACK* ENUMPROPITEMPROC)(CPropTree*, CPropTreeItem*, LPARAM);

// CPropTree window styles
#define PTS_NOTIFY					0x00000001

// CPropTree HitTest return codes
#define HTPROPFIRST					50
#define HTLABEL						(HTPROPFIRST + 0)
#define HTEXPAND					(HTPROPFIRST + 2)
#define HTATTRIBUTE					(HTPROPFIRST + 3)

// CPropTree WM_NOTIFY notification structure
typedef struct _NMPROPTREE
{
    NMHDR			hdr;
	CPropTreeItem*	pItem;
} NMPROPTREE, *PNMPROPTREE, FAR *LPNMPROPTREE;

// CPropTree specific Notification Codes
#define PTN_FIRST					(0U-1100U)
#define PTN_INSERTITEM				(PTN_FIRST-1)
#define PTN_DELETEITEM				(PTN_FIRST-2)
#define PTN_DELETEALLITEMS			(PTN_FIRST-3)
#define PTN_ITEMCHANGED				(PTN_FIRST-5)
#define PTN_ITEMBUTTONCLICK			(PTN_FIRST-6)
#define PTN_SELCHANGE				(PTN_FIRST-7)
#define PTN_ITEMEXPANDING			(PTN_FIRST-8)
#define PTN_COLUMNCLICK				(PTN_FIRST-9)
#define PTN_PROPCLICK				(PTN_FIRST-10)
#define PTN_CHECKCLICK				(PTN_FIRST-12)

///////////////////////////////////////////////////

class CPropTree : public CWnd
{

public:
	// construct / destruct
	CPropTree();
	virtual ~CPropTree();

	// overrides
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	// fonts
	static CFont* GetNormalFont();
	static CFont* GetBoldFont();

	// items
	CPropTreeItem* GetRootItem();
	CPropTreeItem* GetFocusedItem();
	CPropTreeItem* InsertItem(CPropTreeItem* pItem, CPropTreeItem* pParent = NULL);
	void DeleteItem(CPropTreeItem* pItem);
	void DeleteAllItems();
	void SetFocusedItem(CPropTreeItem* pItem);
	BOOL IsItemVisible(CPropTreeItem* pItem);
	void EnsureVisible(CPropTreeItem* pItem);
	CPropTreeItem* FindItem(const POINT& pt);
	CPropTreeItem* FindItem(UINT nCtrlID);

	// functions used by CPropTreeItem...
	// should not call these directly
	void AddToVisibleList(CPropTreeItem* pItem);
	void ClearVisibleList();
	void SetOriginOffset(LONG nOffset);
	void UpdatedItems();
	void UpdateMoveAllItems();
	void RefreshItems(CPropTreeItem* pItem = NULL);
	void SelectItems(CPropTreeItem* pItem, BOOL bSelect = TRUE);

	// misc
	BOOL IsSingleSelection();
	CPropTreeItem* GetVisibleList();
	CWnd* GetCtrlParent();
	const POINT& GetOrigin();

	// item focus controls
	CPropTreeItem *FocusFirst();
	CPropTreeItem *FocusLast();
	CPropTreeItem *FocusPrev();
	CPropTreeItem *FocusNext();

	// columns
	LONG GetColumn();
	void SetColumn(LONG nColumn);

	// Enumerates an item and all its child items
	BOOL EnumItems(CPropTreeItem* pItem, ENUMPROPITEMPROC proc, LPARAM lParam = 0L);

	// Show or hide the info text
	void ShowInfoText(BOOL bShow = TRUE);

	// do a hit test on the control (returns a HTxxxx code)
	LONG HitTest(const POINT& pt);

	// do a hit test and return the current item...
	CPropTreeItem* HitTestEx(const POINT& pt);

	// enable or disable tree input
	void DisableInput(BOOL bDisable = TRUE);
	BOOL IsDisableInput();

	LRESULT SendNotify(UINT nNotifyCode, CPropTreeItem* pItem = NULL);

protected:

	// resize the child windows to fit the exact 
	// dimensions the CPropTree control
	void ResizeChildWindows(int cx, int cy);

	// Initialize global resources, brushes, 
	// fonts, etc.
	void InitGlobalResources();

	// Free global resources, brushes, fonts, etc.
	void FreeGlobalResources();

	// Recursive version of DeleteItem
	void Delete(CPropTreeItem* pItem);

	// Actual tree control
	CPropTreeList	m_List;

	// TRUE to show info control
	BOOL m_bShowInfo;

	// Root level tree item
	CPropTreeItem m_Root;

	// Linked list of visible items
	CPropTreeItem* m_pVisbleList;

	// Pointer to the focused item (selected)
	CPropTreeItem* m_pFocus;

	// PropTree scroll position. x = splitter position, y = vscroll position
	CPoint m_Origin;

	// auto generated last created ID
	UINT m_nLastUID;

	// Number of CPropTree controls in the current application
	static UINT s_nInstanceCount;

	// fonts
	static CFont* s_pNormalFont;
	static CFont* s_pBoldFont;

	BOOL m_bDisableInput;

	// Used for enumeration
	static CPropTreeItem*	s_pFound;

	// message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnSysColorChange();

	// macros
	DECLARE_MESSAGE_MAP()

private:

	// callbacks
	static BOOL CALLBACK EnumFindItem(CPropTree* pProp, CPropTreeItem* pItem, LPARAM lParam);
	static BOOL CALLBACK EnumSelectAll(CPropTree*, CPropTreeItem* pItem, LPARAM lParam);
	static BOOL CALLBACK EnumMoveAll(CPropTree*, CPropTreeItem* pItem, LPARAM);
	static BOOL CALLBACK EnumRefreshAll(CPropTree*, CPropTreeItem* pItem, LPARAM);
};

///////////////////////////////////////////////////
