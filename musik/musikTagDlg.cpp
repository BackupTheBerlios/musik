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

#include "id3/globals.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikTagDlg, CDialog)

///////////////////////////////////////////////////

CmusikTagDlg::CmusikTagDlg( CWnd* parent, CmusikPlaylist* playlist, CmusikLibrary* library )
	: CDialog(CmusikTagDlg::IDD, parent)
{
	m_Playlist = playlist;
	m_Library = library;
	m_Parent = parent;
	m_Modified = false;
	m_Updating = false;
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
	m_Updating = true;

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

	ptrCurr = (CStatic*)GetDlgItem( IDC_APPLY );
	ptrCurr->EnableWindow( FALSE );

	m_Updating = false;
	m_Modified = false;
}

///////////////////////////////////////////////////

void CmusikTagDlg::SaveCurr()
{
	m_Updating = true;
	if ( m_Modified )
	{
		CString curr;
		CWnd* ptrCurr;

		// get new info
		ptrCurr = (CStatic*)GetDlgItem( IDC_TITLE );
		ptrCurr->GetWindowText( curr );
		m_Song.SetTitle( curr.GetBuffer() );

		ptrCurr = (CStatic*)GetDlgItem( IDC_TRACK );
		ptrCurr->GetWindowText( curr );
		m_Song.SetTrackNum( curr.GetBuffer() );

		ptrCurr = (CStatic*)GetDlgItem( IDC_ARTIST );
		ptrCurr->GetWindowText( curr );
		m_Song.SetArtist( curr.GetBuffer() );

		ptrCurr = (CStatic*)GetDlgItem( IDC_ALBUM );
		ptrCurr->GetWindowText( curr );
		m_Song.SetAlbum( curr.GetBuffer() );

		ptrCurr = (CStatic*)GetDlgItem( IDC_GENRE );
		ptrCurr->GetWindowText( curr );
		m_Song.SetGenre( curr.GetBuffer() );

		ptrCurr = (CStatic*)GetDlgItem( IDC_YEAR );
		ptrCurr->GetWindowText( curr );
		m_Song.SetYear( curr.GetBuffer() );

		ptrCurr = (CStatic*)GetDlgItem( IDC_RATING );
		ptrCurr->GetWindowText( curr );
		if ( curr == _T( "Unrated" ) )
			curr = _T( "0" );
		m_Song.SetRating( curr.GetBuffer() );

		ptrCurr = (CStatic*)GetDlgItem( IDC_APPLY );
		ptrCurr->EnableWindow( FALSE );

		// save it
		m_Library->SetSongInfo( &m_Song );

		// send a message to update the playlist ctrl
		int WM_TAGUPDATE = RegisterWindowMessage( _T( "TAGUPDATE" ) );
		m_Parent->PostMessage( WM_TAGUPDATE, (WPARAM)m_Song.GetID() );

		m_Modified = false;
	}
	m_Updating = false;
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
	ON_BN_CLICKED(IDC_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(IDC_PREV, OnBnClickedPrev)
	ON_EN_CHANGE(IDC_TITLE, OnChangeField)
	ON_EN_CHANGE(IDC_TRACK, OnChangeField)
	ON_EN_CHANGE(IDC_ARTIST, OnChangeField)
	ON_EN_CHANGE(IDC_ALBUM, OnChangeField)
	ON_EN_CHANGE(IDC_YEAR, OnChangeField)
	ON_CBN_EDITCHANGE(IDC_GENRE, OnChangeField)
	ON_CBN_EDITCHANGE(IDC_RATING, OnChangeField)
	ON_CBN_SELCHANGE(IDC_GENRE, OnChangeField)
	ON_CBN_SELCHANGE(IDC_RATING, OnChangeField)
	ON_BN_CLICKED(IDC_APPLY, OnBnClickedApply)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikTagDlg::OnClose()
{
	int WM_TAGPROPERTIESDESTROY = RegisterWindowMessage( "TAGPROPERTIESDESTROY" );
	m_Parent->PostMessage( WM_TAGPROPERTIESDESTROY );
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
		else
		{
			if ( pMsg->wParam == VK_RETURN )
			{
				if ( GetKeyState( VK_SHIFT ) < 0 )
					OnShiftEnter();
				else
					OnEnter();
			
				return true;
			}
		}
	}

	return CDialog::PreTranslateMessage( pMsg );
}

///////////////////////////////////////////////////

BOOL CmusikTagDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CComboBox* combo = (CComboBox*)GetDlgItem( IDC_RATING );
	combo->AddString( _T( "Unrated" ) );
	combo->AddString( _T( "1" ) );
	combo->AddString( _T( "2" ) );
	combo->AddString( _T( "3" ) );
    combo->AddString( _T( "4" ) );
	combo->AddString( _T( "5" ) );

	combo = (CComboBox*)GetDlgItem( IDC_GENRE );
	for ( int i = 0; i < ID3_NR_OF_V1_GENRES; i++ )
		combo->AddString( ID3_v1_genre_description[i] );

	return TRUE;
}

///////////////////////////////////////////////////

void CmusikTagDlg::OnBnClickedClose()
{
	OnClose();
}

///////////////////////////////////////////////////

void CmusikTagDlg::OnBnClickedNext()
{
	PromptSave();
	int WM_TAGNEXT = RegisterWindowMessage( "TAGNEXT" );
	m_Parent->SendMessage( WM_TAGNEXT );
}

///////////////////////////////////////////////////

void CmusikTagDlg::OnBnClickedPrev()
{
	PromptSave();
	int WM_TAGPREV = RegisterWindowMessage( "TAGPREV" );
	m_Parent->SendMessage( WM_TAGPREV );
}

///////////////////////////////////////////////////

void CmusikTagDlg::OnEnter()
{
	SaveCurr();
	int WM_TAGNEXT = RegisterWindowMessage( "TAGNEXT" );
	m_Parent->SendMessage( WM_TAGNEXT );
}

///////////////////////////////////////////////////

void CmusikTagDlg::OnShiftEnter()
{
	SaveCurr();
	int WM_TAGPREV = RegisterWindowMessage( "TAGPREV" );
	m_Parent->SendMessage( WM_TAGPREV );
}

///////////////////////////////////////////////////

void CmusikTagDlg::PromptSave()
{
	if ( m_Modified )
	{
		if ( MessageBox( _T( "Save current changes to song information?" ), MUSIK_VERSION_STR, MB_YESNO ) == IDYES )
			SaveCurr();
	}
}

///////////////////////////////////////////////////

void CmusikTagDlg::OnChangeField()
{
	if ( !m_Updating && !m_Modified )
	{
		CWnd* ptrCurr = (CWnd*)GetDlgItem( IDC_APPLY );
		ptrCurr->EnableWindow( TRUE );

		m_Modified = true;
	}
}

///////////////////////////////////////////////////

void CmusikTagDlg::OnBnClickedApply()
{
	SaveCurr();
}

///////////////////////////////////////////////////


