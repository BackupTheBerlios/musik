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

#include "stdafx.h"

#include "PropTree.h"
#include "Resource.h"
#include "PropTreeList.h"

#include "../../Musik.Core/include/MusikLibrary.h"

#include "../MEMDC.H"

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////

#define PROPTREEITEM_EXPANDCOLUMN		16			// width of the expand column
#define PROPTREEITEM_COLRNG				5			// width of splitter
#define PROPTREEITEM_DEFHEIGHT			21			// default heigt of an item

extern HINSTANCE ghInst;

///////////////////////////////////////////////////

CPropTreeList::CPropTreeList() :
	m_pProp(NULL),
	m_BackBufferSize(0,0),
	m_bMouseTrack(false)
{
	m_HoverLast = NULL;
	m_HoverCurrent = NULL;
}

///////////////////////////////////////////////////

CPropTreeList::~CPropTreeList()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPropTreeList, CWnd)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_VSCROLL()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CPropTreeList::SetPropOwner(CPropTree* pProp)
{
	m_pProp = pProp;
}

///////////////////////////////////////////////////

BOOL CPropTreeList::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	CWnd* pWnd = this;

	LPCTSTR pszCreateClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW));

	return pWnd->Create(pszCreateClass, _T(""), dwStyle, rect, pParentWnd, nID);
}

///////////////////////////////////////////////////

void CPropTreeList::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	RecreateBackBuffer(cx, cy);

	if (m_pProp)
	{
		UpdateResize();
		Invalidate();
		UpdateWindow();

		// inform all items that a resize has been made
		m_pProp->UpdateMoveAllItems();
	}
}

///////////////////////////////////////////////////

void CPropTreeList::RecreateBackBuffer(int cx, int cy)
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

