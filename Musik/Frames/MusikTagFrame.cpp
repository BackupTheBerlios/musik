/*
 *  MusikTagFrame.cpp
 *
 *  Tag editing frame
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "MusikTagFrame.h"
#include "../MusikUtils.h"
#include <id3/tag.h>

//--- related frames ---//
#include "../Frames/MusikFrame.h"

//--- globals ---//
#include "../MusikGlobals.h"

//--- threads ---//
#include "../Threads/MusikTagThreads.h"

BEGIN_EVENT_TABLE( MusikTagFrame, wxFrame )
	EVT_CHAR_HOOK		(							MusikTagFrame::OnTranslateKeys			)
	EVT_CHECKBOX		( MUSIK_TAG_CHK_TITLE,		MusikTagFrame::OnClickCheckTitle		)
	EVT_CHECKBOX		( MUSIK_TAG_CHK_TRACKNUM,	MusikTagFrame::OnClickCheckTrackNum		)
	EVT_CHECKBOX		( MUSIK_TAG_CHK_ARTIST,		MusikTagFrame::OnClickCheckArtist		)
	EVT_CHECKBOX		( MUSIK_TAG_CHK_ALBUM,		MusikTagFrame::OnClickCheckAlbum		)
	EVT_CHECKBOX		( MUSIK_TAG_CHK_GENRE,		MusikTagFrame::OnClickCheckGenre		)
	EVT_CHECKBOX		( MUSIK_TAG_CHK_YEAR,		MusikTagFrame::OnClickCheckYear			)
	EVT_BUTTON			( MUSIK_TAG_CANCEL,			MusikTagFrame::OnClickCancel			)
	EVT_BUTTON			( MUSIK_TAG_APPLY,			MusikTagFrame::OnClickApply				)
	EVT_BUTTON			( MUSIK_TAG_OK,				MusikTagFrame::OnClickOK				)
	EVT_BUTTON			( MUSIK_TAG_NEXT,			MusikTagFrame::OnClickNext				)
	EVT_BUTTON			( MUSIK_TAG_PREV,			MusikTagFrame::OnClickPrev				)
	EVT_CLOSE			( MusikTagFrame::OnClose											)

	//---------------------------------------------------------//
	//--- threading events.. we use EVT_MENU becuase its	---//
	//--- nice and simple, and gets the job done. this may	---//
	//--- become a little prettier later, but it works.		---//
	//---------------------------------------------------------//
    EVT_MENU			( MUSIK_TAG_THREAD_START,	MusikTagFrame::OnTagThreadStart			)
    EVT_MENU			( MUSIK_TAG_THREAD_END,		MusikTagFrame::OnTagThreadEnd			)
	EVT_MENU			( MUSIK_TAG_THREAD_PROG,	MusikTagFrame::OnTagThreadProg			)
END_EVENT_TABLE()

MusikTagFrame::MusikTagFrame( wxFrame* pParent, CMusikSongArray aSongs, int nCurFrame, int nEditType, int n )
	: wxFrame ( pParent, -1, wxT(""), wxPoint( 0, 0 ), wxSize( 420, 230 ), wxCAPTION | wxTAB_TRAVERSAL | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR )
{
	//---------------//
 	//--- colours ---//
	//---------------//
	static wxColour cBtnFace = wxSystemSettings::GetColour( wxSYS_COLOUR_3DFACE );
	this->SetBackgroundColour ( cBtnFace );

	//------------//
	//--- icon ---//
	//------------//
	#if defined (__WXMSW__)
		SetIcon( wxICON( musicbox ) );
	#endif

	//----------------------------//
	//--- initialize variables ---//
	//----------------------------//
	m_WriteTag	= false;
	m_Close		= false;
	m_Songs		= aSongs;
	nFrame		= nCurFrame;
	nIndex		= 0;
	nType		= nEditType;
	m_FrameType	= nType;
	SetActiveThread( NULL );

	//-----------------//
	//--- set title ---//
	//-----------------//
	SetCaption();

	//-------------------------------------------------//
	//--- setup right index, if in single edit mode ---//
	//-------------------------------------------------//
	if ( m_FrameType == MUSIK_TAG_SINGLE )
	{
		if ( m_Songs.GetCount() < 2 )
			nIndex = 0;
		else
			nIndex = n;
	}

	//---------------//
	//--- objects ---//
	//---------------//
	wxStaticText *stFilename	=	new wxStaticText	( this, -1, _("File"), wxPoint( 0, 0 ), wxSize( 40, -1 ), wxALIGN_LEFT );
	tcFilename					=	new wxTextCtrl		( this, -1,	wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), wxTE_READONLY );
	tcFilename->Enable( FALSE );

	wxStaticText *stTitle		=	new wxStaticText	( this, -1, _("Title"), wxPoint( 0, 0 ), wxSize( 40, -1 ), wxALIGN_LEFT );
	tcTitle						=	new wxTextCtrl		( this, MUSIK_TAG_TITLE, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	chkTitle					=	new wxCheckBox		( this, MUSIK_TAG_CHK_TITLE, wxT(""), wxPoint( -1, -1 ), wxSize( -1, -1 ) ); 
	
	wxStaticText *stTrackNum	=	new wxStaticText	( this, -1, wxT("Track #  "), wxPoint( 0, 0 ), wxSize( 50, -1 ), wxALIGN_RIGHT );
	tcTrackNum					=	new wxTextCtrl		( this, MUSIK_TAG_TRACKNUM,	wxT(""), wxPoint( 0, 0 ), wxSize( 22, -1 ) );
	chkTrackNum					=	new wxCheckBox		( this, MUSIK_TAG_CHK_TRACKNUM,	wxT(""), wxPoint( -1, -1 ), wxSize( -1, -1 ) );

	wxStaticText *stArtist		=	new wxStaticText	( this, -1, _("Artist"), wxPoint( 0, 0 ), wxSize( 40, -1 ), wxALIGN_LEFT );
	tcArtist					=	new wxTextCtrl		( this, MUSIK_TAG_ARTIST, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	chkArtist					=	new wxCheckBox		( this, MUSIK_TAG_CHK_ARTIST,	wxT(""), wxPoint( -1, -1 ), wxSize( -1, -1 ) );

	wxStaticText *stAlbum		=	new wxStaticText	( this, -1, _("Album"), wxPoint( 0, 0 ), wxSize( 40, -1 ), wxALIGN_LEFT );
	tcAlbum						=	new wxTextCtrl		( this, MUSIK_TAG_ALBUM, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	chkAlbum					=	new wxCheckBox		( this, MUSIK_TAG_CHK_ALBUM, wxT(""), wxPoint( -1, -1 ), wxSize( -1, -1 ) );

	wxStaticText *stGenre		=	new wxStaticText	( this, -1, _("Genre"), wxPoint( 0, 0 ), wxSize( 40, -1 ), wxALIGN_LEFT );
	cmbGenre					=	new wxComboBox		( this, MUSIK_TAG_GENRE, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY | wxCB_SORT );
	chkGenre					=	new wxCheckBox		( this, MUSIK_TAG_CHK_GENRE, wxT(""), wxPoint( -1, -1 ), wxSize( -1, -1 ) );

	wxStaticText *stYear		=	new wxStaticText	( this, -1, _("Year  "),	wxPoint( 0, 0 ), wxSize( 40, -1 ), wxALIGN_RIGHT );
	tcYear						=	new wxTextCtrl		( this, MUSIK_TAG_YEAR,	wxT(""), wxPoint( 0, 0 ), wxSize( 36, -1 ) );
	chkYear						=	new wxCheckBox		( this, MUSIK_TAG_CHK_YEAR,	wxT(""), wxPoint( -1, -1 ), wxSize( -1, -1 ) );

	chkWriteTag					=	new wxCheckBox		( this, MUSIK_TAG_CHK_WRITETAG, _("Write tags to file    "), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkClear					=	new wxCheckBox		( this, MUSIK_TAG_CHK_WRITETAG_CLEAR, _("Clear old tags    "), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkRename					=	new wxCheckBox		( this, MUSIK_TAG_CHK_RENAME, _("Rename files"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );

	gProgress					=	new wxGauge			( this, -1, 100, wxPoint( -1, -1 ), wxSize( -1, 18 ), wxGA_SMOOTH );

	btnNext		= new wxButton( this, MUSIK_TAG_NEXT,	wxT(">"),		wxPoint( 0, 0 ), wxSize( 20, 24 ) );
	btnPrev		= new wxButton( this, MUSIK_TAG_PREV,	wxT("<"),		wxPoint( 0, 0 ), wxSize( 20, 24 ) );
	btnCancel	= new wxButton( this, MUSIK_TAG_CANCEL,	_("Cancel"),	wxPoint( 0, 0 ), wxSize( 60, 24 ) );
	btnApply	= new wxButton( this, MUSIK_TAG_APPLY,	_("Apply"),	wxPoint( 0, 0 ), wxSize( 60, 24 ) );
	btnOK		= new wxButton( this, MUSIK_TAG_OK,		_("OK"),		wxPoint( 0, 0 ), wxSize( 60, 24 ) );

	wxWindow *wndDummy1 = new wxWindow( this, -1, wxPoint( 0, 0 ), wxSize( 0, 0 ) );
	wxWindow *wndDummy2 = new wxWindow( this, -1, wxPoint( 0, 0 ), wxSize( 0, 0 ) );

	//-------------------------//
	//---  top row sizer    ---//
	//---      filename     ---//
	//-------------------------//
	hsRow0 = new wxBoxSizer( wxHORIZONTAL );
	hsRow0->Add( stFilename,		0, wxCENTER			);
	hsRow0->Add( tcFilename,		1, wxEXPAND			);

	//-------------------------//
	//---  first row sizer  ---//
	//--- title, track, num ---//
	//-------------------------//
	hsRow1 = new wxBoxSizer( wxHORIZONTAL );
	hsRow1->Add( stTitle,		0, wxALIGN_CENTER_VERTICAL				);
	hsRow1->Add( tcTitle,		1, wxALIGN_CENTER_VERTICAL				);
	hsRow1->Add( chkTitle,		0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2	);
	hsRow1->Add( stTrackNum,	0, wxALIGN_CENTER_VERTICAL				);
	hsRow1->Add( tcTrackNum,	0, wxALIGN_CENTER_VERTICAL				);
	hsRow1->Add( chkTrackNum,	0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2	);

	//------------------------//
	//--- second row sizer ---//
	//---      artist      ---//
	//------------------------//
	hsRow2 = new wxBoxSizer( wxHORIZONTAL	);
	hsRow2->Add( stArtist,		0, wxCENTER				);
	hsRow2->Add( tcArtist,		1, wxEXPAND				);
	hsRow2->Add( chkArtist,		0, wxCENTER | wxLEFT, 2 );

	//-----------------------//
	//--- third row sizer ---//
	//---      album      ---//
	//-----------------------//
	hsRow3 = new wxBoxSizer( wxHORIZONTAL	);
	hsRow3->Add( stAlbum,		0, wxCENTER				);
	hsRow3->Add( tcAlbum,		1, wxEXPAND				);
	hsRow3->Add( chkAlbum,		0, wxCENTER | wxLEFT, 2 );

	//------------------------//
	//--- fourth row sizer ---//
	//---    genre, year   ---//
	//------------------------//
	hsRow4 = new wxBoxSizer( wxHORIZONTAL	);
	hsRow4->Add( stGenre,		0, wxCENTER				);
	hsRow4->Add( cmbGenre,		1, wxEXPAND				);
	hsRow4->Add( chkGenre,		0, wxCENTER | wxLEFT, 2 );
	hsRow4->Add( stYear,		0, wxCENTER				);
	hsRow4->Add( tcYear,		0, wxCENTER				);
	hsRow4->Add( chkYear,		0, wxCENTER | wxLEFT, 2 );

	//-----------------------//
	//--- fifth row sizer ---//
	//---  write to fle   ---//
	//-----------------------//
	hsRow5 = new wxBoxSizer( wxHORIZONTAL	);
	hsRow5->Add( chkWriteTag,	0, wxTOP | wxLEFT, 4	);
	hsRow5->Add( chkClear,		0, wxTOP, 4	);
	hsRow5->Add( chkRename,		0, wxTOP, 4	);

	//-----------------------//
	//--- sixth row sizer ---//
	//---    progress     ---//
	//-----------------------//
	hsRow6 = new wxBoxSizer( wxHORIZONTAL	);
	hsRow6->Add( gProgress,	1, wxLEFT | wxRIGHT, 2 );

	//--------------------//
	//--- row 1 thru 6 ---//
	//--------------------//
	vsRows = new wxBoxSizer( wxVERTICAL );
	vsRows->Add( hsRow0,	0, wxEXPAND | wxALL, 4 );
	vsRows->Add( hsRow1,	0, wxEXPAND | wxALL, 4 );
	vsRows->Add( hsRow2,	0, wxEXPAND | wxALL, 4 );
	vsRows->Add( hsRow3,	0, wxEXPAND | wxALL, 4 );
	vsRows->Add( hsRow4,	0, wxEXPAND | wxALL, 4 );
	vsRows->Add( hsRow5,	0, wxADJUST_MINSIZE  | wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 16 );
	vsRows->Add( hsRow6,	0, wxEXPAND | wxBOTTOM, 2 );
	vsRows->Show( hsRow6, FALSE );

	//---------------------------//
	//--- system button sizer ---//
	//---     genre, year     ---//
	//---------------------------//
	hsNav = new wxBoxSizer( wxHORIZONTAL );
	hsNav->Add( btnCancel,	0, wxALIGN_LEFT | wxLEFT,			2	);
	hsNav->Add( wndDummy1,	1, wxEXPAND								);
	hsNav->Add( btnPrev,	0, wxALIGN_LEFT							);
	hsNav->Add( btnNext,	0, wxALIGN_LEFT							);
	hsNav->Add( wndDummy2,	1, wxEXPAND								);
	hsNav->Add( btnApply,	0, wxALIGN_RIGHT						);
	hsNav->Add( btnOK,		0, wxALIGN_RIGHT | wxLEFT | wxRIGHT, 2	);

	//-----------------//
	//--- top sizer ---//
	//-----------------//
	vsTopSizer = new wxBoxSizer( wxVERTICAL );
	vsTopSizer->Add( vsRows,	1, wxEXPAND );
	vsTopSizer->Add( hsNav,		0, wxEXPAND | wxBOTTOM, 2 );

	//-------------------//
	//--- layout, etc ---//
	//-------------------//
	SetSizer( vsTopSizer );
	Layout();
	Centre();

	//-------------------//
	//--- other stuff ---//
	//-------------------//	
	for ( int i = 0; i < ID3_NR_OF_V1_GENRES; i++ )
		cmbGenre->Append( ConvA2W( ID3_v1_genre_description[i] ) );

	LoadFirst();
}

void MusikTagFrame::SetCaption()
{
	if ( m_FrameType == MUSIK_TAG_SINGLE )
		SetTitle( _("Tag information editing (single)") );
	else if ( m_FrameType == MUSIK_TAG_MULTIPLE )
		SetTitle( _("Tag information editing (batch)") );	
}

void MusikTagFrame::LoadFirst()
{
	if( ( nType == MUSIK_TAG_MULTIPLE ) || ( m_Songs.GetCount() < 2 ) )
	{
		hsNav->Show( btnPrev, false );
		hsNav->Show( btnNext, false );
	}	

	chkWriteTag->SetValue	( g_Prefs.nTagDlgWrite	);
	chkClear->SetValue		( g_Prefs.nTagDlgClear	);
	chkRename->SetValue		( g_Prefs.nTagDlgRename	);

	SetChecks( nType );
	PopulateTagDlg();
}

void MusikTagFrame::SetChecks( const int i )
{
	if( i == MUSIK_TAG_SINGLE )
	{
		chkTitle->SetValue		( true );
		chkTrackNum->SetValue	( true );
		chkArtist->SetValue		( true );
		chkAlbum->SetValue		( true );
		chkGenre->SetValue		( true );
		chkYear->SetValue		( true );
	}
	else if( i == MUSIK_TAG_MULTIPLE )
	{
		switch( nFrame )
		{
			case 0:
				chkTitle->SetValue		( true );
				break;
			case 1:
				chkTrackNum->SetValue	( true );
				break;
			case 2:
				chkArtist->SetValue		( true );
				break;
			case 3:
				chkAlbum->SetValue		( true );
				break;
			case 4:
				chkGenre->SetValue		( true );
				break;
			case 5:
				chkYear->SetValue		( true );
				break;
			default:
				chkTitle->SetValue		( true );
				break;
		}
	}
}

void MusikTagFrame::PopulateTagDlg()
{
	//--- filename ---//
	tcFilename->SetValue( m_Songs.Item( nIndex ).Filename );

	//--- title ---//
	tcTitle->SetValue( m_Songs.Item( nIndex ).Title	);	

	//--- track number ---//
	wxString sTrackNum;
	int nTrackNum = m_Songs.Item( nIndex ).TrackNum;
	if( nTrackNum < 1 )
		sTrackNum = wxT("0");
	else
		sTrackNum.sprintf( wxT("%d"), nTrackNum );
	tcTrackNum->SetValue	( sTrackNum		);

	//--- artist ---//
	tcArtist->SetValue( m_Songs.Item( nIndex ).Artist );

	//--- album ---//
	tcAlbum->SetValue( m_Songs.Item( nIndex ).Album	);

	//--- genre ---//
	wxString sGenre = m_Songs.Item( nIndex ).Genre;
	int nGenre = GetGenreID( sGenre );
	if ( nGenre != 12 )
		cmbGenre->SetValue( sGenre );
	else
		cmbGenre->SetValue( _("Other") );

    //--- year ---//
	if( m_Songs.Item( nIndex ).Year == wxT("") || m_Songs.Item( nIndex ).Year == _("<unknown>") )
		tcYear->SetValue( wxT("") );
	else
		tcYear->SetValue( m_Songs.Item( nIndex ).Year );

	//--- if we are at beginning, disable back, enable forward ---//
	if( nIndex == 0 )
	{	
		btnPrev->Enable( false );
		btnNext->Enable( true  );
	}

	//--- if we are floating in the middle, enable back and forward ---//
	else if( nIndex > 0 && nIndex < ( (int)m_Songs.GetCount() - 1 ) )
	{
		btnPrev->Enable( true );
		btnNext->Enable( true );
	}

	//--- if we are near the end ---//
	else if ( nIndex + 1 == (int)m_Songs.GetCount() )
	{
		btnPrev->Enable( true  );
		btnNext->Enable( false );
	}

	//--- enable text controls accordingly ---//
	SetEnabled();
	SetFocus();
}

void MusikTagFrame::SetEnabled()
{
	if( chkTitle->IsChecked() )
		tcTitle->Enable( true );
	else
		tcTitle->Enable( false );
	if( chkTrackNum->IsChecked() )
		tcTrackNum->Enable( true );
	else
		tcTrackNum->Enable( false );
	if( chkArtist->IsChecked() )
		tcArtist->Enable( true );
	else
		tcArtist->Enable( false );
	if( chkAlbum->IsChecked() )
		tcAlbum->Enable( true );
	else
		tcAlbum->Enable( false );
	if( chkGenre->IsChecked() )
		cmbGenre->Enable( true );
	else
		cmbGenre->Enable( false );
	if( chkYear->IsChecked() )
		tcYear->Enable( true );
	else
		tcYear->Enable( false );
}

void MusikTagFrame::SetFocus()
{
	//--- select text ctrl based on hotkey / menu selection ---//
	switch( nFrame )
	{
		case 0:
			tcTitle->SetFocus();
			tcTitle->SetSelection( -1, -1 );
			break;
		case 1:
			tcTrackNum->SetFocus();
			tcTrackNum->SetSelection( -1, -1 );
			break;
		case 2:
			tcArtist->SetFocus();
			tcArtist->SetSelection( -1, -1 );
			break;
		case 3:
			tcAlbum->SetFocus();
			tcAlbum->SetSelection( -1, -1 );
			break;
		case 4:
			cmbGenre->SetFocus();
			break;
		case 5:
			tcYear->SetFocus();
			tcYear->SetSelection( -1, -1 );
			break;
		default:
			tcTitle->SetFocus();
			tcTitle->SetSelection( -1, -1 );
			break;
	}
}

void MusikTagFrame::Next()
{
	SaveCurSong();
	++nIndex;
	PopulateTagDlg();
}

void MusikTagFrame::Prev()
{
	SaveCurSong();
	--nIndex;
	PopulateTagDlg();
}

void MusikTagFrame::SaveCurSong()
{
	//--- update title ---//
	if ( tcTitle->IsEnabled() && ( tcTitle->GetValue() != m_Songs.Item( nIndex ).Title ) )
		m_Songs.Item( nIndex ).Title = tcTitle->GetValue();

	//--- update track number ---//
	long nTrackNum;
	tcTrackNum->GetValue().ToLong( &nTrackNum );
	if ( tcTrackNum->IsEnabled() && nTrackNum != m_Songs.Item( nIndex ).TrackNum )
		m_Songs.Item( nIndex ).TrackNum = nTrackNum;
	
	//--- artist ---//
	if ( tcArtist->IsEnabled() && tcArtist->GetValue() != m_Songs.Item( nIndex ).Artist )
		m_Songs.Item( nIndex ).Artist = tcArtist->GetValue();

	//--- album ---//
	if ( tcAlbum->IsEnabled() && tcAlbum->GetValue() != m_Songs.Item( nIndex ).Album )
		m_Songs.Item( nIndex ).Album = tcAlbum->GetValue();

	//--- genre ---//
	if ( cmbGenre->IsEnabled() && cmbGenre->GetValue() != m_Songs.Item( nIndex ).Genre )
		m_Songs.Item( nIndex ).Genre = cmbGenre->GetValue();

	//--- year ---//
	if ( tcYear->IsEnabled() && tcYear->GetValue() != m_Songs.Item( nIndex ).Year )
		m_Songs.Item( nIndex ).Year = tcYear->GetValue();
}

void MusikTagFrame::CheckChangesSingle()
{
	for ( size_t i = 0; i < m_Songs.GetCount(); i++ )
	{
		CMusikSong orig = g_Library.GetSongFromFilename( m_Songs.Item( i ).Filename );

		//--- check title ---//
		if( tcTitle->IsEnabled() && ( orig.Title != m_Songs.Item( i ).Title ) )
			m_Songs.Item( i ).Check1 = 1;

		//--- check title ---//
		if( tcTrackNum->IsEnabled() && orig.TrackNum != m_Songs.Item( i ).TrackNum )
			m_Songs.Item( i ).Check1 = 1;

		//--- check artist ---//
		if( tcArtist->IsEnabled() && orig.Artist != m_Songs.Item( i ).Artist )
			m_Songs.Item( i ).Check1 = 1;

		//--- check album ---//
		if( tcAlbum->IsEnabled() && orig.Album != m_Songs.Item( i ).Album )
			m_Songs.Item( i ).Check1 = 1;

		//--- check genre ---//
		if( cmbGenre->IsEnabled() && orig.Genre != m_Songs.Item( i ).Genre )
			m_Songs.Item( i ).Check1 = 1;

		//--- check year ---//
		if( tcYear->IsEnabled() && orig.Year != m_Songs.Item( i ).Year )
			m_Songs.Item( i ).Check1 = 1;
	}
}

void MusikTagFrame::CheckChangesBatch()
{
	for ( size_t i = 0; i < m_Songs.GetCount(); i++ )
	{
		//--- title ---//
		if ( tcTitle->IsEnabled() && m_Songs.Item( i ).Title != tcTitle->GetValue() )
		{
			m_Songs.Item( i ).Title = tcTitle->GetValue();
			m_Songs.Item( i ).Check1 = 1;
		}

		//--- track number ---//
		int nTrackNum = StringToInt( tcTitle->GetValue() );
		if ( tcTrackNum->IsEnabled() && m_Songs.Item( i ).TrackNum != nTrackNum )
		{
			m_Songs.Item( i ).TrackNum = nTrackNum;
			m_Songs.Item( i ).Check1 = 1;
		}

		//--- artist ---//
		if ( tcArtist->IsEnabled() && m_Songs.Item( i ).Artist != tcArtist->GetValue() )
		{
			m_Songs.Item( i ).Artist = tcArtist->GetValue();
			m_Songs.Item( i ).Check1 = 1;
		}

		//--- album ---//
		if ( tcAlbum->IsEnabled() && m_Songs.Item( i ).Album != tcAlbum->GetValue() )
		{
			m_Songs.Item( i ).Album = tcAlbum->GetValue();
			m_Songs.Item( i ).Check1 = 1;
		}

		//--- genre ---//
		if ( cmbGenre->IsEnabled() && m_Songs.Item( i ).Genre != cmbGenre->GetValue() )
		{
			m_Songs.Item( i ).Genre = cmbGenre->GetValue();
			m_Songs.Item( i ).Check1 = 1;
		}

		//--- year ---//
		if ( tcYear->IsEnabled() && m_Songs.Item( i ).Year != tcYear->GetValue() )
		{
			m_Songs.Item( i ).Year = tcYear->GetValue();
			m_Songs.Item( i ).Check1 = 1;
		}
	}
}

void MusikTagFrame::Apply( bool close )
{
	//--- save settings ---//
	if ( chkWriteTag->IsChecked() )
		g_Prefs.nTagDlgWrite = 1;
	else
		g_Prefs.nTagDlgWrite = 0;
	if ( chkClear->IsChecked() )
		g_Prefs.nTagDlgClear = 1;
	else
		g_Prefs.nTagDlgClear = 0;
	if ( chkRename->IsChecked() )
		g_Prefs.nTagDlgRename = 1;
	else
		g_Prefs.nTagDlgRename = 0;

	//--- do we close upon thread completion? ---//
	m_Close = close;

	//--- start the approperiate thread running ---//
	if ( GetActiveThread() == NULL )
	{
		SetTitle( _( "Scanning for changed attributes" ) );

	        if ( nType == MUSIK_TAG_SINGLE )
	        {
			SaveCurSong();	
			CheckChangesSingle();
	        }
	        else if ( nType == MUSIK_TAG_MULTIPLE )
			CheckChangesBatch();

		pApplyThread = new MusikTagApplyThread();
		pApplyThread->Create();
		pApplyThread->Run();
        }
	else
		wxMessageBox( _("An internal error has occured.\nPrevious thread not terminated correctly.\n\nPlease contact the Musik development team with this error."), MUSIK_VERSION, wxICON_STOP );
}

void MusikTagFrame::OnClickOK( wxCommandEvent& WXUNUSED(event) )
{
	Apply( true );
}
void MusikTagFrame::OnClickCancel( wxCommandEvent& WXUNUSED(event) )
{
	//--- clean up ---//
	g_MusikFrame->Enable( TRUE );
	Destroy();
}

void MusikTagFrame::OnClose( wxCommandEvent& WXUNUSED(event) )
{
	//--- clean up ---//
	g_MusikFrame->Enable( TRUE );
	Destroy();
}

void MusikTagFrame::EnableProgress( bool enable )
{
	if ( enable )
	{
		SetSize( wxSize( 420, 224 ) );
		vsRows->Show( hsRow6, TRUE );
		vsRows->Show( hsRow5, FALSE );
		Layout();
	}
	else if ( !enable )
	{
		SetSize( wxSize( 420, 230 ) );
		vsRows->Show( hsRow6, FALSE );
		vsRows->Show( hsRow5, TRUE );
		Layout();
	}
	Enable( !enable );
}

void MusikTagFrame::OnTranslateKeys( wxKeyEvent& event )
{
	//--- escape is pressed ---//
 	if ( event.GetKeyCode() == WXK_ESCAPE )
	{
		if ( GetActiveThread() != NULL )
			GetActiveThread()->Delete();

		else
		{
			g_MusikFrame->Enable( TRUE );
			Destroy();
		}
	}

	//--- enter, go next ---//
	if ( event.GetKeyCode() == WXK_RETURN && !event.ShiftDown() )
	{
		if ( GetActiveThread() == NULL )
		{
			if ( m_FrameType == MUSIK_TAG_SINGLE )
		{
			if ( nIndex + 1 < (int)m_Songs.GetCount() )
				Next();
		}
			else if ( m_FrameType ==  MUSIK_TAG_MULTIPLE )
			{
				Apply( true );
			}
		}
	}

	//--- shift-enter, go back ---//
	if ( event.GetKeyCode() == WXK_RETURN && event.ShiftDown() && m_FrameType == MUSIK_TAG_SINGLE )
	{
		if ( GetActiveThread() == NULL )
		{
			if ( nIndex > 0 )
				Prev();
		}
	}

	event.Skip();	
}

//--------------------------------------------------------------//
//--- a start thread event was caught, figure out what to do ---//
//--------------------------------------------------------------//
void MusikTagFrame::OnTagThreadStart( wxCommandEvent& WXUNUSED(event) )
{
	EnableProgress( true );

	SetProgress( 0 );
	SetActiveThread( pApplyThread );

	if ( GetProgressType() == MUSIK_TAG_SINGLE_THREAD )
		SetProgressType( MUSIK_TAG_SINGLE_THREAD );
	else if ( GetProgressType() == MUSIK_TAG_BATCH_THREAD )
		SetProgressType( MUSIK_TAG_BATCH_THREAD );

    SetTitle( _( "Updating tags - (ESC to abort)" ) );
}

//-------------------------------------------------------------//
//--- an end thread event was caught, figure out what to do ---//
//-------------------------------------------------------------//
void MusikTagFrame::OnTagThreadEnd( wxCommandEvent& WXUNUSED(event) )
{
	//------------------------------------------//
	//--- if the frame is a single edit mode ---//
	//------------------------------------------//
	if ( m_FrameType == MUSIK_TAG_SINGLE )
	{
		g_Playlist = m_Songs;
		g_PlaylistCtrl->Update();
		g_ActivityAreaCtrl->ResetAllContents();
	}

	//-----------------------------------------//
	//--- if the frame is a batch edit mode ---//
	//-----------------------------------------//
	else if ( m_FrameType == MUSIK_TAG_MULTIPLE )
	{
	//--- give the playlist back ---//
	wxArrayInt sel = g_PlaylistCtrl->GetSelItems();
	for ( size_t i = 0; i < sel.GetCount(); i++ )
		g_Playlist.Item( sel.Item( i ) ) = 	m_Songs.Item( i );
	
		g_PlaylistCtrl->Update();
		g_ActivityAreaCtrl->ResetAllContents();
	}

	//------------------------------------------//
	//--- make sure these are back to normal ---//
	//------------------------------------------//
	SetProgress	( 0 );
	SetProgressType	( 0 );
	SetActiveThread	( NULL );
	SetCaption();

	//----------------------------------//
	//--- start dialog over or close ---//
	//----------------------------------//
	if ( !m_Close )
		EnableProgress( false );

	else
	{
		g_MusikFrame->Enable( TRUE );
		Destroy();
	}
}

//------------------------------------------------------------------//
//--- an progress thread event was caught, figure out what to do ---//
//------------------------------------------------------------------//
void MusikTagFrame::OnTagThreadProg( wxCommandEvent& WXUNUSED(event) )
{
	gProgress->SetValue( GetProgress() );
}
