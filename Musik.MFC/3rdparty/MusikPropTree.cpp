///////////////////////////////////////////////////
//
// PropTree.cpp
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

#include "stdafx.h"
#include "MusikPropTree.h"

#include "../MusikPrefs.h"

#include "../MEMDC.H"
#include ".\musikproptree.h"

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////

#pragma warning (disable : 4311) // pointer truncation from 'HMENU' to 'UINT'
#pragma warning (disable : 4312) // conversion from 'DWORD' to 'HBRUSH' of greater size

///////////////////////////////////////////////////

static const CString strOfficeFontName	= _T("Tahoma");
static const CString strDefaultFontName = _T("MS Sans Serif");

///////////////////////////////////////////////////

HINSTANCE ghInst;

///////////////////////////////////////////////////

static int CALLBACK FontFamilyProcFonts(const LOGFONT FAR* lplf, const TEXTMETRIC FAR*, ULONG, LPARAM)
{
	ASSERT(lplf != NULL);
	CString strFont = lplf->lfFaceName;
	return strFont.CollateNoCase (strOfficeFontName) == 0 ? 0 : 1;
}

///////////////////////////////////////////////////

UINT CMusikPropTree::s_nInstanceCount;
CFont* CMusikPropTree::s_pNormalFont;
CFont* CMusikPropTree::s_pBoldFont;
CMusikPropTreeItem* CMusikPropTree::s_pFound;

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMusikPropTree, CWnd)
	ON_WM_ENABLE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_VSCROLL()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

CMusikPropTree::CMusikPropTree( CMusikPrefs* prefs, CMusikLibrary* library, UINT dropid ) :
	m_pVisbleList(NULL),
	m_ScrollPos(0),
	m_nLastUID(1),
	m_pFocus(NULL),
	m_pHovered(NULL),
	m_BackBufferSize(0,0)
{
	m_Root.Expand();

	// prefs
	m_Prefs = prefs;
	m_Library = library;

	// mouse track
	m_MouseTrack = false;
	m_LockHover	= false;
	m_Count = 0;
	m_LastPos.m_Top = -1;
	m_LastPos.m_Bottom = -1;

	// dropid
	m_DropID = dropid;
	m_IsWinNT = ( 0 == ( GetVersion() & 0x80000000 ) );

	// init global resources only once
	if (!s_nInstanceCount)
		InitGlobalResources();

	s_nInstanceCount++;
}

///////////////////////////////////////////////////

CMusikPropTree::~CMusikPropTree()
{
	DeleteAllItems();

	s_nInstanceCount--;

	// free global resource when ALL CMusikPropTrees are destroyed
	if (!s_nInstanceCount)
		FreeGlobalResources();
}

///////////////////////////////////////////////////

const int& CMusikPropTree::GetOrigin()
{
	return m_ScrollPos;
}

///////////////////////////////////////////////////

BOOL CMusikPropTree::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CWnd* pWnd = this;

	LPCTSTR pszCreateClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW));

	return pWnd->Create(pszCreateClass, _T(""), dwStyle, rect, pParentWnd, nID);
}

///////////////////////////////////////////////////

void CMusikPropTree::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	RecreateBackBuffer(cx, cy);

	UpdateResize();
	Invalidate();
	UpdateWindow();

	// inform all items that a resize has been made
	UpdateMoveAllItems();
}

///////////////////////////////////////////////////