void CPropTreeList::UpdateResize()
{
	SCROLLINFO si;
	LONG nHeight;
	CRect rc;

	ASSERT(m_pProp!=NULL);

	GetClientRect(rc);
	nHeight = rc.Height() + 1;

	ZeroMemory(&si, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE|SIF_PAGE;
	si.nMin = 0;
	si.nMax = m_pProp->GetRootItem()->GetTotalHeight();
	si.nPage = nHeight;

	if ((int)si.nPage>si.nMax)
		m_pProp->SetOriginOffset(0);

	SetScrollInfo(SB_VERT, &si, TRUE);

	// force set column for clipping
	m_pProp->SetColumn(m_pProp->GetColumn());
}

///////////////////////////////////////////////////

void CPropTreeList::OnPaint() 
{
	CPaintDC dc(this);

	ASSERT(m_pProp!=NULL);

	m_pProp->ClearVisibleList();

	// get client rect
	CRect rc;
	GetClientRect( rc );

	// draw control background
	CMemDC memdc( &dc );
	memdc.FillSolidRect( rc, m_pProp->m_Prefs->MUSIK_COLOR_LISTCTRL );

	CPropTreeItem* pItem;
	LONG nTotal = 0;

	ASSERT(m_pProp->GetRootItem()!=NULL);

	// create clip region
	HRGN hRgn = CreateRectRgn( rc.left, rc.top, rc.right, rc.bottom );
	SelectClipRgn( memdc.m_hDC, hRgn );

	// draw all items
	for ( pItem = m_pProp->GetRootItem()->GetChild(); pItem; pItem = pItem->GetSibling() )
	{
		LONG nHeight = pItem->DrawItem(&memdc, rc, 0, nTotal);
		nTotal += nHeight;
	}

	// remove clip region
	SelectClipRgn(memdc.m_hDC, NULL);
	DeleteObject(hRgn);
}

///////////////////////////////////////////////////

BOOL CPropTreeList::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

///////////////////////////////////////////////////

void CPropTreeList::OnLButtonDown(UINT, CPoint point) 
{
	ASSERT(m_pProp!=NULL);

	if (m_pProp->IsDisableInput())
		return;

	m_pProp->SendNotify(NM_CLICK);

	if (!m_pProp->IsWindowEnabled())
		return;

	SetFocus();

	LONG nHit = m_pProp->HitTest(point);

	CPropTreeItem* pItem;
	CRect rc;

	switch (nHit)
	{
		case HTEXPAND:
			if ((pItem = m_pProp->FindItem(point))!=NULL)
			{
				if (pItem->GetChild() && !m_pProp->SendNotify(PTN_ITEMEXPANDING, pItem))
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
			if ((pItem = m_pProp->FindItem(point))!=NULL)
			{
				CPropTreeItem* pOldFocus = m_pProp->GetFocusedItem();

				if ( pItem->IsRootLevel() )
					return;

				m_pProp->SelectItems(NULL, FALSE);
				m_pProp->SetFocusedItem(pItem);

				pItem->Select();

				Invalidate();

				if (pItem!=pOldFocus)
					m_pProp->SendNotify(PTN_SELCHANGE, pItem);

				if (nHit==HTATTRIBUTE && !pItem->IsRootLevel())
				{
					if (!m_pProp->SendNotify(PTN_PROPCLICK, pItem) && !pItem->IsReadOnly())
						pItem->Activate();
				}
			}
			else
			{
				m_pProp->SelectItems(NULL, FALSE);
				m_pProp->SetFocusedItem(NULL);
				m_pProp->SendNotify(PTN_SELCHANGE);
				Invalidate();
			}
			break;
	}
}

///////////////////////////////////////////////////

void CPropTreeList::OnLButtonUp(UINT, CPoint point) 
{

}

///////////////////////////////////////////////////

void CPropTreeList::OnLButtonDblClk(UINT, CPoint point)
{
	ASSERT(m_pProp!=NULL);

	m_pProp->SendNotify(NM_DBLCLK);

	CPropTreeItem* pItem;
	CPropTreeItem* pOldFocus;

	if ((pItem = m_pProp->FindItem(point))!=NULL && pItem->GetChild())
	{
		switch (m_pProp->HitTest(point))
		{
			case HTATTRIBUTE:
				if (!pItem->IsRootLevel())
					break;

				// pass thru to default

			default:
				if ( !pItem->IsRootLevel() )
				{
					pOldFocus = m_pProp->GetFocusedItem();
					m_pProp->SelectItems(NULL, FALSE);
					m_pProp->SetFocusedItem(pItem);
					pItem->Select();

					if (pItem!=pOldFocus)
						m_pProp->SendNotify(PTN_SELCHANGE, pItem);
				}

				// pass thru to HTEXPAND

			case HTEXPAND:
				if (!m_pProp->SendNotify(PTN_ITEMEXPANDING, pItem))
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

void CPropTreeList::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( !m_bMouseTrack )
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		tme.dwHoverTime = HOVER_DEFAULT;
		::_TrackMouseEvent(&tme);

		m_bMouseTrack = true; 	
	}

	CPropTreeItem* pItem = m_pProp->HitTestEx( point );

	// we are dragging...
	if ( pItem && pItem->GetPlaylistType() != -1 && m_bMouseTrack && ( nFlags & MK_LBUTTON ) )
	{
		DoDrag( pItem );
		return;
	}

	// not dragging, just track mouse
	// and hilight hovered entry...
	else if ( m_HoverCurrent != pItem && pItem != NULL )
	{
		m_HoverLast = m_HoverCurrent;
		m_HoverCurrent = pItem;

		if ( m_HoverLast != NULL )
			m_HoverLast->SetMouseOver( FALSE );

		m_HoverCurrent->SetMouseOver( TRUE );

		m_pProp->SetHoveredItem( m_HoverCurrent );
		m_pProp->Invalidate();
	}
}

///////////////////////////////////////////////////

LRESULT CPropTreeList::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bMouseTrack = false;

	if ( m_HoverCurrent )
	{
		m_HoverCurrent->SetMouseOver( FALSE );
		m_HoverCurrent = NULL;
	}

	if ( m_HoverLast )
	{
		m_HoverLast->SetMouseOver( FALSE );
		m_HoverLast = NULL;
	}

    m_pProp->SetHoveredItem( NULL );

	return 0L;
}

///////////////////////////////////////////////////

BOOL CPropTreeList::OnMouseWheel(UINT, short zDelta, CPoint) 
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

void CPropTreeList::OnKeyDown(UINT nChar, UINT, UINT) 
{

	CPropTreeItem* pItem;

	ASSERT(m_pProp!=NULL);

	if (m_pProp->IsDisableInput() || !m_pProp->IsWindowEnabled())
		return;

	switch (nChar)
	{
		case VK_RETURN:
			if ((pItem = m_pProp->GetFocusedItem())!=NULL && !pItem->IsRootLevel() && !pItem->IsReadOnly())
			{
				pItem->Activate();
			}
			break;

		case VK_HOME:
			if (m_pProp->FocusFirst())
				Invalidate();
			break;

		case VK_END:
			if (m_pProp->FocusLast())
				Invalidate();
			break;

		case VK_LEFT:
			if ((pItem = m_pProp->GetFocusedItem())!=NULL)
			{
				if (!m_pProp->SendNotify(PTN_ITEMEXPANDING, pItem))
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
			if (m_pProp->FocusPrev())
				Invalidate();
			break;

		case VK_RIGHT:
			if ((pItem = m_pProp->GetFocusedItem())!=NULL)
			{
				if (!m_pProp->SendNotify(PTN_ITEMEXPANDING, pItem))
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
			if (m_pProp->FocusNext())
				Invalidate();
			break;
	}
}

///////////////////////////////////////////////////

UINT CPropTreeList::OnGetDlgCode() 
{
	return DLGC_WANTARROWS|DLGC_WANTCHARS|DLGC_WANTALLKEYS;
}

///////////////////////////////////////////////////

void CPropTreeList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar*) 
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

	LONG ny = m_pProp->GetOrigin().y;

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

	m_pProp->SetOriginOffset(ny);
	si.fMask = SIF_POS;
	si.nPos = ny;

	SetScrollInfo(SB_VERT, &si, TRUE);
	Invalidate();
}

///////////////////////////////////////////////////

void CPropTreeList::CheckVisibleFocus()
{
	ASSERT(m_pProp!=NULL);

	CPropTreeItem* pItem;
	
	if ((pItem = m_pProp->GetFocusedItem())==NULL)
		return;

	if (!m_pProp->IsItemVisible(pItem))
	{
		if (m_pProp->IsSingleSelection())
			pItem->Select(FALSE);

		m_pProp->SetFocusedItem(NULL);
		m_pProp->SendNotify(PTN_SELCHANGE, NULL);

		Invalidate();
	}
}

///////////////////////////////////////////////////

void CPropTreeList::DoDrag( CPropTreeItem* pItem )
{
	if ( !pItem )
		return;

	COleDataSource datasrc;
	HGLOBAL        hgDrop;
	DROPFILES*     pDrop;
	CStringList    lsDraggedFiles;
	POSITION       pos;
	CString        sFile;
	UINT           uBuffSize = 0;
	TCHAR*         pszBuff;
	FORMATETC      etc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	// get a list of filenames with the currently
	// selected items...
	CStdStringArray files;

	int nMode = pItem->GetPlaylistType();
	if ( nMode == MUSIK_PLAYLIST_TYPE_STANDARD )
		m_pProp->m_Library->GetStdPlaylistFns( pItem->GetPlaylistID(), files, false );
	 else if ( nMode == MUSIK_PLAYLIST_TYPE_DYNAMIC )
		MessageBox( "This operation is not supported yet.", "Musik", MB_ICONINFORMATION | MB_OK );
	 else if ( nMode == MUSIK_SOURCES_TYPE_LIBRARY || nMode == MUSIK_SOURCES_TYPE_NOWPLAYING )
		MessageBox( "This operation is not supported yet.", "Musik", MB_ICONINFORMATION | MB_OK );

	if ( !files.size() )
		return;

	// CStringList containing files
	for ( size_t i = 0; i < files.size(); i++ )
	{
		lsDraggedFiles.AddTail( files.at( i ) );
		uBuffSize += files.at( i ).GetLength() + 1;
	}

	files.clear();

	// Add 1 extra for the final null char, and the size of the DROPFILES struct.
	uBuffSize = sizeof(DROPFILES) + sizeof(TCHAR) * (uBuffSize + 1);

	// Allocate memory from the heap for the DROPFILES struct.
	hgDrop = GlobalAlloc ( GHND | GMEM_SHARE, uBuffSize );

	if ( !hgDrop )
		return;

	pDrop = (DROPFILES*) GlobalLock ( hgDrop );

	if ( !pDrop )
	{
		GlobalFree ( hgDrop );
		return;
	}

	// Fill in the DROPFILES struct.
	pDrop->pFiles = sizeof(DROPFILES);

	// If we're compiling for Unicode, set the Unicode flag in the struct to
	// indicate it contains Unicode strings.
	#ifdef _UNICODE
		pDrop->fWide = TRUE;
	#endif;

	// Copy all the filenames into memory after the end of the DROPFILES struct.
	pos = lsDraggedFiles.GetHeadPosition();
	pszBuff = (TCHAR*) (LPBYTE(pDrop) + sizeof(DROPFILES));

	while ( pos )
	{
		lstrcpy ( pszBuff, (LPCTSTR) lsDraggedFiles.GetNext ( pos ) );
		pszBuff = 1 + _tcschr ( pszBuff, '\0' );
	}

	GlobalUnlock ( hgDrop );

	// Put the data in the data source.
	datasrc.CacheGlobalData ( CF_HDROP, hgDrop, &etc );

	// Add in our own custom data, so we know that the drag originated from our 
	// window.  CMyDropTarget::DragEnter() checks for this custom format, and
	// doesn't allow the drop if it's present.  This is how we prevent the user
	// from dragging and then dropping in our own window.
	// The data will just be a dummy bool.
	HGLOBAL hgBool;

	hgBool = GlobalAlloc ( GHND | GMEM_SHARE, sizeof(bool) );

	if ( NULL == hgBool )
	{
		GlobalFree ( hgDrop );
		return;
	}

	// Put the data in the data source.
	etc.cfFormat = m_pProp->m_DropID;
	datasrc.CacheGlobalData ( m_pProp->m_DropID, hgBool, &etc );

	// Start the drag 'n' drop!
	DROPEFFECT dwEffect = datasrc.DoDragDrop ( DROPEFFECT_COPY | DROPEFFECT_MOVE );

	// If the DnD completed OK, we remove all of the dragged items from our
	// list.
	switch ( dwEffect )
	{
		case DROPEFFECT_COPY:
		case DROPEFFECT_MOVE:
		{
			// the copy completed successfully
			// do whatever we need to do here
			TRACE0( "DND from playlist completed successfully. The data has a new owner.\n" );
		}
		break;

		case DROPEFFECT_NONE:
		{
			// This needs special handling, because on NT, DROPEFFECT_NONE
			// is returned for move operations, instead of DROPEFFECT_MOVE.
			// See Q182219 for the details.
			// So if we're on NT, we check each selected item, and if the
			// file no longer exists, it was moved successfully and we can
			// remove it from the list.
			if ( m_pProp->m_IsWinNT )
			{
				// the copy completed successfully
				// on a windows nt machine.
				// do whatever we need to do here

				bool bDeletedAnything = false;
				if ( ! bDeletedAnything )
				{
					// The DnD operation wasn't accepted, or was canceled, so we 
					// should call GlobalFree() to clean up.
					GlobalFree ( hgDrop );
					GlobalFree ( hgBool );

					TRACE0( "DND had a problem. We had to manually free the data.\n" );
				}
			}

			// not windows NT
			else
			{
				// We're on 9x, and a return of DROPEFFECT_NONE always means
				// that the DnD operation was aborted.  We need to free the
				// allocated memory.
				GlobalFree ( hgDrop );
				GlobalFree ( hgBool );

				TRACE0( "DND had a problem. We had to manually free the data.\n" );
			}
		}

		break;
	}		
}

///////////////////////////////////////////////////