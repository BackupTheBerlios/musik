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


#pragma once

#define IDS_TRUE		100
#define IDS_FALSE		101
#define IDS_NOITEMSEL	102
#define IDS_SELFORINFO	103
#define IDC_SPLITTER	1000
#define IDC_FPOINT		1001

#include "PropTreeList.h"
#include "PropTreeInfo.h"
#include "PropTreeItem.h"

class CPropTree;

typedef BOOL (CALLBACK* ENUMPROPITEMPROC)(CPropTree*, CPropTreeItem*, LPARAM);

// CPropTree window styles
#define PTS_NOTIFY				0x00000001

// CPropTree HitTest return codes
#define HTPROPFIRST					50

#define HTLABEL						(HTPROPFIRST + 0)
#define HTCOLUMN					(HTPROPFIRST + 1)
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

/////////////////////////////////////////////////////////////////////////////
// CPropTree window

class CPropTree : public CWnd
{
// Construction
public:
	CPropTree();
	virtual ~CPropTree();

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes/Operations
public:
	static CFont* GetNormalFont();
	static CFont* GetBoldFont();

	// Returns the root item of the tree
	CPropTreeItem* GetRootItem();

	// Returns the focused item or NULL for none
	CPropTreeItem* GetFocusedItem();

	// Enumerates an item and all its child items
	BOOL EnumItems(CPropTreeItem* pItem, ENUMPROPITEMPROC proc, LPARAM lParam = 0L);

	// Insert a created CPropTreeItem into the control
	CPropTreeItem* InsertItem(CPropTreeItem* pItem, CPropTreeItem* pParent = NULL);

	// Delete an item and ALL its children
	void DeleteItem(CPropTreeItem* pItem);

	// Delete all items from the tree
	void DeleteAllItems();

	// Return the splitter position
	LONG GetColumn();

	// Set the splitter position
	void SetColumn(LONG nColumn);

	// Sets the focused item
	void SetFocusedItem(CPropTreeItem* pItem);

	// Show or hide the info text
	void ShowInfoText(BOOL bShow = TRUE);

	// Returns TRUE if the item is visible (its parent is expanded)
	BOOL IsItemVisible(CPropTreeItem* pItem);

	// Ensures that an item is visible
	void EnsureVisible(CPropTreeItem* pItem);

	// do a hit test on the control (returns a HTxxxx code)
	LONG HitTest(const POINT& pt);

	// find an item by a location
	CPropTreeItem* FindItem(const POINT& pt);

	// find an item by item id
	CPropTreeItem* FindItem(UINT nCtrlID);

protected:
	// Actual tree control
	CPropTreeList	m_List;

	// Descriptive control
	CPropTreeInfo	m_Info;

	// TRUE to show info control
	BOOL			m_bShowInfo;

	// Height of the info control
	LONG			m_nInfoHeight;

	// Root level tree item
	CPropTreeItem	m_Root;

	// Linked list of visible items
	CPropTreeItem*	m_pVisbleList;

	// Pointer to the focused item (selected)
	CPropTreeItem*	m_pFocus;

	// PropTree scroll position. x = splitter position, y = vscroll position
	CPoint			m_Origin;

	// auto generated last created ID
	UINT			m_nLastUID;

	// Number of CPropTree controls in the current application
	static UINT		s_nInstanceCount;

	static CFont*	s_pNormalFont;
	static CFont*	s_pBoldFont;

	BOOL			m_bDisableInput;

	// Used for enumeration
	static CPropTreeItem*	s_pFound;

public:
	//
	// functions used by CPropTreeItem (you normally dont need to call these directly)
	//

	void AddToVisibleList(CPropTreeItem* pItem);
	void ClearVisibleList();

	void SetOriginOffset(LONG nOffset);
	void UpdatedItems();
	void UpdateMoveAllItems();
	void RefreshItems(CPropTreeItem* pItem = NULL);

	// enable or disable tree input
	void DisableInput(BOOL bDisable = TRUE);
	BOOL IsDisableInput();

	BOOL IsSingleSelection();

	CPropTreeItem* GetVisibleList();
	CWnd* GetCtrlParent();

	const POINT& GetOrigin();

	void SelectItems(CPropTreeItem* pItem, BOOL bSelect = TRUE);

	// Focus on the first visible item
	CPropTreeItem *FocusFirst();

	// Focus on the last visible item
	CPropTreeItem *FocusLast();

	// Focus on the previous item
	CPropTreeItem *FocusPrev();
	
	// Focus on the next item
	CPropTreeItem *FocusNext();

	LRESULT SendNotify(UINT nNotifyCode, CPropTreeItem* pItem = NULL);

protected:
	// Resize the child windows to fit the exact dimensions the CPropTree control
	void ResizeChildWindows(int cx, int cy);

	// Initialize global resources, brushes, fonts, etc.
	void InitGlobalResources();

	// Free global resources, brushes, fonts, etc.
	void FreeGlobalResources();

	// Recursive version of DeleteItem
	void Delete(CPropTreeItem* pItem);

// Overrides

// Implementation
private:
	static BOOL CALLBACK EnumFindItem(CPropTree* pProp, CPropTreeItem* pItem, LPARAM lParam);
	static BOOL CALLBACK EnumSelectAll(CPropTree*, CPropTreeItem* pItem, LPARAM lParam);
	static BOOL CALLBACK EnumMoveAll(CPropTree*, CPropTreeItem* pItem, LPARAM);
	static BOOL CALLBACK EnumRefreshAll(CPropTree*, CPropTreeItem* pItem, LPARAM);

	// Generated message map functions
protected:

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnSysColorChange();
	DECLARE_MESSAGE_MAP()
};
