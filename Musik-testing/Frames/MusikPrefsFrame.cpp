/*
 *  MusikPrefsFrame.cpp
 *
 *  Preferences frame
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "MusikPrefsFrame.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

//--- classes ---//
#include "../Classes/MusikPlayer.h"

//--- related frames ---//
#include "../Frames/MusikFrame.h"

#include <wx/colordlg.h>

BEGIN_EVENT_TABLE(MusikPrefsFrame, wxFrame)
	EVT_CHAR_HOOK			(											MusikPrefsFrame::OnTranslateKeys	)
	EVT_TREE_SEL_CHANGED	(MUSIK_PREFERENCES_TREE,					MusikPrefsFrame::OnTreeChange		)
	EVT_BUTTON				(MUSIK_PREFERENCES_OK,						MusikPrefsFrame::OnClickOK			)
	EVT_BUTTON				(MUSIK_PREFERENCES_APPLY,					MusikPrefsFrame::OnClickApply		)
	EVT_BUTTON				(MUSIK_PREFERENCES_CANCEL,					MusikPrefsFrame::OnClickCancel		)
	EVT_BUTTON				(MUSIK_PREFERENCES_PLAYLIST_STRIPE_COLOUR,	MusikPrefsFrame::OnClickColour		)
	EVT_BUTTON				(MUSIK_PREFERENCES_ACTIVITY_STRIPE_COLOUR,	MusikPrefsFrame::OnClickColour		)
	EVT_BUTTON				(MUSIK_PREFERENCES_SOURCES_STRIPE_COLOUR,	MusikPrefsFrame::OnClickColour		)
	EVT_COMBOBOX			(MUSIK_PREFERENCES_OUTPUT_DRV,				MusikPrefsFrame::OnOutputChanged	)
	EVT_CLOSE				(											MusikPrefsFrame::OnClose			)
END_EVENT_TABLE()

MusikPrefsFrame::MusikPrefsFrame( wxFrame *pParent, const wxString &sTitle, const wxPoint &pos, const wxSize &size ) 
	: wxFrame( pParent, -1, sTitle, pos, size, wxCAPTION | wxTAB_TRAVERSAL | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR )
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

	//--------------------//
	//--- Dummy Window ---//
	//--------------------//
	wxWindow *wndDummy = new wxWindow( this, -1, wxPoint( 0, 0 ), wxSize( 0, 0 ) );

	//------------------------------//
	//--- Selection Tree Control ---//
	//------------------------------//
	tcPreferencesTree	= new wxTreeCtrl( this, MUSIK_PREFERENCES_TREE, wxPoint( 0, 0 ), wxSize( 100, 300 ), wxTR_HAS_BUTTONS | wxSIMPLE_BORDER );
	//--- root nodes ---//
	nRootID			=	tcPreferencesTree->AddRoot		( _("Preferences")			);
	nOptionsRootID	=	tcPreferencesTree->AppendItem	( nRootID, _("Options")	);
	nTagRootID		=	tcPreferencesTree->AppendItem	( nRootID, _("Tagging")	);
	nSoundRootID	=	tcPreferencesTree->AppendItem	( nRootID, _("Sound")		);
	//--- child nodes ---//
	nInterfaceID	=	tcPreferencesTree->AppendItem	( nOptionsRootID,	_("General")		);
	nSelectionsID	=	tcPreferencesTree->AppendItem	( nOptionsRootID,	_("Selections")	);
	nGeneralTagID	=	tcPreferencesTree->AppendItem	( nTagRootID,		_("General")		);
	nAutoTagID		=	tcPreferencesTree->AppendItem	( nTagRootID,		_("Auto Tag")		);
	nPlaybackID		=	tcPreferencesTree->AppendItem	( nSoundRootID,		_("Playback")		);
	nDriverID		=	tcPreferencesTree->AppendItem	( nSoundRootID,		_("Driver")		);
	//--- expand all the root nodes ---//
	tcPreferencesTree->Expand( nOptionsRootID );
	tcPreferencesTree->Expand( nTagRootID );
	tcPreferencesTree->Expand( nSoundRootID );

	//-------------------------//
	//--- Sound -> Playback ---//
	//-------------------------//
	//--- shuffle, repeat, and crossfade checkboxes ---//
	chkRepeat		= new wxCheckBox( this, -1, _("Repeat"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkShuffle		= new wxCheckBox( this, -1, _("Shuffle"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkCrossfade		= new wxCheckBox( this, -1, _("Crossfade"), wxPoint( -1, -1 ), wxSize( 120, -1 ) );
	chkCrossfadeSeek	= new wxCheckBox( this, -1, _("Crossfade Seek"), wxPoint( -1, -1 ), wxSize( 120, -1 ) );
	//--- crossfade duration ---//
	tcDuration = new wxTextCtrl		( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( 32, -1 ) );
	wxStaticText *stDuration = new wxStaticText	( this, -1, _(" seconds"), wxPoint( 0, 0 ), wxSize( -1, -1 ), wxALIGN_RIGHT );
	hsDuration = new wxBoxSizer( wxHORIZONTAL );
	hsDuration->Add( chkCrossfade,	0, wxALIGN_CENTER | wxRIGHT, 3 );
	hsDuration->Add( tcDuration,	0, wxALIGN_CENTER | wxRIGHT, 3 );
	hsDuration->Add( stDuration,	0, wxALIGN_CENTER, 0);
	//--- crossfade duration ---//
	tcSeekDuration = new wxTextCtrl		( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( 32, -1 ) );
	wxStaticText *stSeekDuration = new wxStaticText	( this, -1, _(" seconds"), wxPoint( 0, 0 ), wxSize( -1, -1 ), wxALIGN_RIGHT );
	//--- duration sizer ---//
	hsSeekDuration = new wxBoxSizer( wxHORIZONTAL );
	hsSeekDuration->Add( chkCrossfadeSeek,	0, wxALIGN_CENTER | wxRIGHT, 3 );
	hsSeekDuration->Add( tcSeekDuration,	0, wxALIGN_CENTER | wxRIGHT, 3 );
	hsSeekDuration->Add( stSeekDuration,	0, wxALIGN_CENTER, 0);

	//-----------------------//
	//--- Sound -> Driver ---//
	//-----------------------//
	//--- output driver ---//
	wxStaticText *stOutputDrv		= new wxStaticText	( this, -1, _("Ouput Driver:"), wxPoint( 0, 0 ), wxSize( -1, -1 ), wxALIGN_LEFT );
	cmbOutputDrv	= new wxComboBox	( this, MUSIK_PREFERENCES_OUTPUT_DRV, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	vsOutputDrv = new wxBoxSizer( wxVERTICAL );
	vsOutputDrv->Add( stOutputDrv,	0, wxEXPAND | wxBOTTOM, 4 );
	vsOutputDrv->Add( cmbOutputDrv, 1, wxEXPAND | wxBOTTOM, 8 );
	#if defined (__WXMSW__)
		cmbOutputDrv->Append ( wxT("Direct Sound") );
		cmbOutputDrv->Append ( wxT("Windows Multimedia") );
		cmbOutputDrv->Append ( wxT("ASIO") );
	#elif defined (__WXMAC__)
		//--- mac sound stuff ---//
	#elif defined (__WXGTK__)
		cmbOutputDrv->Append ( wxT("OSS") );
		cmbOutputDrv->Append ( wxT("ESD") );
		cmbOutputDrv->Append ( wxT("ALSA 0.9") );
	#endif
	//--- sound device ---//
	wxStaticText *stSndDevice		= new wxStaticText	( this, -1, _("Sound Device:"), wxPoint( 0, 0 ), wxSize( -1, -1 ), wxALIGN_LEFT );
	cmbSndDevice	= new wxComboBox	( this, MUSIK_PREFERENCES_SND_DEVICE, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );	
	vsSndDevice = new wxBoxSizer( wxVERTICAL );
	vsSndDevice->Add( stSndDevice,	0, wxEXPAND | wxBOTTOM, 4 );
	vsSndDevice->Add( cmbSndDevice, 1, wxEXPAND | wxBOTTOM, 8 );
	//--- playrate ---//
	wxStaticText *stPlayRate = new wxStaticText( this, -1, _("Playback Rate (hz):"), wxPoint( 0, 0 ), wxSize( -1, -1 ), wxALIGN_LEFT );
	cmbPlayRate  = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbPlayRate->Append ( wxT("48000") );
	cmbPlayRate->Append ( wxT("44100") );
	cmbPlayRate->Append ( wxT("22050") );
	cmbPlayRate->Append ( wxT("11025") );
	cmbPlayRate->Append ( wxT("8000") );
	vsPlayRate = new wxBoxSizer( wxVERTICAL );
	vsPlayRate->Add( stPlayRate,0, wxEXPAND | wxBOTTOM, 4 );
	vsPlayRate->Add( cmbPlayRate, 1, wxEXPAND | wxBOTTOM, 12 );	
	//--- buffer length ---//
	tcBufferLength = new wxTextCtrl		( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( 32, -1 ) );
	wxStaticText *stBufferLength = new wxStaticText	( this, -1, _(" second buffer length"), wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	hsBufferLength = new wxBoxSizer( wxHORIZONTAL );
	hsBufferLength->Add( tcBufferLength, 0, wxALIGN_CENTER | wxRIGHT, 4 );
	hsBufferLength->Add( stBufferLength, 0, wxALIGN_CENTER, 0 );

	//----------------------------//
	//--- Options -> Selection ---//
	//----------------------------//
	//--- show unselected check box ---//
	chkShowUnsel = new wxCheckBox( this, -1, _("Show unrelated material, highlight related"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	//--- activity box 1 ---//
	wxStaticText *stActivityBox1 = new wxStaticText( this, -1, _("Selection Box 1:"), wxPoint( 0, 0 ), wxSize( -1, -1 ), wxALIGN_LEFT );
	cmbActivityBox1 = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbActivityBox1->Append ( _("None") );	
	cmbActivityBox1->Append ( _("Artist") );
	cmbActivityBox1->Append ( _("Album") );
	cmbActivityBox1->Append ( _("Genre") );
	cmbActivityBox1->Append ( _("Year") );
	/*cmbActivityBox1->Append ( _("Decade") );*/
	//--- activity box 2 ---//
	wxStaticText *stActivityBox2 = new wxStaticText( this, -1, _("Selection Box 2:"), wxPoint( 0, 0 ), wxSize( -1, -1), wxALIGN_LEFT );
	cmbActivityBox2 = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbActivityBox2->Append ( _("None") );	
	cmbActivityBox2->Append ( _("Artist") );
	cmbActivityBox2->Append ( _("Album") );
	cmbActivityBox2->Append ( _("Genre") );
	cmbActivityBox2->Append ( _("Year") );
	/*cmbActivityBox1->Append ( _("Decade") );*/
	//--- activity box 3 ---//
	wxStaticText *stActivityBox3 = new wxStaticText( this, -1, _("Selection Box 3:"), wxPoint( 0, 0 ), wxSize( -1, -1), wxALIGN_LEFT );
	cmbActivityBox3 = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbActivityBox3->Append ( _("None") );
	cmbActivityBox3->Append ( _("Artist") );
	cmbActivityBox3->Append ( _("Album") );
	cmbActivityBox3->Append ( _("Genre") );
	cmbActivityBox3->Append ( _("Year") );
	/*cmbActivityBox1->Append ( _("Decade") );*/
	//--- activity box 4 ---//
	wxStaticText *stActivityBox4 = new wxStaticText( this, -1, _("Selection Box 4:"), wxPoint( 0, 0 ), wxSize( -1, -1), wxALIGN_LEFT );
	cmbActivityBox4 = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbActivityBox4->Append ( _("None") );
	cmbActivityBox4->Append ( _("Artist") );
	cmbActivityBox4->Append ( _("Album") );
	cmbActivityBox4->Append ( _("Genre") );
	cmbActivityBox4->Append ( _("Year") );
	/*cmbActivityBox1->Append ( _("Decade") );*/
	//--- activity box sizers ---//
	hsActivityBox1 = new wxBoxSizer ( wxHORIZONTAL );
	hsActivityBox2 = new wxBoxSizer	( wxHORIZONTAL );
	hsActivityBox3 = new wxBoxSizer	( wxHORIZONTAL );
	hsActivityBox4 = new wxBoxSizer	( wxHORIZONTAL );
	hsActivityBox1->Add ( stActivityBox1,	0, wxCENTER | wxRIGHT, 4 );
	hsActivityBox2->Add ( stActivityBox2,	0, wxCENTER | wxRIGHT, 4 );
	hsActivityBox3->Add ( stActivityBox3,	0, wxCENTER | wxRIGHT, 4 );
	hsActivityBox4->Add ( stActivityBox4,	0, wxCENTER | wxRIGHT, 4 );
	hsActivityBox1->Add ( cmbActivityBox1,	1, wxCENTER, 0 );
	hsActivityBox2->Add ( cmbActivityBox2,	1, wxCENTER, 0 );
	hsActivityBox3->Add ( cmbActivityBox3,	1, wxCENTER, 0 );
	hsActivityBox4->Add ( cmbActivityBox4,	1, wxCENTER, 0 );

	//-------------------------------//
	//--- Sound -> Playback Sizer ---//
	//-------------------------------//
	vsSound_Playback = new wxBoxSizer( wxVERTICAL );
	vsSound_Playback->Add( chkRepeat,		0, wxALL, 4  );
	vsSound_Playback->Add( chkShuffle,		0, wxALL, 4  );
	vsSound_Playback->Add( hsDuration,		0, wxALL, 4  );
	vsSound_Playback->Add( hsSeekDuration,	0, wxALL, 4  );

	//-----------------------------//
	//--- Sound -> Driver Sizer ---//
	//-----------------------------//
	vsSound_Driver = new wxBoxSizer ( wxVERTICAL );
	vsSound_Driver->Add( vsOutputDrv,		0, wxALL, 4 );
	vsSound_Driver->Add( vsSndDevice,		0, wxALL, 4  );
	vsSound_Driver->Add( vsPlayRate,		0, wxALL, 4  );
	vsSound_Driver->Add( hsBufferLength,	0, wxALL, 4  );

	//--------------------------//
	//--- Options -> General ---//
	//--------------------------//
	chkAutoScan				=	new wxCheckBox( this, -1,	_("Automatically scan for new songs on startup"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkAutoDelta			=	new wxCheckBox( this, -1,	_("Automatically scan for new files in library setup"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkExtendedPlaylist		=	new wxCheckBox( this, -1,	_("Show extended playlist information"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkShowAllSongs			=	new wxCheckBox( this, -1,	_("Selecting library shows all songs in playlist"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkBlankSwears			=	new wxCheckBox( this, -1,	_("Censor common swearwords"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkPlaylistStripes		=	new wxCheckBox( this, -1,	_("Show \"stripes\" in playlist"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkActivityBoxStripes	=	new wxCheckBox( this, -1,	_("Show \"stripes\" in selection boxes"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkSourcesBoxStripes	=	new wxCheckBox( this, -1,	_("Show \"stripes\" in sources box"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );

	btnPlaylistStripeColour	=	new wxButton( this, MUSIK_PREFERENCES_PLAYLIST_STRIPE_COLOUR,	_("Set Color"), wxPoint( 0, 0 ), wxSize( 64, 24 ) );
	btnActivityStripeColour	=	new wxButton( this, MUSIK_PREFERENCES_ACTIVITY_STRIPE_COLOUR,	_("Set Color"), wxPoint( 0, 0 ), wxSize( 64, 24 ) );
	btnSourcesStripeColour	=	new wxButton( this, MUSIK_PREFERENCES_SOURCES_STRIPE_COLOUR,	_("Set Color"), wxPoint( 0, 0 ), wxSize( 64, 24 ) );

	//----------------------------------//
	//--- Options -> Interface Sizer ---//
	//----------------------------------//
	vsOptions_Interface = new wxBoxSizer( wxVERTICAL );
	vsOptions_Interface->Add( chkAutoScan,				0, wxALL, 4 );
	vsOptions_Interface->Add( chkAutoDelta,				0, wxALL, 4 );
	vsOptions_Interface->Add( chkExtendedPlaylist,		0, wxALL, 4 );
	vsOptions_Interface->Add( chkShowAllSongs,			0, wxALL, 4 );
	vsOptions_Interface->Add( chkBlankSwears,			0, wxALL, 4 );
	vsOptions_Interface->Add( chkPlaylistStripes,		0, wxALL, 4 );
	vsOptions_Interface->Add( btnPlaylistStripeColour,	0, wxALL, 4 );
	vsOptions_Interface->Add( chkActivityBoxStripes,	0, wxALL, 4 );
	vsOptions_Interface->Add( btnActivityStripeColour,	0, wxALL, 4 );
	vsOptions_Interface->Add( chkSourcesBoxStripes,		0, wxALL, 4 );
	vsOptions_Interface->Add( btnSourcesStripeColour,	0, wxALL, 4 );

	//--------------------------//
	//--- Tagging -> General ---//
	//--------------------------//
	wxStaticText *stActivityTag =	new wxStaticText( this, -1, _("Activity Boxes (artist/album/etc):"), wxPoint( 0, 0 ), wxSize( 50, -1 ), wxALIGN_LEFT );
	chkActivityWrite			=	new wxCheckBox	( this, -1,	_("Write tag to file"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkActivityClear			=	new wxCheckBox	( this, -1,	_("Clear old tag"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkActivityRename			=	new wxCheckBox	( this, -1,	_("Automatically rename file"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	wxStaticText *stTagDlgTag	=	new wxStaticText( this, -1, _("\nTag Dialog Box:"), wxPoint( 0, 0 ), wxSize( 50, -1 ), wxALIGN_LEFT );
	chkTagDlgWrite				=	new wxCheckBox	( this, -1,	_("Write tag to file"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkTagDlgClear				=	new wxCheckBox	( this, -1,	_("Clear old tag"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkTagDlgRename				=	new wxCheckBox	( this, -1,	_("Automatically rename file"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );

	//--------------------------------//
	//--- Tagging -> General Sizer ---//
	//--------------------------------//
	vsTagging_General = new wxBoxSizer( wxVERTICAL );
	vsTagging_General->Add	( stActivityTag,		0, wxALL | wxEXPAND, 4 );
	vsTagging_General->Add	( chkActivityWrite,		0, wxALL | wxEXPAND, 4 );
	vsTagging_General->Add	( chkActivityClear,		1, wxALL | wxEXPAND, 4 );
	vsTagging_General->Add	( chkActivityRename,	1, wxALL | wxEXPAND, 4 );
	vsTagging_General->Add	( stTagDlgTag,			0, wxALL | wxEXPAND, 4 );
	vsTagging_General->Add	( chkTagDlgWrite,		0, wxALL | wxEXPAND, 4 );
	vsTagging_General->Add	( chkTagDlgClear,		1, wxALL | wxEXPAND, 4 );
	vsTagging_General->Add	( chkTagDlgRename,		1, wxALL | wxEXPAND, 4 );

	//---------------------------//
	//--- Tagging -> Auto Tag ---//
	//---------------------------//
	//--- rename options and sizer ---//
	wxStaticText *stRename	= new wxStaticText	( this, -1, _("Rename:"), wxPoint( 0, 0 ), wxSize( 50, -1 ), wxALIGN_LEFT );
	tcAutoRename			= new wxTextCtrl	( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	wxBoxSizer *hsRename	= new wxBoxSizer	( wxHORIZONTAL );
	hsRename->Add ( stRename, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 2 );
	hsRename->Add ( tcAutoRename, 1, wxEXPAND, 0 );
	//--- tag options and sizer ---//
	wxStaticText *stTag		= new wxStaticText	( this, -1, _("Get Tag:"), wxPoint( 0, 0 ), wxSize( 50, -1 ), wxALIGN_LEFT );
	tcAutoTag				= new wxTextCtrl	( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	wxBoxSizer *hsTag	= new wxBoxSizer		( wxHORIZONTAL );
	hsTag->Add ( stTag, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 2 );
	hsTag->Add ( tcAutoTag, 1, wxEXPAND, 0 );
	//--- information ---//
	wxString sInfo = 
		wxString( _("\nAuto Tag and Rename syntax:\n\n") ) + 
		wxString( wxString( MUSIK_PATH_SEPARATOR ) + _( " - Directory Separator\n" ) ) +
		wxString( _("%1 - Song Title\n")				) + 
		wxString( _("%2 - Artist Name\n")				) +
		wxString( _("%3 - Album Name\n")				) +
		wxString( _("%4 - Genre\n")					) +
		wxString( _("%5 - Year\n")						) +
		wxString( _("%6 - Track Number\n\n")			) +
		wxString( _("Musik will not delete empty directories!" ) );
	wxStaticText *stInfo = new wxStaticText( this, -1, sInfo, wxPoint( 0, 0 ), wxSize( -1, -1), wxALIGN_LEFT );

	//---------------------------------//
	//--- Tagging -> Auto Tag Sizer ---//
	//---------------------------------//
	vsTagging_Auto = new wxBoxSizer( wxVERTICAL );
	vsTagging_Auto->Add	( hsRename, 0, wxALL | wxEXPAND, 4 );
	vsTagging_Auto->Add	( hsTag, 0, wxALL | wxEXPAND, 4 );
	vsTagging_Auto->Add ( stInfo, 0, wxADJUST_MINSIZE | wxALL | wxEXPAND, 4 );

	//----------------------//
	//--- System Buttons ---//
	//----------------------//
	btnCancel =	new wxButton( this, MUSIK_PREFERENCES_CANCEL,	_("Cancel"),	wxPoint( 0, 0 ), wxSize( 64, 24 ) );
	btnApply =	new wxButton( this, MUSIK_PREFERENCES_APPLY,	_("Apply"),	wxPoint( 0, 0 ), wxSize( 64, 24 ) );
	btnOK =		new wxButton( this, MUSIK_PREFERENCES_OK,		_("OK"),		wxPoint( 0, 0 ), wxSize( 64, 24 ) );

	//-----------------------------------//
	//--- Options -> Selections Sizer ---//
	//-----------------------------------//
	vsOptions_Selections = new wxBoxSizer ( wxVERTICAL );
	vsOptions_Selections->Add ( hsActivityBox1,		0, wxALL, 4 );
	vsOptions_Selections->Add ( hsActivityBox2,		0, wxALL, 4 );
	vsOptions_Selections->Add ( hsActivityBox3,		0, wxALL, 4 );
	vsOptions_Selections->Add ( hsActivityBox4,		0, wxALL, 4 );
	vsOptions_Selections->Add ( chkShowUnsel,		0, wxALL, 4 );

	//----------------------------//
	//--- System Buttons Sizer ---//
	//----------------------------//
	hsSysButtons = new wxBoxSizer( wxHORIZONTAL );
	hsSysButtons->Add( btnCancel,	0, wxALIGN_LEFT		);
	hsSysButtons->Add( wndDummy,	1, wxEXPAND			);
	hsSysButtons->Add( btnApply,	0, wxALIGN_RIGHT	);
	hsSysButtons->Add( btnOK,		0, wxALIGN_RIGHT | wxLEFT, 4);

	//-------------------------//
	//--- Hide / Show Sizer ---//
	//-------------------------//
	hsSplitter = new wxBoxSizer( wxHORIZONTAL );
	hsSplitter->Add( tcPreferencesTree,		2, wxEXPAND | wxRIGHT, 8 );
	hsSplitter->Add( vsOptions_Selections,	5 );
	hsSplitter->Add( vsOptions_Interface,	5 );
	hsSplitter->Add( vsSound_Playback,		5 );
	hsSplitter->Add( vsSound_Driver,		5 );
	hsSplitter->Add( vsTagging_General,		5 );
	hsSplitter->Add( vsTagging_Auto,		5 );

	//-----------------//
	//--- Top Sizer ---//
	//-----------------//
	vsTopSizer = new wxBoxSizer( wxVERTICAL );
	vsTopSizer->Add( hsSplitter,	1, wxEXPAND | wxALL, 2 );
	vsTopSizer->Add( hsSysButtons,	0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 2 );
	SetSizer( vsTopSizer );

	//--------------//
	//--- Layout ---//
	//--------------//
	Layout();
	Centre();
	HidePanels();

	//--------------------//
	//---  Load Prefs  ---//
	//--- Set Defaults ---//
	//--------------------//
	LoadPrefs();
	tcPreferencesTree->SelectItem( nInterfaceID );
}

//--------------//
//--- Events ---//
//--------------//
void MusikPrefsFrame::OnTreeChange		( wxCommandEvent& WXUNUSED(event) )	{	UpdatePrefsPanel();		}
void MusikPrefsFrame::OnOutputChanged	( wxCommandEvent& WXUNUSED(event) )	{	FindDevices();			}
void MusikPrefsFrame::OnClickOK			( wxCommandEvent& WXUNUSED(event) )	{	Close( false );			}
void MusikPrefsFrame::OnClickApply		( wxCommandEvent& WXUNUSED(event) )	{	SavePrefs();			}
void MusikPrefsFrame::OnClickCancel		( wxCommandEvent& WXUNUSED(event) )	{	Close( true );			}
void MusikPrefsFrame::OnClose			( wxCommandEvent& WXUNUSED(event) )	{	Close( true );			}

void MusikPrefsFrame::OnClickColour		( wxCommandEvent &event )
{
	//--- show the standard color dialog to change the background color of the button ---//
	wxButton* Button = (wxButton*)event.GetEventObject();
	Button->SetBackgroundColour( wxGetColourFromUser( this, Button->GetBackgroundColour() ) );
}

void MusikPrefsFrame::Close( bool bCancel )
{
	if ( !bCancel )
		SavePrefs();

	g_MusikFrame->Enable( TRUE );
	this->Destroy();
	
}

void MusikPrefsFrame::LoadPrefs()
{
	//--------------------------//
	//--- options -> general ---//
	//--------------------------//
	chkAutoScan->SetValue			( g_Prefs.nAutoAdd	);
	chkAutoDelta->SetValue			( g_Prefs.nAutoDelta );
	chkExtendedPlaylist->SetValue	( g_Prefs.nExtendedPlaylist );
	chkShowAllSongs->SetValue		( g_Prefs.nShowAllSongs );
	chkBlankSwears->SetValue		( g_Prefs.nBlankSwears );
	chkPlaylistStripes->SetValue	( g_Prefs.nPLStripes );
	chkActivityBoxStripes->SetValue	( g_Prefs.nActStripes );
	chkSourcesBoxStripes->SetValue	( g_Prefs.nSourcesStripes );

	btnPlaylistStripeColour->SetBackgroundColour( StringToColour( g_Prefs.sPLStripeColour ) );
	btnActivityStripeColour->SetBackgroundColour( StringToColour( g_Prefs.sActStripeColour ) );
	btnSourcesStripeColour->SetBackgroundColour( StringToColour( g_Prefs.sSourcesStripeColour ) );

	//-----------------------------//
	//--- options -> selections ---//
	//-----------------------------//
	chkShowUnsel->SetValue			( g_Prefs.nShowUnsel );
	cmbActivityBox1->SetSelection	( g_Prefs.nActBox1 );
	cmbActivityBox2->SetSelection	( g_Prefs.nActBox2 );
	cmbActivityBox3->SetSelection	( g_Prefs.nActBox3 );
	cmbActivityBox4->SetSelection	( g_Prefs.nActBox4 );

	//--------------------------//
	//--- tagging -> general ---//
	//--------------------------//
	chkActivityWrite->SetValue		( g_Prefs.nActBoxWrite	);
	chkActivityClear->SetValue		( g_Prefs.nActBoxClear	);
	chkActivityRename->SetValue		( g_Prefs.nActBoxRename );
	chkTagDlgWrite->SetValue		( g_Prefs.nTagDlgWrite	);
	chkTagDlgClear->SetValue		( g_Prefs.nTagDlgClear	);
	chkTagDlgRename->SetValue		( g_Prefs.nTagDlgRename );

	//---------------------------//
	//--- tagging -> auto tag ---//
	//---------------------------//
	tcAutoRename->SetValue			( g_Prefs.sAutoRename	);
	tcAutoTag->SetValue				( g_Prefs.sAutoTag		);

	//-------------------------//
	//--- sound -> playback ---//
	//-------------------------//
	float		fDuration, fSeekDuration;
	wxString	sDuration, sSeekDuration;
	chkRepeat->SetValue				( g_Prefs.nRepeat );
	chkShuffle->SetValue			( g_Prefs.nShuffle );
	chkCrossfade->SetValue			( g_Prefs.nFadeEnable );
	fDuration =						(float)g_Prefs.nFadeDuration / 1000;
	sDuration.sprintf				( wxT("%.1f"), fDuration );
	tcDuration->SetValue			( sDuration );
	chkCrossfadeSeek->SetValue		( g_Prefs.nFadeSeekEnable );
	fSeekDuration =					(float)g_Prefs.nFadeSeekDuration / 1000;
	sSeekDuration.sprintf			( wxT("%.1f"), fSeekDuration );
	tcSeekDuration->SetValue		( sSeekDuration );

	//-----------------------//
	//--- sound -> driver ---//
	//-----------------------//
	FindDevices();
	wxString sSndRate, sLength;
	float fLength;
	cmbOutputDrv->SetSelection		( g_Prefs.nSndOutput );
    cmbSndDevice->SetSelection		( g_Prefs.nSndDevice );
	sSndRate.sprintf				( wxT("%d"), g_Prefs.nSndRate );
	fLength =						(float)g_Prefs.nSndBuffer / 1000;
	sLength.sprintf					( wxT("%.1f"), fLength );
	tcBufferLength->SetValue		( sLength );
	cmbPlayRate->SetSelection		( cmbPlayRate->FindString ( sSndRate ) );
}

void MusikPrefsFrame::FindDevices()
{
	cmbSndDevice->Clear();
	for ( int i = 0; i < FSOUND_GetNumDrivers(); i++ )
		cmbSndDevice->Append( ConvA2W( FSOUND_GetDriverName( i ) ) );
	if ( cmbSndDevice->GetCount() < 1 )
		cmbSndDevice->Append( _("[No Devices]") );
	cmbSndDevice->SetSelection( 0 );
}

void MusikPrefsFrame::HidePanels()
{
	hsSplitter->Show( vsOptions_Selections,	false );
	hsSplitter->Show( vsOptions_Interface,	false );
	hsSplitter->Show( vsSound_Playback,		false );
	hsSplitter->Show( vsSound_Driver,		false );
	hsSplitter->Show( vsTagging_General,	false );
	hsSplitter->Show( vsTagging_Auto,		false );
}

void MusikPrefsFrame::UpdatePrefsPanel()
{
	if( tcPreferencesTree->GetSelection() == nPlaybackID )
	{
		HidePanels();
		hsSplitter->Show( vsSound_Playback, true );
	}
	else if( tcPreferencesTree->GetSelection() == nDriverID )
	{
		HidePanels();
		hsSplitter->Show( vsSound_Driver, true );
	}
	else if( tcPreferencesTree->GetSelection() == nSelectionsID )
	{
		HidePanels();
		hsSplitter->Show( vsOptions_Selections, true );
	}
	else if( tcPreferencesTree->GetSelection() == nInterfaceID )
	{
		HidePanels();
		hsSplitter->Show( vsOptions_Interface, true );
	}
	else if ( tcPreferencesTree->GetSelection() == nAutoTagID )
	{
		HidePanels();
		hsSplitter->Show( vsTagging_Auto, true );
	}
	else if ( tcPreferencesTree->GetSelection() == nGeneralTagID )
	{
		HidePanels();
		hsSplitter->Show( vsTagging_General, true );
	}
	this->Layout();
}

void MusikPrefsFrame::OnTranslateKeys( wxKeyEvent& event )
{
	if ( event.GetKeyCode() == WXK_ESCAPE )
		Close( true );
	event.Skip();
}

void MusikPrefsFrame::SavePrefs()
{
	bool bRestartFMOD		= false;
	bool bShowUnselChange	= false;
	bool bActivityChange	= false;
	bool bPlaymodeChange	= false;

	//--------------------------//
	//--- Options -> general ---//
	//--------------------------//
	g_Prefs.nAutoDelta	= chkAutoDelta->GetValue();
	g_Prefs.nAutoAdd	= chkAutoScan->GetValue();
	if ( chkExtendedPlaylist->GetValue() != g_Prefs.nExtendedPlaylist )
	{
		g_Prefs.nExtendedPlaylist = chkExtendedPlaylist->GetValue();
		g_PlaylistCtrl->ResetColumns( true );
	}
	g_Prefs.nShowAllSongs = chkShowAllSongs->GetValue();
	g_Prefs.nBlankSwears = chkBlankSwears->GetValue();

	bool bPlaylistUpdate = false;
	if ( chkPlaylistStripes->GetValue() != g_Prefs.nPLStripes )
	{
		g_Prefs.nPLStripes = chkPlaylistStripes->GetValue();
		bPlaylistUpdate = true;
	}
	if ( ColourToString( btnPlaylistStripeColour->GetBackgroundColour() ) != g_Prefs.sPLStripeColour )
	{
		g_Prefs.sPLStripeColour = ColourToString( btnPlaylistStripeColour->GetBackgroundColour() );
		bPlaylistUpdate = true;
	}
	if ( bPlaylistUpdate )
		g_PlaylistCtrl->Update();

	bool bActivityUpdate = false;
	if ( chkActivityBoxStripes->GetValue() != g_Prefs.nActStripes )
	{
		g_Prefs.nActStripes = chkActivityBoxStripes->GetValue();
		bActivityUpdate = true;
	}
	if ( ColourToString( btnActivityStripeColour->GetBackgroundColour() ) != g_Prefs.sActStripeColour )
	{
		g_Prefs.sActStripeColour = ColourToString( btnActivityStripeColour->GetBackgroundColour() );
		bActivityUpdate = true;
	}
	if ( bActivityUpdate )
		g_ActivityAreaCtrl->ResetAllContents();

	bool bSourcesUpdate = false;
	if ( chkSourcesBoxStripes->GetValue() != g_Prefs.nSourcesStripes )
	{
		g_Prefs.nSourcesStripes = chkSourcesBoxStripes->GetValue();
		bSourcesUpdate = true;
	}
	if ( ColourToString( btnSourcesStripeColour->GetBackgroundColour() ) != g_Prefs.sSourcesStripeColour )
	{
		g_Prefs.sSourcesStripeColour = ColourToString( btnSourcesStripeColour->GetBackgroundColour() );
		bSourcesUpdate = true;
	}
	if ( bSourcesUpdate )
		g_SourcesCtrl->Update();

	//-----------------------------//
    //--- Options -> selections ---//
	//-----------------------------//
	if ( chkShowUnsel->GetValue() != g_Prefs.nShowUnsel )
	{
		g_Prefs.nShowUnsel = chkShowUnsel->GetValue();
		bShowUnselChange = true;
	}

	if ( cmbActivityBox1->GetSelection() != g_Prefs.nActBox1 )
	{
		g_Prefs.nActBox1 = cmbActivityBox1->GetSelection();
		bActivityChange = true;
	}

	if ( cmbActivityBox2->GetSelection() != g_Prefs.nActBox2 )
	{
		g_Prefs.nActBox2 = cmbActivityBox2->GetSelection();
		bActivityChange = true;
	}

	if ( cmbActivityBox3->GetSelection() != g_Prefs.nActBox3 )
	{
		g_Prefs.nActBox3 = cmbActivityBox3->GetSelection();
		bActivityChange = true;
	}

	if ( cmbActivityBox4->GetSelection() != g_Prefs.nActBox4 )
	{
		g_Prefs.nActBox4 = cmbActivityBox4->GetSelection();
		bActivityChange = true;
	}

	//--------------------------//
	//--- tagging -> general ---//
	//--------------------------//
	g_Prefs.nActBoxWrite	= chkActivityWrite->GetValue();
	g_Prefs.nActBoxClear	= chkActivityClear->GetValue();
	g_Prefs.nActBoxRename	= chkActivityRename->GetValue();
	g_Prefs.nTagDlgWrite	= chkTagDlgWrite->GetValue();
	g_Prefs.nTagDlgClear	= chkTagDlgClear->GetValue();
	g_Prefs.nTagDlgRename	= chkTagDlgRename->GetValue();

	//-----------------------//
	//--- tagging -> auto ---//
	//-----------------------//
	g_Prefs.sAutoRename = tcAutoRename->GetValue();
	g_Prefs.sAutoTag	= tcAutoTag->GetValue();

	//-------------------------//
	//--- sound -> playback ---//
	//-------------------------//
	if ( chkRepeat->GetValue() != g_Prefs.nRepeat )
	{
		g_Prefs.nRepeat = chkRepeat->GetValue();
		bPlaymodeChange = true;
	}
	if ( chkShuffle->GetValue() != g_Prefs.nShuffle )
	{
		g_Prefs.nShuffle = chkShuffle->GetValue();
		bPlaymodeChange = true;
	}
	if ( tcDuration->GetValue() != g_Prefs.nFadeDuration )
	{
        double fDuration = StringToDouble( tcDuration->GetValue() );
		int nDuration = ( int )( fDuration * 1000 );
		g_Prefs.nFadeDuration = nDuration;
	}
	g_Prefs.nFadeEnable = chkCrossfade->GetValue();
	if ( tcSeekDuration->GetValue() != g_Prefs.nFadeSeekDuration )
	{
        double fSeekDuration = StringToDouble( tcSeekDuration->GetValue() );
		int nSeekDuration = ( int )( fSeekDuration * 1000 );
		g_Prefs.nFadeSeekDuration = nSeekDuration;
	}
	g_Prefs.nFadeSeekEnable = chkCrossfadeSeek->GetValue();

	//-----------------------//
	//--- sound -> driver ---//
	//-----------------------//
	if ( cmbOutputDrv->GetSelection() != g_Prefs.nSndOutput )
	{
		g_Prefs.nSndOutput = cmbOutputDrv->GetSelection();
		bRestartFMOD = true;
	}
	if ( cmbSndDevice->GetSelection() != g_Prefs.nSndDevice )
	{
		g_Prefs.nSndDevice = cmbSndDevice->GetSelection();
		bRestartFMOD = true;
	}
	if ( wxStringToInt( cmbPlayRate->GetString( cmbPlayRate->GetSelection() ) ) != g_Prefs.nSndRate )
	{
		int nRate = wxStringToInt( cmbPlayRate->GetString( cmbPlayRate->GetSelection() ) );
		g_Prefs.nSndRate = nRate;
		bRestartFMOD = true;
	}
	if ( tcBufferLength->GetValue() != g_Prefs.nSndBuffer )
	{
		double fLength = StringToDouble( tcBufferLength->GetValue() );
		int nLength = ( int )( fLength * 1000 );
		g_Prefs.nSndBuffer = nLength;
	}

	//--- save ---//
	g_Prefs.SavePrefs();

	//--- if we need to restart fmod ---//
	if ( bRestartFMOD )
		g_Player.InitializeFMOD( FMOD_INIT_RESTART, g_Prefs.nSndOutput, g_Prefs.nSndDevice, g_Prefs.nSndRate );

	//--- if playmode ( repeat / etc ) ---//
	if ( bPlaymodeChange )
		g_Player.SetPlaymode();

	if ( bActivityChange )
	{
		g_ActivityAreaCtrl->Delete();
		g_ActivityAreaCtrl->Create();
		g_MusikFrame->Layout();
		g_ActivityAreaCtrl->ResetAllContents();
	}
	else if ( bShowUnselChange )
		g_ActivityAreaCtrl->ResetAllContents();
}