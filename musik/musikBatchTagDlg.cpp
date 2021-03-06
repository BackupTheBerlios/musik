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
//   CmusikBatchTagDlg
//
// Filename(s): 
//
//   musikBatchTagDlg.h, musikBatchTagDlg.cpp
//
// Information:
//
//   Dialog used for batch tag editing
// 
// Usage: 
//
//   Ment to be modal.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikBatchTagDlg.h"

#include "id3/globals.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikBatchTagDlg, CDialog)

///////////////////////////////////////////////////

CmusikBatchTagDlg::CmusikBatchTagDlg( CWnd* parent, CmusikSongInfoArray* songs )
	: CDialog( CmusikBatchTagDlg::IDD, parent )
{
	m_Songs = songs;
	m_Parent = parent;
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::Apply()
{
	SetWindowText( _T( "Cube Batch Edit (scanning...)" ) );
	if ( m_Songs )
	{
		int title, artist, album, track, genre, year, rating;
		CString title_str, artist_str, album_str, track_str, genre_str, year_str, rating_str;

		CButton* ptrButton;

        ptrButton = (CButton*)GetDlgItem( IDC_ENBTITLE );
		title = ptrButton->GetCheck();
		m_Title.GetWindowText( title_str );

        ptrButton = (CButton*)GetDlgItem( IDC_ENBARTIST );
		artist = ptrButton->GetCheck();
		m_Artist.GetWindowText( artist_str );

        ptrButton = (CButton*)GetDlgItem( IDC_ENBALBUM );
		album = ptrButton->GetCheck();
		m_Album.GetWindowText( album_str );

        ptrButton = (CButton*)GetDlgItem( IDC_ENBTRACK );
		track = ptrButton->GetCheck();
		m_Track.GetWindowText( track_str );

        ptrButton = (CButton*)GetDlgItem( IDC_ENBGENRE );
		genre = ptrButton->GetCheck();
		m_Genre.GetWindowText( genre_str );

        ptrButton = (CButton*)GetDlgItem( IDC_ENBYEAR );
		year = ptrButton->GetCheck();
		m_Year.GetWindowText( year_str );

        ptrButton = (CButton*)GetDlgItem( IDC_ENBRATING );
		rating = ptrButton->GetCheck();
		m_Rating.GetWindowText( rating_str );

		if ( rating_str == _T( "Unrated" ) )
			rating_str = _T( "0" );

		for ( size_t i = 0; i < m_Songs->size(); i++ )
		{
			if ( title )
				m_Songs->at( i ).SetTitle( title_str.GetBuffer() );

			if ( artist )
				m_Songs->at( i ).SetArtist( artist_str.GetBuffer() );

			if ( album )
				m_Songs->at( i ).SetAlbum( album_str.GetBuffer() );

			if ( track )
				m_Songs->at( i ).SetTrackNum( track_str.GetBuffer() );

			if ( genre )
				m_Songs->at( i ).SetGenre( genre_str.GetBuffer() );

			if ( year )
				m_Songs->at( i ).SetYear( year_str.GetBuffer() );

			if ( rating )
				m_Songs->at( i ).SetRating( rating_str.GetBuffer() );
		}
	}
	SetWindowText( _T( "Cube Batch Edit" ) );
}

///////////////////////////////////////////////////

CmusikBatchTagDlg::~CmusikBatchTagDlg()
{
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TITLE, m_Title);
	DDX_Control(pDX, IDC_ARTIST, m_Artist);
	DDX_Control(pDX, IDC_ALBUM, m_Album);
	DDX_Control(pDX, IDC_TRACK, m_Track);
	DDX_Control(pDX, IDC_GENRE, m_Genre);
	DDX_Control(pDX, IDC_YEAR, m_Year);
	DDX_Control(pDX, IDC_RATING, m_Rating);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikBatchTagDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_ENBTITLE, OnBnClickedEnbtitle)
	ON_BN_CLICKED(IDC_ENBARTIST, OnBnClickedEnbartist)
	ON_BN_CLICKED(IDC_ENBALBUM, OnBnClickedEnbalbum)
	ON_BN_CLICKED(IDC_ENBTRACK, OnBnClickedEnbtrack)
	ON_BN_CLICKED(IDC_ENBGENRE, OnBnClickedEnbgenre)
	ON_BN_CLICKED(IDC_ENBYEAR, OnBnClickedEnbyear)
	ON_BN_CLICKED(IDC_ENBRATING, OnBnClickedEnbrating)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedCancel()
{
	OnCancel();
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedOk()
{
	Apply();
	OnOK();
}

///////////////////////////////////////////////////

BOOL CmusikBatchTagDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// combo boxes
	m_Rating.AddString( _T( "Unrated" ) );
	m_Rating.AddString( _T( "1" ) );
	m_Rating.AddString( _T( "2" ) );
	m_Rating.AddString( _T( "3" ) );
    m_Rating.AddString( _T( "4" ) );
	m_Rating.AddString( _T( "5" ) );

	for ( int i = 0; i < ID3_NR_OF_V1_GENRES; i++ )
		m_Genre.AddString( ID3_v1_genre_description[i] );

	// default values
	if ( m_Songs->size() )
	{
		m_Title.SetWindowText( m_Songs->at( 0 ).GetTitle() );
		m_Artist.SetWindowText( m_Songs->at( 0 ).GetArtist() );
		m_Album.SetWindowText( m_Songs->at( 0 ).GetAlbum() );
		m_Track.SetWindowText( m_Songs->at( 0 ).GetTrackNum() );
		m_Genre.SetWindowText( m_Songs->at( 0 ).GetGenre() );
		m_Year.SetWindowText( m_Songs->at( 0 ).GetYear() );

		if ( m_Songs->at( 0 ).GetRating().IsEmpty() || m_Songs->at( 0 ).GetRating() == _T( "0" ) )
			m_Rating.SelectString( -1, _T( "Unrated" ) );
		else
			m_Rating.SelectString( -1, m_Songs->at( 0 ).GetRating() );
	}	

	m_Title.EnableWindow( FALSE );
	m_Artist.EnableWindow( FALSE );
	m_Album.EnableWindow( FALSE );
	m_Track.EnableWindow( FALSE );
	m_Genre.EnableWindow( FALSE );
	m_Year.EnableWindow( FALSE );
	m_Rating.EnableWindow( FALSE );

	return TRUE;
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedEnbtitle()
{
	CButton* ptrButton;
    ptrButton = (CButton*)GetDlgItem( IDC_ENBTITLE );
	if ( ptrButton->GetCheck() )
	{
		m_Title.EnableWindow( TRUE );
		m_Title.SetFocus();

		CString text;
		m_Title.GetWindowText( text );
		m_Title.SetSel( 0, text.GetLength() );
	}
	else
		m_Title.EnableWindow( FALSE );

	CString text;
	m_Title.GetWindowText( text );
	m_Title.SetSel( 0, text.GetLength() );
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedEnbartist()
{
	CButton* ptrButton;
    ptrButton = (CButton*)GetDlgItem( IDC_ENBARTIST );
	if ( ptrButton->GetCheck() )
	{
		m_Artist.EnableWindow( TRUE );
		m_Artist.SetFocus();

		CString text;
		m_Artist.GetWindowText( text );
		m_Artist.SetSel( 0, text.GetLength() );
	}
	else
		m_Artist.EnableWindow( FALSE );


}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedEnbalbum()
{
	CButton* ptrButton;
    ptrButton = (CButton*)GetDlgItem( IDC_ENBALBUM );
	if ( ptrButton->GetCheck() )
	{
		m_Album.EnableWindow( TRUE );
		m_Album.SetFocus();

		CString text;
		m_Album.GetWindowText( text );
		m_Album.SetSel( 0, text.GetLength() );	
	}
	else
		m_Album.EnableWindow( FALSE );
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedEnbtrack()
{
	CButton* ptrButton;
    ptrButton = (CButton*)GetDlgItem( IDC_ENBTRACK );
	if ( ptrButton->GetCheck() )
	{
		m_Track.EnableWindow( TRUE );
		m_Track.SetFocus();

		CString text;
		m_Track.GetWindowText( text );
		m_Track.SetSel( 0, text.GetLength() );	
	}
	else
		m_Track.EnableWindow( FALSE );
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedEnbgenre()
{
	CButton* ptrButton;
    ptrButton = (CButton*)GetDlgItem( IDC_ENBGENRE );
	if ( ptrButton->GetCheck() )
	{
		m_Genre.EnableWindow( TRUE );
		m_Genre.SetFocus();
	}
	else
		m_Genre.EnableWindow( FALSE );
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedEnbyear()
{
	CButton* ptrButton;
    ptrButton = (CButton*)GetDlgItem( IDC_ENBYEAR );
	if ( ptrButton->GetCheck() )
	{
		m_Year.EnableWindow( TRUE );
		m_Year.SetFocus();

		CString text;
		m_Year.GetWindowText( text );
		m_Year.SetSel( 0, text.GetLength() );	
	}
	else
		m_Year.EnableWindow( FALSE );
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::OnBnClickedEnbrating()
{
	CButton* ptrButton;
    ptrButton = (CButton*)GetDlgItem( IDC_ENBRATING );
	if ( ptrButton->GetCheck() )
	{
		m_Rating.EnableWindow( TRUE );
		m_Rating.SetFocus();
	}
	else
		m_Rating.EnableWindow( FALSE );
}

///////////////////////////////////////////////////

