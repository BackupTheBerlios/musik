///////////////////////////////////////////////////
//
// PropTreeItem.cpp
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
//  Modified heavily by Casey Langen for musik MFC port...
//    - Rather than linking to a dll or external lib, it compiles in.
//    - PropTree and PropTreeList combined into one file
//    - Mouse hover tracking
//    - Drawing enhancements
//    - Various musik specific functions
//
//  The musik team thanks Scott Ramsay for creating this great
//  class and making it open source. Cheers.
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "../../musikCore/include/musikPlaylist.h"
#include "../musikPrefs.h"

#include "musikPropTree.h"
#include "musikPropTreeItem.h"

///////////////////////////////////////////////////

#define PROPTREEITEM_DEFHEIGHT			21			// default heigt of an item
#define PROPTREEITEM_SPACE				5			// default horz spacing
#define PROPTREEITEM_EXPANDBOX			9			// size of the expand box
#define PROPTREEITEM_EXPANDCOLUMN		16			// width of the expand column
#define PNINDENT						16			// child level indent

#define PROPTREEITEM_EXPANDBOXHALF		(PROPTREEITEM_EXPANDBOX/2)

///////////////////////////////////////////////////

static void HLine(HDC hdc, LONG x, LONG y, LONG w, COLORREF color)
{
	for (; w>0; w--, x++)
		SetPixel(hdc, x, y, color);
	
}

static void _DrawExpand(HDC hdc, LONG x, LONG y, BOOL bExpand, BOOL bFill)
{
	HPEN hPen;
	HPEN oPen;
	HBRUSH oBrush;

	hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
	oPen = (HPEN)SelectObject(hdc, hPen);
	oBrush = (HBRUSH)SelectObject(hdc, GetStockObject(bFill ? WHITE_BRUSH : NULL_BRUSH));

	Rectangle(hdc, x, y, x + PROPTREEITEM_EXPANDBOX, y + PROPTREEITEM_EXPANDBOX);
	SelectObject(hdc, GetStockObject(BLACK_PEN));

	if (!bExpand)
	{
		MoveToEx(hdc, x + PROPTREEITEM_EXPANDBOXHALF, y + 2, NULL);
		LineTo(hdc, x + PROPTREEITEM_EXPANDBOXHALF, y + PROPTREEITEM_EXPANDBOX - 2);
	}

	MoveToEx(hdc, x + 2, y + PROPTREEITEM_EXPANDBOXHALF, NULL);
	LineTo(hdc, x + PROPTREEITEM_EXPANDBOX - 2, y + PROPTREEITEM_EXPANDBOXHALF);

	SelectObject(hdc, oPen);
	SelectObject(hdc, oBrush);
	DeleteObject(hPen);
}

///////////////////////////////////////////////////

CmusikPropTreeItem::CmusikPropTreeItem() :
	m_pProp(NULL),
	m_sLabel(_T("")),
	m_loc(0,0),
	m_rc(0,0,0,0),
	m_lParam(0),
	m_nCtrlID(0),
	m_dwState(0),
	m_bActivated(FALSE),
	m_bCommitOnce(FALSE),
	m_bHover(FALSE),
	m_rcExpand(0,0,0,0),
	m_pParent(NULL),
	m_pSibling(NULL),
	m_pChild(NULL),
	m_pVis(NULL)
{
	m_PlaylistInfo = new CmusikPlaylistInfo();
}

///////////////////////////////////////////////////

