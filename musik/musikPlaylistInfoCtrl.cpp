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
// Class(es): 
//
//   CmusikPlaylistInfoCtrl
//
// Filename(s): 
//
//   musikPlaylistInfoCtrl.h, musikPlaylistInfoCtrl.cpp
//
// Information:
//
//   Shows information about playlist, such as number of songs, size, etc.
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "musik.h"
#include "musikPlaylistView.h"
#include "musikPrefs.h"

#include "../musikCore/include/musikLibrary.h"
#include "../musikCore/include/musikArrays.h"
#include "../musikCore/include/musikPlayer.h"

#include "MEMDC.H"
#include ".\musikplaylistinfoctrl.h"

///////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(CmusikPlaylistInfoCtrl, CWnd)

///////////////////////////////////////////////////

CmusikPlaylistInfoCtrl::CmusikPlaylistInfoCtrl( CmusikPlaylistCtrl* parent, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs )
{
	m_Library = library;
	m_Player = player;
	m_Prefs = prefs;
	m_ListCtrl = parent;
}

///////////////////////////////////////////////////

CmusikPlaylistInfoCtrl::~CmusikPlaylistInfoCtrl()
{
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPlaylistInfoCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikPlaylistInfoCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect( &rect );
	CMemDC memDC( &dc, &rect );

	memDC.FillSolidRect( rect, m_Prefs->MUSIK_COLOR_BTNFACE );
	memDC.Draw3dRect( rect, m_Prefs->MUSIK_COLOR_BTNSHADOW, m_Prefs->MUSIK_COLOR_BTNHILIGHT );

	DefWindowProc( WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0 );
}

///////////////////////////////////////////////////

int CmusikPlaylistInfoCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rcClient;
	GetClientRect( &rcClient );

	
	m_Font.CreateStockObject( DEFAULT_GUI_FONT );
	
	CDC* pDC = GetDC();
	pDC->SetBkColor( GetSysColor( COLOR_BTNHILIGHT ) );
	ReleaseDC( pDC );

	return 0;
}


///////////////////////////////////////////////////

void CmusikPlaylistInfoCtrl::OnSize(UINT nType, int cx, int cy)
{
	
}

///////////////////////////////////////////////////

void CmusikPlaylistInfoCtrl::OnDestroy()
{
	CWnd::OnDestroy();
}

///////////////////////////////////////////////////

BOOL CmusikPlaylistInfoCtrl::OnEraseBkgnd(CDC* pDC)
{
	return false;
}

///////////////////////////////////////////////////