void CMusikPropTree::InitGlobalResources()
{
	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);

	LOGFONT lf;
	memset(&lf, 0, sizeof (LOGFONT));

	CWindowDC dc(NULL);
	lf.lfCharSet = (BYTE)GetTextCharsetInfo(dc.GetSafeHdc(), NULL, 0);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	// check if we should use system font
	_tcscpy(lf.lfFaceName, info.lfMenuFont.lfFaceName);

	BOOL fUseSystemFont = (info.lfMenuFont.lfCharSet > SYMBOL_CHARSET);
	if (!fUseSystemFont)
	{
		// check for "Tahoma" font existance:
		if (::EnumFontFamilies(dc.GetSafeHdc(), NULL, FontFamilyProcFonts, 0)==0)
		{
			// Found! Use MS Office font!
			_tcscpy(lf.lfFaceName, strOfficeFontName);
		}
		else
		{
			// Not found. Use default font:
			_tcscpy(lf.lfFaceName, strDefaultFontName);
		}
	}

	s_pNormalFont = new CFont;
	s_pNormalFont->CreateFontIndirect(&lf);

	lf.lfWeight = FW_BOLD;
	s_pBoldFont = new CFont;
	s_pBoldFont->CreateFontIndirect(&lf);
}

///////////////////////////////////////////////////

void CMusikPropTree::FreeGlobalResources()
{
	if (s_pNormalFont)
	{
		delete s_pNormalFont;
		s_pNormalFont = NULL;
	}

	if (s_pBoldFont)
	{
		delete s_pBoldFont;
		s_pBoldFont = NULL;
	}
}

///////////////////////////////////////////////////

CFont* CMusikPropTree::GetNormalFont()
{
	return s_pNormalFont;
}

///////////////////////////////////////////////////

CFont* CMusikPropTree::GetBoldFont()
{
	return s_pBoldFont;
}

///////////////////////////////////////////////////

CMusikPropTreeItem* CMusikPropTree::GetFocusedItem()
{
	return m_pFocus;
}

///////////////////////////////////////////////////

CMusikPropTreeItem* CMusikPropTree::GetHoveredItem()
{
	return m_pHovered;
}

///////////////////////////////////////////////////

CMusikPropTreeItem* CMusikPropTree::GetRootItem()
{
	return &m_Root;
}

///////////////////////////////////////////////////

void CMusikPropTree::ClearVisibleList()
{
	m_pVisbleList = NULL;
}

///////////////////////////////////////////////////

CMusikPropTreeItem* CMusikPropTree::GetVisibleList()
{
	return m_pVisbleList;
}

///////////////////////////////////////////////////

void CMusikPropTree::AddToVisibleList(CMusikPropTreeItem* pItem)
{
	if (!pItem)
		return;

	// check for an empty visible list
	if (!m_pVisbleList)
		m_pVisbleList = pItem;
	else
	{
		// Add the new item to the end of the list
		CMusikPropTreeItem* pNext;

		pNext = m_pVisbleList;
		while (pNext->GetNextVisible())
			pNext = pNext->GetNextVisible();

		pNext->SetNextVisible(pItem);
	}

	pItem->SetNextVisible(NULL);
}

///////////////////////////////////////////////////

BOOL CMusikPropTree::EnumItems(CMusikPropTreeItem* pItem, ENUMPROPITEMPROC proc, LPARAM lParam)
{
	if (!pItem || !proc)
		return FALSE;

	CMusikPropTreeItem* pNext;

	// don't count the root item in any enumerations
	if (pItem!=&m_Root && !proc(this, pItem, lParam))
		return FALSE;

	// recurse thru all child items
	pNext = pItem->GetChild();

	while (pNext)
	{
		if (!EnumItems(pNext, proc, lParam))
			return FALSE;

		pNext = pNext->GetSibling();
	}

	return TRUE;
}

///////////////////////////////////////////////////

void CMusikPropTree::SetOriginOffset(LONG nOffset)
{
	m_ScrollPos = nOffset;
}

///////////////////////////////////////////////////
	
void CMusikPropTree::UpdatedItems()
{
	if (!IsWindow(m_hWnd))
		return;

	Invalidate();

	UpdateResize();
	Invalidate();
}

///////////////////////////////////////////////////

void CMusikPropTree::DeleteAllItems()
{
	Delete(NULL);
	UpdatedItems();
	m_nLastUID = 1; // reset uid counter
	m_Count = 0;
}

///////////////////////////////////////////////////

void CMusikPropTree::DeleteItem(CMusikPropTreeItem* pItem)
{
	Delete(pItem);
	UpdatedItems();
}

