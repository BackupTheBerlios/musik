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

#include "../../Musik.Core/include/MusikPlaylist.h"
#include "../MusikPrefs.h"

#include "MusikPropTree.h"
#include "MusikPropTreeItem.h"

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

CMusikPropTreeItem::CMusikPropTreeItem() :
	m_pProp(NULL),
	m_sLabel(_T("")),
	m_loc(0,0),
	m_rc(0,0,0,0),
	m_lParam(0),
	m_nCtrlID(0),
	m_dwState(0),
	m_bActivated(FALSE),
	m_bCommitOnce(FALSE),
	m_bMouseOver(FALSE),
	m_rcExpand(0,0,0,0),
	m_pParent(NULL),
	m_pSibling(NULL),
	m_pChild(NULL),
	m_pVis(NULL)
{
	m_PlaylistInfo = new CMusikPlaylistInfo();
}

///////////////////////////////////////////////////

CMusikPropTreeItem::~CMusikPropTreeItem()
{
	if ( m_PlaylistInfo )
		delete m_PlaylistInfo;
}

///////////////////////////////////////////////////

void CMusikPropTreeItem::SetPlaylistInfo( const CMusikPlaylistInfo info )
{
	if ( m_PlaylistInfo )
	{
		*m_PlaylistInfo = info;

		SetLabelText( m_PlaylistInfo->GetName() );
	}
}

///////////////////////////////////////////////////

int CMusikPropTreeItem::GetPlaylistType()
{
	return m_PlaylistInfo->GetType();
}

///////////////////////////////////////////////////

int CMusikPropTreeItem::GetPlaylistID()
{
	return m_PlaylistInfo->GetID();
}

///////////////////////////////////////////////////

BOOL CMusikPropTreeItem::IsExpanded()
{
	return (m_dwState & TreeItemExpanded) ? TRUE : FALSE;
}

///////////////////////////////////////////////////

BOOL CMusikPropTreeItem::IsSelected()
{
	return (m_dwState & TreeItemSelected) ? TRUE : FALSE;
}

///////////////////////////////////////////////////

BOOL CMusikPropTreeItem::IsMouseOver()
{
	return m_bMouseOver;
}

///////////////////////////////////////////////////

void CMusikPropTreeItem::SetMouseOver( BOOL bMouseOver )
{
	m_bMouseOver = bMouseOver;
}

///////////////////////////////////////////////////

BOOL CMusikPropTreeItem::IsActivated()
{
	return (m_dwState & TreeItemActivated) ? TRUE : FALSE;
}

///////////////////////////////////////////////////

void CMusikPropTreeItem::Select(BOOL bSelect)
{
	if (bSelect)
		m_dwState |= TreeItemSelected;
	else
		m_dwState &= ~TreeItemSelected;
}

///////////////////////////////////////////////////

void CMusikPropTreeItem::Expand(BOOL bExpand)
{
	if (bExpand)
		m_dwState |= TreeItemExpanded;
	else
		m_dwState &= ~TreeItemExpanded;
}

///////////////////////////////////////////////////

BOOL CMusikPropTreeItem::HitExpand(const POINT& pt)
{
	return m_rcExpand.PtInRect(pt);
}

///////////////////////////////////////////////////

BOOL CMusikPropTreeItem::IsRootLevel()
{
	ASSERT(m_pProp!=NULL);
	return GetParent() == m_pProp->GetRootItem();
}

///////////////////////////////////////////////////

