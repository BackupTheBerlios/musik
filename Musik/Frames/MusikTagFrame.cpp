/*
 *  MusikTagFrame.cpp
 *
 *  Tag editing frame
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "MusikTagFrame.h"

//--- id3 tagging ( to populate genres box ) ---//
#include <id3/tag.h>

//--- related frames ---//
#include "../Frames/MusikFrame.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

//--- threads ---//
#include "../Threads/MusikTagThreads.h"

BEGIN_EVENT_TABLE( MusikTagFrame, wxFrame )
	EVT_CHAR_HOOK		(							MusikTagFrame::OnTranslateKeys			)
	EVT_COMMAND_RANGE		( MUSIK_TAG_CHK_TAGFIRST,MUSIK_TAG_CHK_TAGLAST,wxEVT_COMMAND_CHECKBOX_CLICKED ,	MusikTagFrame::OnClickCheckTags		)

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

MusikTagFrame::MusikTagFrame( wxFrame* pParent, CPlaylistCtrl * pPlaylistctrl, int nCurFrame)
	: wxFrame ( pParent, -1, wxT(""), wxPoint( 0, 0 ), wxSize( 420, 230 ), wxCAPTION | wxTAB_TRAVERSAL | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR )
	, m_Songs(*pPlaylistctrl->GetPlaylist())
{
	pPlaylistctrl->GetSelItems(m_arrSongsSelected);

	m_bDirty = true;
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
	nFrame		= nCurFrame;
	
	
	//--- if there is only 1 song selected, setup for single edit mode ---//
	if ( m_arrSongsSelected.GetCount() <= 1 )
	{
		m_EditType = MUSIK_TAG_SINGLE;
	}
	//--- more than 1 song, setup for batch edit mode ---//
	else 
	{
		m_EditType = MUSIK_TAG_MULTIPLE;
	}

	//-----------------//
	//--- set title ---//
	//-----------------//
	SetCaption();

    nIndex = m_arrSongsSelected[0];


	//---------------//
	//--- objects ---//
	//---------------//
	wxStaticText *stFilename	=	new wxStaticText	( this, -1, _("File"), wxPoint( 0, 0 ), wxSize( 40, -1 ), wxALIGN_LEFT );
	tcFilename					=	new wxTextCtrl		( this, -1,	wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), wxTE_READONLY );

	wxStaticText *stTitle		=	new wxStaticText	( this, -1, _("Title"), wxPoint( 0, 0 ), wxSize( 40, -1 ), wxALIGN_LEFT );
	tcTitle						=	new wxTextCtrl		( this, MUSIK_TAG_TITLE, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	chkTitle					=	new wxCheckBox		( this, MUSIK_TAG_CHK_TITLE, wxT(""), wxPoint( -1, -1 ), wxSize( -1, -1 ) ); 
	
	wxStaticText *stTrackNum	=	new wxStaticText	( this, -1, _("Track #  "), wxPoint( 0, 0 ), wxSize( 50, -1 ), wxALIGN_RIGHT );
	tcTrackNum					=	new wxTextCtrl		( this, MUSIK_TAG_TRACKNUM,	wxT(""), wxPoint( 0, 0 ), wxSize( 22, -1 ) );
	chkTrackNum					=	new wxCheckBox		( this, MUSIK_TAG_CHK_TRACKNUM,	wxT(""), wxPoint( -1, -1 ), wxSize( -1, -1 ) );

	wxStaticText *stArtist		=	new wxStaticText	( this, -1, _("Artist"), wxPoint( 0, 0 ), wxSize( 40, -1 ), wxALIGN_LEFT );
	tcArtist					=	new wxTextCtrl		( this, MUSIK_TAG_ARTIST, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	chkArtist					=	new wxCheckBox		( this, MUSIK_TAG_CHK_ARTIST,	wxT(""), wxPoint( -1, -1 ), wxSize( -1, -1 ) );

	wxStaticText *stAlbum		=	new wxStaticText	( this, -1, _("Album"), wxPoint( 0, 0 ), wxSize( 40, -1 ), wxALIGN_LEFT );
	tcAlbum						=	new wxTextCtrl		( this, MUSIK_TAG_ALBUM, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	chkAlbum					=	new wxCheckBox		( this, MUSIK_TAG_CHK_ALBUM, wxT(""), wxPoint( -1, -1 ), wxSize( -1, -1 ) );

	wxStaticText *stGenre		=	new wxStaticText	( this, -1, _("Genre"), wxPoint( 0, 0 ), wxSize( 40, -1 ), wxALIGN_LEFT );
	cmbGenre					=	new wxComboBox		( this, MUSIK_TAG_GENRE, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_DROPDOWN );
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
	// get genre from db
	wxArrayString arrGenre;
	wxGetApp().Library.GetAllGenres(arrGenre);

	// add standard id3v1 genres to array
	for ( int i = 0; i < ID3_NR_OF_V1_GENRES; i++ )
		arrGenre.Add(ConvA2W( ID3_v1_genre_description[i] )	);
	arrGenre.Sort();
	for ( size_t i = 0; i < arrGenre.GetCount(); i++ )	
	{
		if( i > 0 && arrGenre[i] == arrGenre[i-1])
			continue; // skip double entrys
		cmbGenre->Append( arrGenre[i]);
	}
}

void MusikTagFrame::SetCaption()
{
	if ( m_EditType == MUSIK_TAG_SINGLE )
		SetTitle( _("Tag information editing (single)") );
	else if ( m_EditType == MUSIK_TAG_MULTIPLE )
		SetTitle( _("Tag information editing (batch)") );	
}

bool MusikTagFrame::Show( bool show )
{
	bool bRet = wxWindow::Show( show );
	
	if ( show )
	{
		if( ( m_EditType == MUSIK_TAG_MULTIPLE ) || ( m_Songs.GetCount() < 2 ) )
		{
			hsNav->Show( btnPrev, false );
			hsNav->Show( btnNext, false );
		}	

		chkWriteTag->SetValue	( wxGetApp().Prefs.bTagDlgWrite	);
		chkClear->SetValue		( wxGetApp().Prefs.bTagDlgClear	);
		chkRename->SetValue		( wxGetApp().Prefs.bTagDlgRename	);

		SetChecks( m_EditType );
		PopulateTagDlg();
	}
	
	return bRet;	
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
	tcFilename->SetValue( m_Songs.Item( nIndex ).MetaData.Filename.GetFullPath() );

	//--- title ---//
	tcTitle->SetValue(ConvFromUTF8( m_Songs.Item( nIndex ).MetaData.Title	));	

	//--- track number ---//
	wxString sTrackNum;
	int nTrackNum = m_Songs.Item( nIndex ).MetaData.nTracknum;
	if( nTrackNum < 1 )
		sTrackNum = wxT("0");
	else
		sTrackNum.sprintf( wxT("%d"), nTrackNum );
	tcTrackNum->SetValue	( sTrackNum		);

	//--- artist ---//
	tcArtist->SetValue(ConvFromUTF8( m_Songs.Item( nIndex ).MetaData.Artist ));

	//--- album ---//
	tcAlbum->SetValue(ConvFromUTF8( m_Songs.Item( nIndex ).MetaData.Album	));
	//--- genre ---//
	cmbGenre->SetValue( ConvFromUTF8( m_Songs.Item( nIndex ).MetaData.Genre ) );
    //--- year ---//
	tcYear->SetValue(ConvFromUTF8( m_Songs.Item( nIndex ).MetaData.Year ));

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
	m_bDirty = true;
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
	SaveSong(nIndex);
}
void MusikTagFrame::SaveSong(int n)
{
	//--- update title ---//
	if ( tcTitle->IsEnabled() && ( tcTitle->GetValue() != ConvFromUTF8( m_Songs.Item( n ).MetaData.Title )) )
	{
		m_Songs.Item( n ).MetaData.Title = ConvToUTF8( tcTitle->GetValue() );
		m_Songs.Item( n ).Check1 = 1;
	}
	//--- update track number ---//
	long nTrackNum;
	tcTrackNum->GetValue().ToLong( &nTrackNum );
	if ( tcTrackNum->IsEnabled() && nTrackNum != m_Songs.Item( n ).MetaData.nTracknum )
	{
		m_Songs.Item( n ).MetaData.nTracknum = nTrackNum;
		m_Songs.Item( n ).Check1 = 1;
	}
	//--- artist ---//
	if ( tcArtist->IsEnabled() && tcArtist->GetValue() != ConvFromUTF8( m_Songs.Item( n ).MetaData.Artist ))
	{
		m_Songs.Item( n ).MetaData.Artist = ConvToUTF8( tcArtist->GetValue() );
		m_Songs.Item( n ).Check1 = 1;
	}

	//--- album ---//
	if ( tcAlbum->IsEnabled() && tcAlbum->GetValue() != ConvFromUTF8( m_Songs.Item( n ).MetaData.Album ))
	{
		m_Songs.Item( n ).MetaData.Album = ConvToUTF8(tcAlbum->GetValue());
		m_Songs.Item( n ).Check1 = 1;
	}
	//--- genre ---//
	if ( cmbGenre->IsEnabled() && cmbGenre->GetValue() != ConvFromUTF8( m_Songs.Item( n ).MetaData.Genre ))
	{
		m_Songs.Item( n ).MetaData.Genre = ConvToUTF8(cmbGenre->GetValue());
		m_Songs.Item( n ).Check1 = 1;
	}

	//--- year ---//
	if ( tcYear->IsEnabled() && tcYear->GetValue() != ConvFromUTF8(m_Songs.Item( n ).MetaData.Year) )
	{
		m_Songs.Item( n ).MetaData.Year = ConvW2A(tcYear->GetValue());
		m_Songs.Item( n ).Check1 = 1;
	}
	m_bDirty = m_bDirty || m_Songs.Item( n ).Check1 == 1;  // if m_bDirty is once set TRUE, it stays TRUE
}



void MusikTagFrame::CheckChangesBatch()
{
	for ( size_t j = 0; j < m_arrSongsSelected.GetCount(); j++ )
	{
		SaveSong(m_arrSongsSelected[j]);
	}
}

void MusikTagFrame::Apply( bool close )
{
	//--- save settings ---//
	wxGetApp().Prefs.bTagDlgWrite = chkWriteTag->IsChecked() ? 1:0;
	wxGetApp().Prefs.bTagDlgClear = chkClear->IsChecked() ? 1:0;
	wxGetApp().Prefs.bTagDlgRename = chkRename->IsChecked() ? 1:0;

	//--- do we close upon thread completion? ---//
	m_Close = close;

	//--- start the approperiate thread running ---//
	if ( m_ActiveThreadController.IsAlive() == false )
	{
		SetTitle( _( "Scanning for changed attributes" ) );

		if ( m_EditType == MUSIK_TAG_SINGLE )
		{
			SaveCurSong();	
		}
		else if ( m_EditType == MUSIK_TAG_MULTIPLE )
			CheckChangesBatch();
		if(m_bDirty)
		{
			EnableProgress( true );
			m_ActiveThreadController.AttachAndRun( new MusikTagApplyThread(this,m_Songs) );
		}
		else
		{
			if (m_Close)
				Close();
		}
	}
	else
		InternalErrorMessageBox(wxT("Previous thread not terminated correctly."));
}
void MusikTagFrame::Close()
{
	g_MusikFrame->Enable( TRUE );
	Destroy();

}
void MusikTagFrame::OnClickOK( wxCommandEvent& WXUNUSED(event) )
{
	Apply( true );
}
void MusikTagFrame::OnClickCancel( wxCommandEvent& WXUNUSED(event) )
{
	//--- clean up ---//
	Close();
}

void MusikTagFrame::OnClose( wxCloseEvent& WXUNUSED(event) )
{
	//--- clean up ---//
	Close();
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
		if ( m_ActiveThreadController.IsAlive() )
			m_ActiveThreadController.Cancel();
		else
		{
			g_MusikFrame->Enable( TRUE );
			Destroy();
		}
	}

	//--- enter, go next ---//
	else if ( event.GetKeyCode() == WXK_RETURN && !event.ShiftDown() )
	{
		if ( m_ActiveThreadController.IsAlive() == false )
		{
			if ( m_EditType == MUSIK_TAG_SINGLE )
			{
				if ( nIndex + 1 < (int)m_Songs.GetCount() )
					Next();
			}
			else if ( m_EditType ==  MUSIK_TAG_MULTIPLE )
			{
				Apply( true );
			}
		}
	}

	//--- shift-enter, go back ---//
	else if ( event.GetKeyCode() == WXK_RETURN && event.ShiftDown() && m_EditType == MUSIK_TAG_SINGLE )
	{
		if ( m_ActiveThreadController.IsAlive() == false )
		{
			if ( nIndex > 0 )
				Prev();
		}
		return;
	}
	else
		event.Skip();	
}

//--------------------------------------------------------------//
//--- a start thread event was caught, figure out what to do ---//
//--------------------------------------------------------------//
void MusikTagFrame::OnTagThreadStart( wxCommandEvent& WXUNUSED(event) )
{

    SetTitle( _( "Updating tags - (ESC to abort)" ) );
}

//-------------------------------------------------------------//
//--- an end thread event was caught, figure out what to do ---//
//-------------------------------------------------------------//
void MusikTagFrame::OnTagThreadEnd( wxCommandEvent& WXUNUSED(event) )
{
	m_bDirty = false;
	m_ActiveThreadController.Join();
	g_PlaylistBox->Update();
	g_ActivityAreaCtrl->ResetAllContents();

	//------------------------------------------//
	//--- make sure these are back to normal ---//
	//------------------------------------------//
	SetCaption();

	//----------------------------------//
	//--- start dialog over or close ---//
	//----------------------------------//
	if ( m_Close )
	{
		Close();
	}
	else
	{
		EnableProgress( false );
	}
}

//------------------------------------------------------------------//
//--- an progress thread event was caught, figure out what to do ---//
//------------------------------------------------------------------//
void MusikTagFrame::OnTagThreadProg( wxCommandEvent& event )
{
	gProgress->SetValue( event.GetExtraLong() );
}