///////////////////////////////////////////////////

void CMusikPropTree::Delete(CMusikPropTreeItem* pItem)
{
	if (pItem && pItem!=&m_Root && SendNotify(PTN_DELETEITEM, pItem))
		return;

	// passing in a NULL item is the same as calling DeleteAllItems
	if (!pItem)
	{
		pItem = &m_Root;
		m_Count = 0;
	}
	else 
		m_Count--;

	// delete children

	CMusikPropTreeItem* pIter;
	CMusikPropTreeItem* pNext;

	pIter = pItem->GetChild();
	while (pIter)
	{
		pNext = pIter->GetSibling();
		DeleteItem(pIter);
		pIter = pNext;
	}

	// unlink from tree
	if (pItem->GetParent())
	{
		if (pItem->GetParent()->GetChild()==pItem)
			pItem->GetParent()->SetChild(pItem->GetSibling());
		else
		{
			pIter = pItem->GetParent()->GetChild();
			while (pIter->GetSibling() && pIter->GetSibling()!=pItem)
				pIter = pIter->GetSibling();

			if (pIter->GetSibling())
				pIter->SetSibling(pItem->GetSibling());
		}
	}

	if (pItem!=&m_Root)
	{
		if (pItem==GetFocusedItem())
			SetFocusedItem(NULL);
		delete pItem;
	}
}

///////////////////////////////////////////////////

void CMusikPropTree::SetFocusedItem(CMusikPropTreeItem* pItem)
{
	m_pFocus = pItem;
	//EnsureVisible(m_pFocus);

	if (!IsWindow(m_hWnd))
		return;

	Invalidate();
}

///////////////////////////////////////////////////

void CMusikPropTree::SetHoveredItem(CMusikPropTreeItem* pItem)
{
	m_pHovered = pItem;
	EnsureVisible(m_pHovered);

	if (!IsWindow(m_hWnd))
		return;

	Invalidate();
}

///////////////////////////////////////////////////

BOOL CMusikPropTree::IsItemVisible(CMusikPropTreeItem* pItem)
{
	if (!pItem)
		return FALSE;

	for (CMusikPropTreeItem* pNext = m_pVisbleList; pNext; pNext = pNext->GetNextVisible())
	{
		if (pNext==pItem)
			return TRUE;
	}

	return FALSE;
}

///////////////////////////////////////////////////

void CMusikPropTree::EnsureVisible(CMusikPropTreeItem* pItem)
{
	if (!pItem)
		return;

	// item is not scroll visible (expand all parents)
	if (!IsItemVisible(pItem))
	{
		CMusikPropTreeItem* pParent;

		pParent = pItem->GetParent();
		while (pParent)
		{
			pParent->Expand();
			pParent = pParent->GetParent();
		}

		UpdatedItems();
		UpdateWindow();
	}

	CRect rc;

	GetClientRect(rc);
	rc.OffsetRect(0, m_ScrollPos);
	rc.bottom -= pItem->GetHeight();

	CPoint pt;

	pt = pItem->GetLocation();

	if (!rc.PtInRect(pt))
	{
		LONG oy;

		if (pt.y < rc.top)
			oy = pt.y;
		else
			oy = pt.y - rc.Height() + pItem->GetHeight();

		OnVScroll(SB_THUMBTRACK, oy, NULL);
	}
}

///////////////////////////////////////////////////

CMusikPropTreeItem* CMusikPropTree::InsertItem(CMusikPropTreeItem* pItem, CMusikPropTreeItem* pParent)
{
	if (!pItem)
		return NULL;

	if (!pParent)
		pParent = &m_Root;

	if (!pParent->GetChild())
		pParent->SetChild(pItem);
	else
	{
		// add to end of the sibling list
		CMusikPropTreeItem* pNext;

		pNext = pParent->GetChild();
		while (pNext->GetSibling())
			pNext = pNext->GetSibling();

		pNext->SetSibling(pItem);
	}

	pItem->SetParent(pParent);
	pItem->SetPropOwner(this);

	// auto generate a default ID
	pItem->SetCtrlID(m_nLastUID++);

	SendNotify(PTN_INSERTITEM, pItem);
	UpdatedItems();
	m_Count++;

	return pItem;

}

