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
//   CmusikTagDlg
//
// Filename(s): 
//
//   musikTagDlg.h, musikTagDlg.cpp
//
// Information:
//
//   Dialog used for tag editing
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikTagDlg.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikTagDlg, CDialog)

///////////////////////////////////////////////////

CmusikTagDlg::CmusikTagDlg( CWnd* parent, CmusikPlaylist* playlist, CmusikLibrary* library )
	: CDialog(CmusikTagDlg::IDD, parent)
{
	m_Playlist = playlist;
	m_Library = library;
	m_Parent = parent;
	m_SongIndex = 0;

	UpdateSongInfo();
}

///////////////////////////////////////////////////

CmusikTagDlg::~CmusikTagDlg()
{
}

///////////////////////////////////////////////////

void CmusikTagDlg::UpdateSongInfo( int item )
{
	if ( m_Playlist && m_Library )
		m_SongIndex = item;
}

///////////////////////////////////////////////////

void CmusikTagDlg::UpdateSel( int index )
{
	if ( m_Playlist && index < 0 || index <= (int)m_Playlist->GetCount() )
	{
		
	}
}

///////////////////////////////////////////////////

void CmusikTagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikTagDlg, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikTagDlg::OnClose()
{
	int WM_TAGPROPERTIESDESTROY = RegisterWindowMessage( "TAGPROPERTIESDESTROY" );
	m_Parent->SendMessage( WM_TAGPROPERTIESDESTROY );
}

///////////////////////////////////////////////////

BOOL CmusikTagDlg::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN )
	{
		if ( pMsg->wParam == VK_ESCAPE )
		{
			OnClose();
			return true;
		}
	}

	return CDialog::PreTranslateMessage( pMsg );
}

///////////////////////////////////////////////////


