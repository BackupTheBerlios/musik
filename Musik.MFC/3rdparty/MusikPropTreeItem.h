///////////////////////////////////////////////////
//
// PropTreeItem.h
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

class CMusikPropTree;
class CMusikPlaylistInfo;

///////////////////////////////////////////////////

class CMusikPropTreeItem
{
public:

	// construct / destruct
	CMusikPropTreeItem();
	virtual ~CMusikPropTreeItem();

	// each item has playlist info, including
	// id and type...
	void SetPlaylistInfo( const CMusikPlaylistInfo info );
	CMusikPlaylistInfo* GetPlaylistInfo() { return m_PlaylistInfo; }
	int GetPlaylistType();
	int GetPlaylistID();

	// treeItem states
	BOOL IsExpanded();
	BOOL IsSelected();
	BOOL IsActivated();

	// set states
	void Select(BOOL bSelect = TRUE);
	void Expand(BOOL bExpand = TRUE);
	void SetMouseOver( BOOL bMouseOver = TRUE );

	// returns TRUE if the point is on the expand button
	BOOL HitExpand(const POINT& pt);

	// returns TRUE if the item is on the root level. 
	// root level items don't have attribute areas
	BOOL IsRootLevel();

	// returns TRUE if the mouse is over the item
	BOOL IsMouseOver();

	// returns the total height of the item and 
	// all its children
	LONG GetTotalHeight();

	// Set the items label text
	void SetLabelText(LPCTSTR sLabel);

	// Return the items label text
	LPCTSTR GetLabelText();

	// Set the item's ID
	void SetCtrlID(UINT nCtrlID);

	// Return the item's ID
	UINT GetCtrlID();

	// draw the item's non attribute area
	LONG DrawItem(CDC* pDC, const RECT& rc, LONG x, LONG y);

	// call to mark attribute changes
	void CommitChanges();

	// call to activate item attribute
	void Activate();
	void Deactivate();

	//
	// Overrideables
	//

	// The attribute area needs drawing
	virtual void DrawAttribute(CDC* pDC, const RECT& rc);

	// Return the height of the item
	virtual LONG GetHeight();

	// Retrieve the item's attribute value
	virtual LPARAM GetItemValue();

	// Set the item's attribute value
	virtual void SetItemValue(LPARAM lParam);

	// Called when attribute area has changed size
	virtual void OnMove();

	// Called when the item needs to refresh its data
	virtual void OnRefresh();

	// Called when the item needs to commit its changes
	virtual void OnCommit();

	// Called to activate the item
	virtual void OnActivate();

	//
	// Usually only CMusikPropTree should calls these
	//

	void SetPropOwner(CMusikPropTree* pProp);

	// Return the location of the PropItem
	const POINT& GetLocation();

	// TreeItem link pointer access
	CMusikPropTreeItem* GetParent();
	CMusikPropTreeItem* GetSibling();
	CMusikPropTreeItem* GetChild();
	CMusikPropTreeItem* GetNextVisible();

	// misc
	void SetParent(CMusikPropTreeItem* pParent);
	void SetSibling(CMusikPropTreeItem* pSibling);
	void SetChild(CMusikPropTreeItem* pChild);
	void SetNextVisible(CMusikPropTreeItem* pVis);

protected:

	// CMusikPropTree class that this class belongs
	CMusikPropTree*			m_pProp;

	// TreeItem label name
	CString				m_sLabel;

	// TreeItem location
	CPoint				m_loc;

	// TreeItem attribute size
	CRect				m_rc;

	// user defined LPARAM value
	LPARAM				m_lParam;

	// ID of control item (should be unique)
	UINT				m_nCtrlID;

	// MUSIK SPECIFIC: item's playlist information
	CMusikPlaylistInfo*	m_PlaylistInfo;

private:
	enum TreeItemStates
	{
		TreeItemSelected =		0x00000001,
		TreeItemExpanded =		0x00000002,
		TreeItemActivated =		0x00000010 // 0x00000020 = next
	};

	// TreeItem state
	DWORD				m_dwState;

	// TRUE if the mouse is over the item
	BOOL				m_bMouseOver;

	// TRUE if item is activated
	BOOL				m_bActivated;

	// TRUE if item has been commited once (activation)
	BOOL				m_bCommitOnce;

	// Rectangle position of the expand button (if contains one)
	CRect				m_rcExpand;

	// link pointers
	CMusikPropTreeItem*		m_pParent;
	CMusikPropTreeItem*		m_pSibling;
	CMusikPropTreeItem*		m_pChild;
	CMusikPropTreeItem*		m_pVis;
};

///////////////////////////////////////////////////