///////////////////////////////////////////////////

LONG CMusikPropTree::HitTest(const POINT& pt)
{
	POINT p = pt;

	CMusikPropTreeItem* pItem;

	// convert screen to tree coordinates
	p.y += m_ScrollPos;

	if ((pItem = FindItem(pt))!=NULL)
	{
		if (pItem->HitExpand(p))
			return HTEXPAND;

		return HTLABEL;
	}

	return HTCLIENT;
}

///////////////////////////////////////////////////

CMusikPropTreeItem* CMusikPropTree::HitTestEx( const POINT& pt )
{
	POINT p = pt;

	CMusikPropTreeItem* pItem;

	// convert screen to tree coordinates
	p.y += m_ScrollPos;

	if ((pItem = FindItem(pt))!=NULL)
		return pItem;

	return NULL;
}

///////////////////////////////////////////////////

CMusikPropTreeItem* CMusikPropTree::FindItem(const POINT& pt)
{
	CMusikPropTreeItem* pItem;

	CPoint p = pt;

	// convert screen to tree coordinates
	p.y += m_ScrollPos;

	// search the visible list for the item
	for (pItem = m_pVisbleList; pItem; pItem = pItem->GetNextVisible())
	{
		CPoint ipt = pItem->GetLocation();
		if (p.y>=ipt.y && p.y<ipt.y + pItem->GetHeight())
			return pItem;
	}

	return NULL;
}

///////////////////////////////////////////////////

CMusikPropTreeItem* CMusikPropTree::FindItem(UINT nCtrlID)
{
	s_pFound = NULL;

	EnumItems(&m_Root, EnumFindItem, nCtrlID);

	return s_pFound;
}

///////////////////////////////////////////////////

