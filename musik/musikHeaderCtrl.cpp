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
// Class(s): 
//
//   CmusikHeaderCtrl
//
// Filename(s): 
//
//   musikHeaderCtrl.h, musikHeaderCtrl.cpp
//
// Information:
//
//   A custom header control that allows drag and drop rearranging
//   to replace the one that was done very poorly in MFC
// 
// Usage: 
//
//   CmusikListCtrl will subclass the default header with this one,
//   so integration should be seamless.
//
// Original Credit:
//
//   http://www.codeguru.com/listview/drag_col.shtml
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikHeaderCtrl.h"

#include "MEMDC.H"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikHeaderCtrl, CHeaderCtrl)

///////////////////////////////////////////////////

CmusikHeaderCtrl::CmusikHeaderCtrl()
{
	m_pWidth		= NULL;
	m_bDragging		= FALSE;
	m_bCheckForDrag = FALSE;
	m_fpDragCol		= NULL;
	m_pOwnerWnd		= NULL;
	m_bEnableDND	= TRUE;
	m_bFirstRun		= TRUE;
}

///////////////////////////////////////////////////

CmusikHeaderCtrl::CmusikHeaderCtrl( CWnd* pWnd, void (CWnd::*fpDragCol)( int, int ) )
{
	m_pWidth		= NULL;
	m_bDragging		= FALSE;
	m_bCheckForDrag = FALSE;
	m_fpDragCol		= fpDragCol;
	m_pOwnerWnd		= pWnd;
	m_bFirstRun		= TRUE;
}

///////////////////////////////////////////////////

CmusikHeaderCtrl::~CmusikHeaderCtrl()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikHeaderCtrl, CHeaderCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikHeaderCtrl::OnMouseMove( UINT nFlags, CPoint point ) 
{
	if ( m_bEnableDND )
	{
		if( ( MK_LBUTTON & nFlags ) == 0)
		{
			// The left mouse button is not pressed - so reset flags
			m_bCheckForDrag = FALSE;
			m_bDragging = FALSE;
		}

		else if( m_bDragging )
		{
			// Get column number that falls under the mouse
			int i = 0; 
			int cx = 0;

			if( point.x > 0 )
			{
				for( i = 0; i < GetItemCount(); i++ )
				{
					if( point.x >= cx && point.x < cx + m_pWidth[i] )
						break;
					cx += m_pWidth[i];
				}
			}

			if( i != m_nDropPos )
			{
				m_nDropPos = i;

				CRect rect;
				GetWindowRect( &rect );

				// invalidate the last known rect, but
				// if its the first run, no rect exists,
				// so there is nothing to invalidate
				if ( m_bFirstRun )
					m_bFirstRun = false;
				else
					InvalidateRect( &marker_rect );

				// Draw a new marker
				CClientDC dc(this);
			
				// draw the bar...
				CRect temp_rect;
				temp_rect.left = cx - 1;
				temp_rect.top = 0;
				temp_rect.right = cx + 1;
				temp_rect.bottom = rect.Height();
				dc.FillSolidRect( temp_rect, GetSysColor( COLOR_ACTIVECAPTION ) );

				// store for later use...
				marker_rect = temp_rect;
			}

			return;
		}

		else if( m_bCheckForDrag )
		{
			// The mouse button was pressed over a column header
			// and now the mouse has moved - so start drag
			m_bCheckForDrag = FALSE;

			m_bDragging = TRUE;
			m_nDropPos = m_nDragCol;

			SetCapture();

			// Store information for later use
			int iCount = GetItemCount();
			HD_ITEM hd_item;
			m_pWidth = new int[iCount];
			for( int i = 0; i < iCount; i++ )
			{
				hd_item.mask = HDI_WIDTH;
				GetItem( i, &hd_item );
				m_pWidth[i] = hd_item.cxy;
			}

			return;
		}
	}
	
	CHeaderCtrl::OnMouseMove( nFlags, point );
}

///////////////////////////////////////////////////

void CmusikHeaderCtrl::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	ASSERT( m_pOwnerWnd != NULL && m_fpDragCol != NULL );

	if ( m_bEnableDND )
	{
		if( m_bDragging )
		{
			m_bDragging = FALSE;
			delete[] m_pWidth;
			ReleaseCapture();
			Invalidate();

			// Call the callback function.
			if( m_nDragCol != m_nDropPos && m_nDragCol != m_nDropPos -1 )
				( m_pOwnerWnd->*m_fpDragCol )( m_nDragCol, m_nDropPos );
		}
	}

	CHeaderCtrl::OnLButtonUp(nFlags, point);
}

///////////////////////////////////////////////////

void CmusikHeaderCtrl::SetCallback( CWnd* pWnd, void (CWnd::*fpDragCol)(int, int) )
{
	m_fpDragCol = fpDragCol;
	m_pOwnerWnd = pWnd;
}

///////////////////////////////////////////////////

void CmusikHeaderCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ( m_bEnableDND )
	{
		// Determine if mouse was pressed over a column header
		HD_HITTESTINFO hd_hittestinfo;
		hd_hittestinfo.pt = point;

		SendMessage(HDM_HITTEST, 0, (LPARAM)(&hd_hittestinfo));
		if( hd_hittestinfo.flags == HHT_ONHEADER )
		{
			m_nDragCol = hd_hittestinfo.iItem;
			m_bCheckForDrag = TRUE;
		}
	}
	
	CHeaderCtrl::OnLButtonDown(nFlags, point);
}

///////////////////////////////////////////////////