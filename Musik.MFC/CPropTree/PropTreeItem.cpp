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
//  Modified by Casey Langen for Musik MFC port...
//  Rather than linking to a dll or external lib, it compiles in.

#include "stdafx.h"
#include "PropTree.h"

#include "PropTreeItem.h"

#define PROPTREEITEM_DEFHEIGHT			21			// default heigt of an item
#define PROPTREEITEM_SPACE				5			// default horz spacing
#define PROPTREEITEM_EXPANDBOX			9			// size of the expand box
#define PROPTREEITEM_EXPANDCOLUMN		16			// width of the expand column
#define PNINDENT						16			// child level indent

#define PROPTREEITEM_EXPANDBOXHALF		(PROPTREEITEM_EXPANDBOX/2)


/////////////////////////////////////////////////////////////////////////////
// drawing helper functions
//

// draw a dotted horizontal line
static void HLine(HDC hdc, LONG x, LONG y, LONG w, COLORREF color)
{
	for (; w>0; w--, x++)
		SetPixel(hdc, x, y, color);
	
}


// draw the plus/minus button
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

/////////////////////////////////////////////////////////////////////////////
// CPropTreeItem
//

CPropTreeItem::CPropTreeItem() :
	m_pProp(NULL),
	m_sLabel(_T("")),
	m_sInfo(_T("")),
	m_loc(0,0),
	m_rc(0,0,0,0),
	m_lParam(0),
	m_nCtrlID(0),
	m_dwState(0),
	m_bActivated(FALSE),
	m_bCommitOnce(FALSE),
	m_rcExpand(0,0,0,0),
	m_pParent(NULL),
	m_pSibling(NULL),
	m_pChild(NULL),
	m_pVis(NULL)
{
}


CPropTreeItem::~CPropTreeItem()
{
}


BOOL CPropTreeItem::IsExpanded()
{
	return (m_dwState & TreeItemExpanded) ? TRUE : FALSE;
}


BOOL CPropTreeItem::IsSelected()
{
	return (m_dwState & TreeItemSelected) ? TRUE : FALSE;
}


BOOL CPropTreeItem::IsReadOnly()
{
	return (m_dwState & TreeItemReadOnly) ? TRUE : FALSE;
}


BOOL CPropTreeItem::IsActivated()
{
	return (m_dwState & TreeItemActivated) ? TRUE : FALSE;
}


void CPropTreeItem::Select(BOOL bSelect)
{
	if (bSelect)
		m_dwState |= TreeItemSelected;
	else
		m_dwState &= ~TreeItemSelected;
}


void CPropTreeItem::Expand(BOOL bExpand)
{
	if (bExpand)
		m_dwState |= TreeItemExpanded;
	else
		m_dwState &= ~TreeItemExpanded;
}


void CPropTreeItem::ReadOnly(BOOL bReadOnly)
{
	if (bReadOnly)
		m_dwState |= TreeItemReadOnly;
	else
		m_dwState &= ~TreeItemReadOnly;
}

BOOL CPropTreeItem::HitExpand(const POINT& pt)
{
	return m_rcExpand.PtInRect(pt);
}


BOOL CPropTreeItem::IsRootLevel()
{
	ASSERT(m_pProp!=NULL);
	return GetParent() == m_pProp->GetRootItem();
}


LONG CPropTreeItem::GetTotalHeight()
{
	CPropTreeItem* pItem;
	LONG nHeight;

	nHeight = GetHeight();

	if (IsExpanded())
	{
		for (pItem = GetChild(); pItem; pItem = pItem->GetSibling())
			nHeight += pItem->GetTotalHeight();
	}

	return nHeight;
}


void CPropTreeItem::SetLabelText(LPCTSTR sLabel)
{
	m_sLabel = sLabel;
}


LPCTSTR CPropTreeItem::GetLabelText()
{
	return m_sLabel;
}


void CPropTreeItem::SetInfoText(LPCTSTR sInfo)
{
	m_sInfo = sInfo;
}


LPCTSTR CPropTreeItem::GetInfoText()
{
	return m_sInfo;
}


void CPropTreeItem::SetCtrlID(UINT nCtrlID)
{
	m_nCtrlID = nCtrlID;
}


UINT CPropTreeItem::GetCtrlID()
{
	return m_nCtrlID;
}


LONG CPropTreeItem::GetHeight()
{
	return PROPTREEITEM_DEFHEIGHT;
}


LPARAM CPropTreeItem::GetItemValue()
{
	// no items are assocatied with this type
	return 0L;
}


void CPropTreeItem::SetItemValue(LPARAM)
{
	// no items are assocatied with this type
}


void CPropTreeItem::OnMove()
{
	// no attributes, do nothing
}


void CPropTreeItem::OnRefresh()
{
	// no attributes, do nothing
}


void CPropTreeItem::OnCommit()
{
	// no attributes, do nothing
}


void CPropTreeItem::Activate()
{
	m_bActivated = TRUE;
	m_bCommitOnce = FALSE;

	OnActivate();
}


void CPropTreeItem::CommitChanges()
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


void CPropTreeItem::OnActivate()
{
	// no attributes, do nothing
}


void CPropTreeItem::SetPropOwner(CPropTree* pProp)
{
	m_pProp = pProp;
}