BOOL CALLBACK CMusikPropTree::EnumFindItem(CMusikPropTree*, CMusikPropTreeItem* pItem, LPARAM lParam)
{
	ASSERT(pItem!=NULL);

	if (pItem->GetCtrlID()==(UINT)lParam)
	{
		s_pFound = pItem;
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////

void CMusikPropTree::SelectItems(CMusikPropTreeItem* pItem, BOOL bSelect)
{
	if (!pItem)
		pItem = &m_Root;

	EnumItems(pItem, EnumSelectAll, (LPARAM)bSelect);
}

///////////////////////////////////////////////////

CMusikPropTreeItem* CMusikPropTree::FocusFirst()
{
	CMusikPropTreeItem *pold;

	pold = m_pFocus;

	SetFocusedItem(m_pVisbleList);

	if (m_pFocus)
	{
		SelectItems(NULL, FALSE);
		m_pFocus->Select();
	}

	if (pold!=m_pFocus)
		SendNotify(PTN_SELCHANGE, m_pFocus);

	return m_pFocus;
}

///////////////////////////////////////////////////

CMusikPropTreeItem* CMusikPropTree::FocusLast()
{
	CMusikPropTreeItem* pNext;
	CMusikPropTreeItem* pChange;

	pChange = m_pFocus;

	pNext = m_pVisbleList;

	if (pNext)
	{
		while (pNext->GetNextVisible())
			pNext = pNext->GetNextVisible();

		SetFocusedItem(pNext);

		if (m_pFocus)
		{
			SelectItems(NULL, FALSE);
			m_pFocus->Select();
		}
	}

	if (pChange!=m_pFocus)
		SendNotify(PTN_SELCHANGE, m_pFocus);

	return pNext;
}

///////////////////////////////////////////////////

CMusikPropTreeItem* CMusikPropTree::FocusPrev()
{
	CMusikPropTreeItem* pNext;
	CMusikPropTreeItem* pChange;

	pChange = m_pFocus;

	if (m_pFocus==NULL)
	{
		// get the last visible item
		pNext = m_pVisbleList;
		while (pNext && pNext->GetNextVisible())
			pNext = pNext->GetNextVisible();
	}
	else
	{
		pNext = m_pVisbleList;
		while (pNext && pNext->GetNextVisible()!=m_pFocus)
			pNext = pNext->GetNextVisible();
	}

	if (pNext)
		SetFocusedItem(pNext);
	
	if (m_pFocus)
	{
		SelectItems(NULL, FALSE);
		m_pFocus->Select();
	}

	if (pChange!=m_pFocus)
		SendNotify(PTN_SELCHANGE, m_pFocus);

	return pNext;
}

///////////////////////////////////////////////////

CMusikPropTreeItem* CMusikPropTree::FocusNext()
{
	CMusikPropTreeItem* pNext;
	CMusikPropTreeItem* pChange;

	pChange = m_pFocus;

	// if there is no current focus, set
	// it to the first item?
	if ( m_pFocus == NULL )
		pNext = m_pVisbleList;

	// else, get the next visible item
	else if ( m_pFocus->GetNextVisible() )
		pNext = m_pFocus->GetNextVisible();

	// else, no next item is availble
	else
		pNext = NULL;

	// if there is a next item, focus it
	if ( pNext )
		SetFocusedItem( pNext );

	// if there is a currently focused item,
	// all appeared to go ok, so select it
	if ( m_pFocus )
	{
		SelectItems( NULL, FALSE );
		m_pFocus->Select();
	}

	// if the selection has updated, send
	// a notify event
	if ( pChange != m_pFocus )
		SendNotify(PTN_SELCHANGE, m_pFocus);

	return pNext;
}

///////////////////////////////////////////////////

void CMusikPropTree::UpdateMoveAllItems()
{
	EnumItems(&m_Root, EnumMoveAll);
}

///////////////////////////////////////////////////

void CMusikPropTree::RefreshItems(CMusikPropTreeItem* pItem)
{
	if (!pItem)
		pItem = &m_Root;

	EnumItems(pItem, EnumRefreshAll);

	UpdatedItems();
}

///////////////////////////////////////////////////

BOOL CALLBACK CMusikPropTree::EnumSelectAll(CMusikPropTree*, CMusikPropTreeItem* pItem, LPARAM lParam)
{
	if (!pItem)
		return FALSE;

	pItem->Select((BOOL)lParam);

	return TRUE;
}

///////////////////////////////////////////////////

BOOL CALLBACK CMusikPropTree::EnumRefreshAll(CMusikPropTree*, CMusikPropTreeItem* pItem, LPARAM)
{
	if (!pItem)
		return FALSE;

	pItem->OnRefresh();

	return TRUE;
}

///////////////////////////////////////////////////

BOOL CALLBACK CMusikPropTree::EnumMoveAll(CMusikPropTree*, CMusikPropTreeItem* pItem, LPARAM)
{
	if (!pItem)
		return FALSE;

	pItem->OnMove();

	return TRUE;
}

///////////////////////////////////////////////////

LRESULT CMusikPropTree::SendNotify(UINT nNotifyCode, CMusikPropTreeItem* pItem)
{
	if (!IsWindow(m_hWnd))
		return 0L;

	if (!(GetStyle() & PTS_NOTIFY))
		return 0L;

	NMPROPTREE nmmp;
	LPNMHDR lpnm;

	lpnm = NULL;

	switch (nNotifyCode)
	{
		case PTN_INSERTITEM:
		case PTN_DELETEITEM:
		case PTN_DELETEALLITEMS:
		case PTN_ITEMCHANGED:
		case PTN_ITEMBUTTONCLICK:
		case PTN_SELCHANGE:
		case PTN_ITEMEXPANDING:
		case PTN_PROPCLICK:
			lpnm = (LPNMHDR)&nmmp;
			nmmp.pItem = pItem;
			break;
	}

	if (lpnm)
	{
		UINT id = (UINT)::GetMenu(m_hWnd);
		lpnm->code = nNotifyCode;
		lpnm->hwndFrom = m_hWnd;
		lpnm->idFrom = id;
	
		return GetParent()->SendMessage(WM_NOTIFY, (WPARAM)id, (LPARAM)lpnm);
	}

	return 0L;
}

///////////////////////////////////////////////////

void CMusikPropTree::OnEnable(BOOL bEnable) 
{
	CWnd::OnEnable(bEnable);

	Invalidate();
}

///////////////////////////////////////////////////

void CMusikPropTree::OnSysColorChange() 
{
	CWnd::OnSysColorChange();
	
	Invalidate();	
}

///////////////////////////////////////////////////

void CMusikPropTree::RecreateBackBuffer(int cx, int cy)
{
	if (m_BackBufferSize.cx<cx || m_BackBufferSize.cy<cy)
	{
		m_BackBufferSize = CSize(cx, cy);

		CWindowDC dc(NULL);

		int nPlanes = dc.GetDeviceCaps(PLANES);
		int nBitCount = dc.GetDeviceCaps(BITSPIXEL);

		m_BackBuffer.DeleteObject();
		m_BackBuffer.CreateBitmap(cx, cy, nPlanes, nBitCount, NULL);
	}
}

///////////////////////////////////////////////////

void CMusikPropTree::UpdateResize()
{
	SCROLLINFO si;
	LONG nHeight;
	CRect rc;

	GetClientRect(rc);
	nHeight = rc.Height() + 1;

	ZeroMemory(&si, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE|SIF_PAGE;
	si.nMin = 0;
	si.nMax = GetRootItem()->GetTotalHeight();
	si.nPage = nHeight;

	if ((int)si.nPage>si.nMax)
		SetOriginOffset(0);

	SetScrollInfo(SB_VERT, &si, TRUE);
}

///////////////////////////////////////////////////

void CMusikPropTree::OnPaint() 
{
	CPaintDC dc(this);

	ClearVisibleList();

	// get client rect
	CRect rc;
	GetClientRect( rc );

	// draw control background
	CMemDC memdc( &dc );
	memdc.FillSolidRect( rc, m_Prefs->MUSIK_COLOR_LISTCTRL );

	CMusikPropTreeItem* pItem;
	LONG nTotal = 0;

	ASSERT(GetRootItem()!=NULL);

	// create clip region
	HRGN hRgn = CreateRectRgn( rc.left, rc.top, rc.right, rc.bottom );
	SelectClipRgn( memdc.m_hDC, hRgn );

	// draw all items
	for ( pItem = GetRootItem()->GetChild(); pItem; pItem = pItem->GetSibling() )
	{
		LONG nHeight = pItem->DrawItem(&memdc, rc, 0, nTotal);
		nTotal += nHeight;
	}

	// remove clip region
	SelectClipRgn(memdc.m_hDC, NULL);
	DeleteObject(hRgn);
}

///////////////////////////////////////////////////

BOOL CMusikPropTree::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

///////////////////////////////////////////////////

void CMusikPropTree::OnLButtonDown(UINT, CPoint point) 
{
	SendNotify(NM_CLICK);

	if (!IsWindowEnabled())
		return;

	SetFocus();

	LONG nHit = HitTest(point);

	CMusikPropTreeItem* pItem;
	CRect rc;

	switch (nHit)
	{
		case HTEXPAND:
			if ((pItem = FindItem(point))!=NULL)
			{
				if (pItem->GetChild() && !SendNotify(PTN_ITEMEXPANDING, pItem))
				{
					pItem->Expand(!pItem->IsExpanded());

					UpdateResize();
					Invalidate();
					UpdateWindow();
					CheckVisibleFocus();
				}
			}
			break;

		default:
			if ((pItem = FindItem(point))!=NULL)
			{
				CMusikPropTreeItem* pOldFocus = GetFocusedItem();

				if ( pItem->IsRootLevel() )
					return;

				SelectItems(NULL, FALSE);
				SetFocusedItem(pItem);

				pItem->Select();

				Invalidate();

				if (pItem!=pOldFocus)
					SendNotify(PTN_SELCHANGE, pItem);

				if (nHit==HTATTRIBUTE && !pItem->IsRootLevel())
				{
					if (!SendNotify(PTN_PROPCLICK, pItem))
						pItem->Activate();
				}
			}
			break;
	}
}

///////////////////////////////////////////////////

void CMusikPropTree::OnLButtonDblClk(UINT, CPoint point)
{
	SendNotify(NM_DBLCLK);

	CMusikPropTreeItem* pItem;
	CMusikPropTreeItem* pOldFocus;

	if ((pItem = FindItem(point))!=NULL && pItem->GetChild())
	{
		switch (HitTest(point))
		{
			case HTATTRIBUTE:
				if (!pItem->IsRootLevel())
					break;

				// pass thru to default

			default:
				if ( !pItem->IsRootLevel() )
				{
					pOldFocus = GetFocusedItem();
					SelectItems(NULL, FALSE);
					SetFocusedItem(pItem);
					pItem->Select();

					if (pItem!=pOldFocus)
						SendNotify(PTN_SELCHANGE, pItem);
				}

				// pass thru to HTEXPAND

			case HTEXPAND:
				if (!SendNotify(PTN_ITEMEXPANDING, pItem))
				{
					pItem->Expand(!pItem->IsExpanded());

					UpdateResize();
					Invalidate();
					UpdateWindow();
					CheckVisibleFocus();
				}
				break;
		}
	}
}

///////////////////////////////////////////////////



///////////////////////////////////////////////////

BOOL CMusikPropTree::OnMouseWheel(UINT, short zDelta, CPoint) 
{
	SCROLLINFO si;

	ZeroMemory(&si, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE;

	GetScrollInfo(SB_VERT, &si);

	CRect rc;
	GetClientRect(rc);

	if (si.nMax - si.nMin < rc.Height())
		return TRUE;

	SetFocus();
	OnVScroll(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0, NULL);

	return TRUE;
}

///////////////////////////////////////////////////

void CMusikPropTree::OnKeyDown(UINT nChar, UINT, UINT) 
{
	CMusikPropTreeItem* pItem;

	switch (nChar)
	{
		case VK_RETURN:
			if ((pItem = GetFocusedItem())!=NULL && !pItem->IsRootLevel())
			{
				pItem->Activate();
			}
			break;

		case VK_HOME:
			if (FocusFirst())
				Invalidate();
			break;

		case VK_END:
			if (FocusLast())
				Invalidate();
			break;

		case VK_LEFT:
			if ((pItem = GetFocusedItem())!=NULL)
			{
				if (!SendNotify(PTN_ITEMEXPANDING, pItem))
				{
					if (pItem->GetChild() && pItem->IsExpanded())
					{
						pItem->Expand(FALSE);
						UpdateResize();
						Invalidate();
						UpdateWindow();
						CheckVisibleFocus();
						break;
					}
				}
			}
			else
				break;
			// pass thru to next case VK_UP
		case VK_UP:
			if (FocusPrev())
				Invalidate();
			break;

		case VK_RIGHT:
			if ((pItem = GetFocusedItem())!=NULL)
			{
				if (!SendNotify(PTN_ITEMEXPANDING, pItem))
				{
					if (pItem->GetChild() && !pItem->IsExpanded())
					{
						pItem->Expand();
						UpdateResize();
						Invalidate();
						UpdateWindow();
						CheckVisibleFocus();
						break;
					}
				}
			}
			else
				break;
			// pass thru to next case VK_DOWN
		case VK_DOWN:
			if (FocusNext())
				Invalidate();
			break;
	}
}

///////////////////////////////////////////////////

UINT CMusikPropTree::OnGetDlgCode() 
{
	return DLGC_WANTARROWS|DLGC_WANTCHARS|DLGC_WANTALLKEYS;
}

///////////////////////////////////////////////////

void CMusikPropTree::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar*) 
{
	SCROLLINFO si;
	CRect rc;
	LONG nHeight;

	SetFocus();

	GetClientRect(rc);
	nHeight = rc.Height() + 1;

	ZeroMemory(&si, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE;

	GetScrollInfo(SB_VERT, &si);

	LONG ny = GetOrigin();

	switch (nSBCode)
	{
		case SB_LINEDOWN:
			ny += PROPTREEITEM_DEFHEIGHT;
			break;

		case SB_LINEUP:
			ny -= PROPTREEITEM_DEFHEIGHT;
			break;

		case SB_PAGEDOWN:
			ny += nHeight;
			break;

		case SB_PAGEUP:
			ny -= nHeight;
			break;

		case SB_THUMBTRACK:
			ny = nPos;
			break;
	}

	ny = __min(__max(ny, si.nMin), si.nMax - nHeight);

	SetOriginOffset(ny);
	si.fMask = SIF_POS;
	si.nPos = ny;

	SetScrollInfo(SB_VERT, &si, TRUE);
	Invalidate();
}

///////////////////////////////////////////////////

void CMusikPropTree::CheckVisibleFocus()
{
	CMusikPropTreeItem* pItem;
	
	if ((pItem = GetFocusedItem())==NULL)
		return;

	if (!IsItemVisible(pItem))
	{
		pItem->Select(FALSE);

		SetFocusedItem(NULL);
		SendNotify(PTN_SELCHANGE, NULL);

		Invalidate();
	}
}

///////////////////////////////////////////////////

void CMusikPropTree::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( m_LockHover )
		return;

	m_LockHover = true;

	if ( !m_MouseTrack )
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof( tme );
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		tme.dwHoverTime = HOVER_DEFAULT;
		::_TrackMouseEvent( &tme );

		m_MouseTrack = true;
	}

	// see if a mouse is over a new item...
	int nItemPos = FindNewItemPos( point );
	if ( nItemPos > -1 )
		OnNewHoveredItem( nItemPos );

	m_LockHover = false;
}

int CMusikPropTree::FindNewItemPos( const CPoint& point )
{
	const int nTotal = PROPTREEITEM_DEFHEIGHT * ( m_Count );
	int nItemPos = -1;

	if ( ( point.y > m_LastPos.m_Top && point.y <= nTotal )		|| 
		( point.y < m_LastPos.m_Top && point.y >= 0 )			|| 
		( m_LastPos.m_Bottom == -1 && m_LastPos.m_Top == -1 ) )
	{

		CMusikTrack curr;
		for( int i = 0; i < m_Count; i++ )
		{
			curr.m_Top = PROPTREEITEM_DEFHEIGHT * i;
			curr.m_Bottom = PROPTREEITEM_DEFHEIGHT * ( i + 1 );

			if ( point.y > curr.m_Top && point.y <= curr.m_Bottom  )
			{
				if ( m_LastPos.m_Bottom != curr.m_Bottom && m_LastPos.m_Top != curr.m_Top )
				{
					nItemPos = i;
					m_LastPos = curr;
				}
			}
		}
	}

	return nItemPos;
}

///////////////////////////////////////////////////

void CMusikPropTree::OnNewHoveredItem( int nIndex )
{
	CStdString s;
	s.Format( "new item hovered at %d\n", nIndex );
	TRACE0( s.c_str() );
}

///////////////////////////////////////////////////

LRESULT CMusikPropTree::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_MouseTrack = false;

	m_LastPos.m_Bottom = -1;
	m_LastPos.m_Top = -1;

	if ( m_pHovered )
	{
		m_pHovered->Hover( FALSE );
		SetHoveredItem( NULL );
		m_pHovered = NULL;
	}

    SetHoveredItem( NULL );

	return 0L;
}

///////////////////////////////////////////////////