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
#include "musikPrefs.h"

#include "../musikCore/include/musikPlayer.h"
#include ".\musiktagdlg.h"

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
}

///////////////////////////////////////////////////

CmusikTagDlg::~CmusikTagDlg()
{
}

///////////////////////////////////////////////////

void CmusikTagDlg::UpdateSel( int index )
{
	if ( m_Playlist && m_Library )
	{
		if ( index < 0 || index <= (int)m_Playlist->GetCount() )
		{
			m_SongIndex = index;
			m_Library->GetSongInfoFromID( m_Playlist->GetSongID( index ), &m_Song );		
			UpdateDlg();
		}
	}
}

///////////////////////////////////////////////////

void CmusikTagDlg::UpdateDlg()
{
	CStatic* ptrCurr;
	CString temp;

	ptrCurr = (CStatic*)GetDlgItem( IDC_FILENAME );
	ptrCurr->SetWindowText( m_Song.GetFilename() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_BITRATE );
	ptrCurr->SetWindowText( m_Song.GetBitrate() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_TITLE );
	ptrCurr->SetWindowText( m_Song.GetTitle() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_ALBUM );
	ptrCurr->SetWindowText( m_Song.GetAlbum() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_ARTIST );
	ptrCurr->SetWindowText( m_Song.GetArtist() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_TRACK );
	ptrCurr->SetWindowText( m_Song.GetTrackNum() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_YEAR );
	ptrCurr->SetWindowText( m_Song.GetYear() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_TIMEADDED );
	ptrCurr->SetWindowText( m_Song.GetTimeAdded() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_LASTPLAYED );
	temp = m_Song.GetLastPlayed();
	if ( temp.IsEmpty() )
		temp = _T( "never" );
	ptrCurr->SetWindowText( temp );

	CComboBox* cmbtemp = (CComboBox*)GetDlgItem( IDC_RATING );
	int sel = 0;
	temp = m_Song.GetRating();
	if ( temp == _T( "0" ) )
		temp = _T( "Unrated" );
	if ( !cmbtemp->SelectString( -1, temp ) )
		cmbtemp->SelectString( -1, _T( "Unrated" ) );

	ptrCurr = (CStatic*)GetDlgItem( IDC_TIMESPLAYED );
	ptrCurr->SetWindowText( m_Song.GetTimesPlayed() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_GENRE );
	ptrCurr->SetWindowText( m_Song.GetGenre() );

	ptrCurr = (CStatic*)GetDlgItem( IDC_DURATION );
	ptrCurr->SetWindowText( CmusikPlayer::GetTimeStr( StringToInt( m_Song.GetDuration() ) ) );

	ptrCurr = (CStatic*)GetDlgItem( IDC_FORMAT );
	int nFormat = StringToInt( m_Song.GetFormat() );
	switch ( nFormat )
	{
	case MUSIK_LIBRARY_FORMAT_MP3:
		ptrCurr->SetWindowText( _T( "MPEG Layer 3" ) );
		break;

	case MUSIK_LIBRARY_FORMAT_OGG:
		ptrCurr->SetWindowText( _T( "OGG Vorbis" ) );
		break;

	default:
		ptrCurr->SetWindowText( _T( "Unknown" ) );
		break;
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
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
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

BOOL CmusikTagDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CComboBox* rating = (CComboBox*)GetDlgItem( IDC_RATING );
	rating->AddString( _T( "Unrated" ) );
	rating->AddString( _T( "1" ) );
	rating->AddString( _T( "2" ) );
	rating->AddString( _T( "3" ) );
    rating->AddString( _T( "4" ) );
	rating->AddString( _T( "5" ) );

	return TRUE;
}

///////////////////////////////////////////////////

void CmusikTagDlg::OnBnClickedClose()
{
	OnClose();
}

///////////////////////////////////////////////////