LONG CMusikPropTreeItem::GetTotalHeight()
{
	CMusikPropTreeItem* pItem;
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

void CMusikPropTreeItem::SetLabelText(LPCTSTR sLabel)
{
	m_sLabel = sLabel;
}

///////////////////////////////////////////////////

LPCTSTR CMusikPropTreeItem::GetLabelText()
{
	return m_sLabel;
}

///////////////////////////////////////////////////

void CMusikPropTreeItem::SetCtrlID(UINT nCtrlID)
{
	m_nCtrlID = nCtrlID;
}

///////////////////////////////////////////////////

UINT CMusikPropTreeItem::GetCtrlID()
{
	return m_nCtrlID;
}

///////////////////////////////////////////////////

LONG CMusikPropTreeItem::GetHeight()
{
	return PROPTREEITEM_DEFHEIGHT;
}

///////////////////////////////////////////////////

LPARAM CMusikPropTreeItem::GetItemValue()
{
	// no items are assocatied with this type
	return 0L;
}

///////////////////////////////////////////////////

void CMusikPropTreeItem::SetItemValue(LPARAM)
{
	// no items are assocatied with this type
}

///////////////////////////////////////////////////

void CMusikPropTreeItem::OnMove()
{
	// no attributes, do nothing
}

///////////////////////////////////////////////////

void CMusikPropTreeItem::OnRefresh()
{
	// no attributes, do nothing
}

///////////////////////////////////////////////////

void CMusikPropTreeItem::OnCommit()
{
	// no attributes, do nothing
}

///////////////////////////////////////////////////

void CMusikPropTreeItem::Activate()
{
	m_bActivated = TRUE;
	m_bCommitOnce = FALSE;

	OnActivate();
}

///////////////////////////////////////////////////

void CMusikPropTreeItem::Deactivate()
{
	CommitChanges();
}

///////////////////////////////////////////////////

void CMusikPropTreeItem::CommitChanges()
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

void CMusikPropTreeItem::OnActivate()
{
	// no attributes, do nothing
}

///////////////////////////////////////////////////

void CMusikPropTreeItem::SetPropOwner(CMusikPropTree* pProp)
{
	m_pProp = pProp;
}

///////////////////////////////////////////////////

const POINT& CMusikPropTreeItem::GetLocation()
{
	return m_loc;
}

///////////////////////////////////////////////////

CMusikPropTreeItem* CMusikPropTreeItem::GetParent()
{
	return m_pParent;
}

///////////////////////////////////////////////////

CMusikPropTreeItem* CMusikPropTreeItem::GetSibling()
{
	return m_pSibling;
}

///////////////////////////////////////////////////

CMusikPropTreeItem* CMusikPropTreeItem::GetChild()
{
	return m_pChild;
}

///////////////////////////////////////////////////

CMusikPropTreeItem* CMusikPropTreeItem::GetNextVisible()
{
	return m_pVis;
}

///////////////////////////////////////////////////

void CMusikPropTreeItem::SetParent(CMusikPropTreeItem* pParent)
{
	m_pParent = pParent;
}

///////////////////////////////////////////////////

void CMusikPropTreeItem::SetSibling(CMusikPropTreeItem* pSibling)
{
	m_pSibling = pSibling;
}

///////////////////////////////////////////////////


void CMusikPropTreeItem::SetChild(CMusikPropTreeItem* pChild)
{
	m_pChild = pChild;
}

///////////////////////////////////////////////////

void CMusikPropTreeItem::SetNextVisible(CMusikPropTreeItem* pVis)
{
	m_pVis = pVis;
}

///////////////////////////////////////////////////

LONG CMusikPropTreeItem::DrawItem( CDC* pDC, const RECT& rc, LONG x, LONG y )
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
		HGDIOBJ hOld = pDC->SelectObject( CreateSolidBrush( m_pProp->m_Prefs->MUSIK_COLOR_INACTIVECAPTION ) );
		pDC->PatBlt(rc.left, drc.top, rc.right - rc.left + 1, drc.Height(), PATCOPY);
		pDC->SelectObject(hOld);
	}

	// children have a light colored with a border
	else
	{
		HGDIOBJ hOld;

		//the object
		hOld = pDC->SelectObject( CreateSolidBrush( m_pProp->m_Prefs->MUSIK_COLOR_LISTCTRL ) );
		pDC->PatBlt(rc.left + 14, drc.top, rc.right - rc.left + 1, drc.Height(), PATCOPY);

		//the gutter
		if ( IsSelected() )
		{
			hOld = pDC->SelectObject( CreateSolidBrush( m_pProp->m_Prefs->MUSIK_COLOR_ACTIVECAPTION ) );
			pDC->PatBlt( rc.left, drc.top, 8, drc.Height(), PATCOPY);
		}
		else if ( IsMouseOver() )
		{
			hOld = pDC->SelectObject( CreateSolidBrush( m_pProp->m_Prefs->MUSIK_COLOR_INACTIVECAPTION ) );
			pDC->PatBlt( rc.left, drc.top, 8, drc.Height(), PATCOPY);
		}
		else
		{
			hOld = pDC->SelectObject( CreateSolidBrush( m_pProp->m_Prefs->MUSIK_COLOR_BTNFACE ) );
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
			pDC->SelectObject( CMusikPropTree::GetBoldFont() );
			if ( IsRootLevel() )
				pDC->SetTextColor( m_pProp->m_Prefs->MUSIK_COLOR_CAPTIONTEXT );
			else
				pDC->SetTextColor( m_pProp->m_Prefs->MUSIK_COLOR_LISTCTRLTEXT );
		}
		else
		{
			pDC->SelectObject( CMusikPropTree::GetNormalFont() );
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

		CMusikPropTreeItem* pNext;

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

void CMusikPropTreeItem::DrawAttribute(CDC*, const RECT&)
{
	// no attributes are assocatied with this type
}

///////////////////////////////////////////////////