CmusikPropTreeItem::~CmusikPropTreeItem()
{
	if ( m_PlaylistInfo )
		delete m_PlaylistInfo;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::SetPlaylistInfo( const CmusikPlaylistInfo info )
{
	if ( m_PlaylistInfo )
	{
		*m_PlaylistInfo = info;

		SetLabelText( m_PlaylistInfo->GetName() );
	}
}

///////////////////////////////////////////////////

int CmusikPropTreeItem::GetPlaylistType()
{
	return m_PlaylistInfo->GetType();
}

///////////////////////////////////////////////////

int CmusikPropTreeItem::GetPlaylistID()
{
	return m_PlaylistInfo->GetID();
}

///////////////////////////////////////////////////

BOOL CmusikPropTreeItem::IsExpanded()
{
	return (m_dwState & TreeItemExpanded) ? TRUE : FALSE;
}

///////////////////////////////////////////////////

BOOL CmusikPropTreeItem::IsSelected()
{
	return (m_dwState & TreeItemSelected) ? TRUE : FALSE;
}

///////////////////////////////////////////////////

BOOL CmusikPropTreeItem::IsHovered()
{
	return m_bHover;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::Hover( BOOL bHover )
{
	m_bHover = bHover;
}

///////////////////////////////////////////////////

BOOL CmusikPropTreeItem::IsActivated()
{
	return (m_dwState & TreeItemActivated) ? TRUE : FALSE;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::Select(BOOL bSelect)
{
	if (bSelect)
		m_dwState |= TreeItemSelected;
	else
		m_dwState &= ~TreeItemSelected;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::Expand(BOOL bExpand)
{
	if (bExpand)
		m_dwState |= TreeItemExpanded;
	else
		m_dwState &= ~TreeItemExpanded;
}

///////////////////////////////////////////////////

BOOL CmusikPropTreeItem::HitExpand(const POINT& pt)
{
	return m_rcExpand.PtInRect(pt);
}

///////////////////////////////////////////////////

BOOL CmusikPropTreeItem::IsRootLevel()
{
	return GetParent() == m_pProp->GetRootItem();
}

///////////////////////////////////////////////////

LONG CmusikPropTreeItem::GetTotalHeight()
{
	CmusikPropTreeItem* pItem;
	LONG nHeight;

	nHeight = GetHeight();

	if (IsExpanded())
	{
		for (pItem = GetChild(); pItem; pItem = pItem->GetSibling())
			nHeight += pItem->GetTotalHeight();
	}

	return nHeight;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::SetLabelText(LPCTSTR sLabel)
{
	m_sLabel = sLabel;
}

///////////////////////////////////////////////////

LPCTSTR CmusikPropTreeItem::GetLabelText()
{
	return m_sLabel;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::SetCtrlID(UINT nCtrlID)
{
	m_nCtrlID = nCtrlID;
}

///////////////////////////////////////////////////

UINT CmusikPropTreeItem::GetCtrlID()
{
	return m_nCtrlID;
}

///////////////////////////////////////////////////

LONG CmusikPropTreeItem::GetHeight()
{
	return PROPTREEITEM_DEFHEIGHT;
}

///////////////////////////////////////////////////

LPARAM CmusikPropTreeItem::GetItemValue()
{
	// no items are assocatied with this type
	return 0L;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::SetItemValue(LPARAM)
{
	// no items are assocatied with this type
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::OnMove()
{
	// no attributes, do nothing
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::OnRefresh()
{
	// no attributes, do nothing
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::OnCommit()
{
	// no attributes, do nothing
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::Activate()
{
	m_bActivated = TRUE;
	m_bCommitOnce = FALSE;

	OnActivate();
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::Deactivate()
{
	CommitChanges();
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::CommitChanges()
{
	m_bActivated = FALSE;

	if (m_bCommitOnce)
		return;

	m_bCommitOnce = TRUE;

	ASSERT(m_pProp!=NULL);

	OnCommit();

	m_pProp->SendNotify(PTN_ITEMCHANGED, this);
	m_pProp->RefreshItems(this);
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::OnActivate()
{
	// no attributes, do nothing
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::SetPropOwner(CmusikPropTree* pProp)
{
	m_pProp = pProp;
}

///////////////////////////////////////////////////

const POINT& CmusikPropTreeItem::GetLocation()
{
	return m_loc;
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikPropTreeItem::GetParent()
{
	return m_pParent;
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikPropTreeItem::GetSibling()
{
	return m_pSibling;
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikPropTreeItem::GetChild()
{
	return m_pChild;
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikPropTreeItem::GetNextVisible()
{
	return m_pVis;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::SetParent(CmusikPropTreeItem* pParent)
{
	m_pParent = pParent;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::SetSibling(CmusikPropTreeItem* pSibling)
{
	m_pSibling = pSibling;
}

///////////////////////////////////////////////////


void CmusikPropTreeItem::SetChild(CmusikPropTreeItem* pChild)
{
	m_pChild = pChild;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::SetNextVisible(CmusikPropTreeItem* pVis)
{
	m_pVis = pVis;
}

///////////////////////////////////////////////////

LONG CmusikPropTreeItem::DrawItem( CDC* pDC, const RECT& rc, LONG x, LONG y )
{
	CPoint pt;
	LONG nTotal, nCol, ey;
	CRect drc, ir;

	ASSERT( m_pProp!=NULL );

	// Add TreeItem the list of visble items
	m_pProp->AddToVisibleList(this);

	// store the item's location
	m_loc = CPoint(x, y);

	// store the items rectangle position
	m_rc.SetRect(PROPTREEITEM_SPACE, m_loc.y, rc.right, m_loc.y + GetHeight()-1);
	m_rc.OffsetRect(0, -m_pProp->GetOrigin());

	// init temp drawing variables
	nTotal = GetHeight();
	ey = (nTotal >> 1) - (PROPTREEITEM_EXPANDBOX >> 1) - 2;

	// convert item coordinates to screen coordinates
	pt = m_loc;
	pt.y -= m_pProp->GetOrigin();
	nCol = 0;

	// draws the label the whole size
	drc.SetRect(pt.x + PROPTREEITEM_EXPANDCOLUMN, pt.y, rc.right, pt.y + nTotal);

	// root level items have a dark, colored background
	if (IsRootLevel())
	{
		// SH 2004-01-03 GDI leak cleanup : was previously plain old Win32 CreateSolidBrush without any variable
		// assignment and cleanup. Changed to CBrush, as its destructor handles cleanup properly.

		CBrush newBrush;
		newBrush.CreateSolidBrush( m_pProp->m_Prefs->MUSIK_COLOR_INACTIVECAPTION );
		HGDIOBJ hOld = pDC->SelectObject( newBrush );
		pDC->PatBlt(rc.left, drc.top, rc.right - rc.left + 1, drc.Height(), PATCOPY);
		pDC->SelectObject(hOld);
	}

	// children have a light colored with a border
	else
	{
		HGDIOBJ hOld;

		// SH 2004-01-03 GDI leak cleanup : was previously plain old Win32 CreateSolidBrush without any variable
		// assignment and cleanup. Changed to CBrush, as its destructor handles cleanup properly.

		CBrush listBrush, gutterBrush;

		//the object

		listBrush.CreateSolidBrush( m_pProp->m_Prefs->MUSIK_COLOR_LISTCTRL );
		hOld = pDC->SelectObject( listBrush );
		pDC->PatBlt(rc.left, drc.top, rc.right - rc.left + 1, drc.Height(), PATCOPY);

		pDC->SelectObject(hOld);
		//the gutter
		if ( IsSelected() )
		{
			gutterBrush.CreateSolidBrush( m_pProp->m_Prefs->MUSIK_COLOR_ACTIVECAPTION );
			hOld = pDC->SelectObject( gutterBrush );
			pDC->PatBlt( rc.left, drc.top, 8, drc.Height(), PATCOPY);
		}
		else if ( IsHovered() )
		{

			gutterBrush.CreateSolidBrush( m_pProp->m_Prefs->MUSIK_COLOR_INACTIVECAPTION );
			hOld = pDC->SelectObject( gutterBrush );
			pDC->PatBlt( rc.left, drc.top, 8, drc.Height(), PATCOPY);
		}
		else
		{
			gutterBrush.CreateSolidBrush( m_pProp->m_Prefs->MUSIK_COLOR_BTNFACE );
			hOld = pDC->SelectObject( gutterBrush );
			pDC->PatBlt( rc.left, drc.top, 8, drc.Height(), PATCOPY);
		}

		pDC->SelectObject(hOld);
		
	}

	// draw the show / expand (+/-) box
	if (GetChild())
	{
		m_rcExpand.left = PROPTREEITEM_EXPANDCOLUMN/2 - PROPTREEITEM_EXPANDBOXHALF;
		m_rcExpand.top = m_loc.y + ey;
		m_rcExpand.right = m_rcExpand.left + PROPTREEITEM_EXPANDBOX - 1;
		m_rcExpand.bottom = m_rcExpand.top + PROPTREEITEM_EXPANDBOX - 1;

		ir = m_rcExpand;
		ir.OffsetRect(0, -m_pProp->GetOrigin());
		_DrawExpand( pDC->m_hDC, ir.left, ir.top, IsExpanded(), true );
	}
	else
		m_rcExpand.SetRectEmpty();

	HRGN hRgn = NULL;

	// calc label position
	ir = drc;
	ir.left += PROPTREEITEM_SPACE;
	
	// draw label
	if ( !m_sLabel.IsEmpty() )
	{
		if (IsRootLevel() || IsSelected())
		{
			pDC->SelectObject( CmusikPropTree::GetBoldFont() );
			if ( IsRootLevel() )
				pDC->SetTextColor( m_pProp->m_Prefs->MUSIK_COLOR_CAPTIONTEXT );
			else
				pDC->SetTextColor( m_pProp->m_Prefs->MUSIK_COLOR_LISTCTRLTEXT );
		}
		else
		{
			pDC->SelectObject( CmusikPropTree::GetNormalFont() );
			pDC->SetTextColor( m_pProp->m_Prefs->MUSIK_COLOR_LISTCTRLTEXT );
		}

		
		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(m_sLabel, &ir, DT_SINGLELINE|DT_VCENTER|DT_CALCRECT);

		// draw the text highlighted if selected
		if (IsSelected())
		{
			HGDIOBJ oPen = pDC->SelectObject(GetStockObject(NULL_PEN));

			CRect dr;
			dr = drc;
			dr.left = PROPTREEITEM_EXPANDCOLUMN;
			
			pDC->Rectangle(&dr);
			
			pDC->SelectObject(oPen);

			if ( !IsRootLevel() )
				pDC->SetTextColor( m_pProp->m_Prefs->MUSIK_COLOR_LISTCTRLTEXT );
		}

		// check if we need to draw the text as disabled
		if (!m_pProp->IsWindowEnabled())
			pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));

		pDC->DrawText(m_sLabel, &ir, DT_SINGLELINE|DT_VCENTER);

	}

	// remove clip region
	if (hRgn)
	{
		SelectClipRgn(pDC->m_hDC, NULL);
		DeleteObject(hRgn);
	}	

	// draw horzontal sep
	// top and bottom
	if ( IsSelected() )
	{
		HLine( pDC->m_hDC, 8, pt.y, rc.right, m_pProp->m_Prefs->MUSIK_COLOR_BTNFACE );
		HLine( pDC->m_hDC, 8, pt.y + nTotal - 1, rc.right, m_pProp->m_Prefs->MUSIK_COLOR_BTNFACE );
	}
	else if ( IsRootLevel() )
		HLine( pDC->m_hDC, 0, pt.y + nTotal - 1, rc.right, m_pProp->m_Prefs->MUSIK_COLOR_BTNFACE );

	// draw children
	if (GetChild() && IsExpanded())
	{
		y += nTotal;

		CmusikPropTreeItem* pNext;

		for (pNext = GetChild(); pNext; pNext = pNext->GetSibling())
		{
			LONG nHeight = pNext->DrawItem(pDC, rc, x + (IsRootLevel() ? 0 : PNINDENT), y);
			nTotal += nHeight;
			y += nHeight;
		}
	}

	return nTotal;
}

///////////////////////////////////////////////////

void CmusikPropTreeItem::DrawAttribute(CDC*, const RECT&)
{
	// no attributes are assocatied with this type
}

///////////////////////////////////////////////////