const POINT& CPropTreeItem::GetLocation()
{
	return m_loc;
}


CPropTreeItem* CPropTreeItem::GetParent()
{
	return m_pParent;
}


CPropTreeItem* CPropTreeItem::GetSibling()
{
	return m_pSibling;
}


CPropTreeItem* CPropTreeItem::GetChild()
{
	return m_pChild;
}


CPropTreeItem* CPropTreeItem::GetNextVisible()
{
	return m_pVis;
}


void CPropTreeItem::SetParent(CPropTreeItem* pParent)
{
	m_pParent = pParent;
}


void CPropTreeItem::SetSibling(CPropTreeItem* pSibling)
{
	m_pSibling = pSibling;
}


void CPropTreeItem::SetChild(CPropTreeItem* pChild)
{
	m_pChild = pChild;
}


void CPropTreeItem::SetNextVisible(CPropTreeItem* pVis)
{
	m_pVis = pVis;
}


LONG CPropTreeItem::DrawItem( CDC* pDC, const RECT& rc, LONG x, LONG y )
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
	m_rc.SetRect(m_pProp->GetOrigin().x + PROPTREEITEM_SPACE, m_loc.y, rc.right, m_loc.y + GetHeight()-1);
	m_rc.OffsetRect(0, -m_pProp->GetOrigin().y);

	// init temp drawing variables
	nTotal = GetHeight();
	ey = (nTotal >> 1) - (PROPTREEITEM_EXPANDBOX >> 1) - 2;

	// convert item coordinates to screen coordinates
	pt = m_loc;
	pt.y -= m_pProp->GetOrigin().y;
	nCol = m_pProp->GetOrigin().x;

	// draws the label the whole size
	drc.SetRect(pt.x + PROPTREEITEM_EXPANDCOLUMN, pt.y, rc.right, pt.y + nTotal);

	// root level items have a dark, color'd background
	if (IsRootLevel())
	{
		HGDIOBJ hOld = pDC->SelectObject(GetSysColorBrush(COLOR_ACTIVECAPTION));
		pDC->PatBlt(rc.left, drc.top, rc.right - rc.left + 1, drc.Height(), PATCOPY);
		pDC->SelectObject(hOld);
	}

	// children have a light colored with a border
	else
	{
		HGDIOBJ hOld;

		//the object
		hOld = pDC->SelectObject( GetSysColorBrush(COLOR_BTNHILIGHT) );
		pDC->PatBlt(rc.left + 14, drc.top, rc.right - rc.left + 1, drc.Height(), PATCOPY);

		//the gutter
		hOld = pDC->SelectObject( GetSysColorBrush(COLOR_BTNFACE) );
		pDC->PatBlt( 0, drc.top, 14, drc.Height(), PATCOPY);

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
		ir.OffsetRect(0, -m_pProp->GetOrigin().y);
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
		if (IsRootLevel())
		{
			pDC->SelectObject(CPropTree::GetBoldFont());
			pDC->SetTextColor(GetSysColor(COLOR_BTNHILIGHT));
		}
		else
		{
			pDC->SelectObject(CPropTree::GetNormalFont());
			pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
		}

		
		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(m_sLabel, &ir, DT_SINGLELINE|DT_VCENTER|DT_CALCRECT);

		// draw the text highlighted if selected
		if (IsSelected())
		{
			HGDIOBJ oPen = pDC->SelectObject(GetStockObject(NULL_PEN));
			HGDIOBJ oBrush;
			if ( !IsRootLevel() )
				oBrush = pDC->SelectObject(GetSysColorBrush(COLOR_INFOBK));
			else
				oBrush = pDC->SelectObject(GetSysColorBrush(COLOR_ACTIVECAPTION));
			
			CRect dr;
			dr = drc;
			dr.left = PROPTREEITEM_EXPANDCOLUMN;
			
			pDC->Rectangle(&dr);
			
			pDC->SelectObject(oPen);
			pDC->SelectObject(oBrush);

			if ( !IsRootLevel() )
				pDC->SetTextColor(GetSysColor(COLOR_INFOTEXT));
			else
				pDC->SetTextColor(GetSysColor(COLOR_BTNHIGHLIGHT));
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
	if ( IsRootLevel() )
		HLine( pDC->m_hDC, 0, pt.y + nTotal - 1, rc.right, GetSysColor(COLOR_BTNFACE) );
	else
		HLine( pDC->m_hDC, PROPTREEITEM_EXPANDCOLUMN, pt.y + nTotal - 1, rc.right - PROPTREEITEM_EXPANDCOLUMN + 1, GetSysColor(COLOR_BTNFACE) );

	// draw children
	if (GetChild() && IsExpanded())
	{
		y += nTotal;

		CPropTreeItem* pNext;

		for (pNext = GetChild(); pNext; pNext = pNext->GetSibling())
		{
			LONG nHeight = pNext->DrawItem(pDC, rc, x + (IsRootLevel() ? 0 : PNINDENT), y);
			nTotal += nHeight;
			y += nHeight;
		}
	}

	return nTotal;
}


void CPropTreeItem::DrawAttribute(CDC*, const RECT&)
{
	// no attributes are assocatied with this type
}
