/*
 *  MusikPrefsFrame.cpp
 *
 *  Preferences frame
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
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
	nRootID			=	tcPreferencesTree->AddRoot		( _( "Preferences" )		);
	nOptionsRootID	=	tcPreferencesTree->AppendItem	( nRootID, _( "Options" )	);
	nTagRootID		=	tcPreferencesTree->AppendItem	( nRootID, _( "Tagging" )	);
	nSoundRootID	=	tcPreferencesTree->AppendItem	( nRootID, _( "Sound" )		);	
	wxTreeItemId nStreamingRootID	=	tcPreferencesTree->AppendItem	( nRootID, _( "Streaming" )	);
	//--- child nodes ---//
	nInterfaceID	=	tcPreferencesTree->AppendItem	( nOptionsRootID,	_( "General" )		);
	nSelectionsID	=	tcPreferencesTree->AppendItem	( nOptionsRootID,	_( "Selections" )	);
	nPlaylistID		=	tcPreferencesTree->AppendItem	( nOptionsRootID,	_( "Playlist" )		);
	nTunageID		=	tcPreferencesTree->AppendItem	( nOptionsRootID,	_( "Tunage" )		);
	nGeneralTagID	=	tcPreferencesTree->AppendItem	( nTagRootID,		_( "General" )		);
	nAutoTagID		=	tcPreferencesTree->AppendItem	( nTagRootID,		_( "Auto Rename" )		);
	nDriverID		=	tcPreferencesTree->AppendItem	( nSoundRootID,		_( "Driver" )		);
	nPlaybackID		=	tcPreferencesTree->AppendItem	( nSoundRootID,		_( "Crossfader" )	);
	nStreamingBufferID	=	tcPreferencesTree->AppendItem	( nStreamingRootID, _( "Buffer" )	);
   	nStreamingProxyServerID	=	tcPreferencesTree->AppendItem	( nStreamingRootID, _( "Proxy Server" )	);

	//--- expand all the root nodes ---//
	tcPreferencesTree->Expand( nOptionsRootID );
	tcPreferencesTree->Expand( nTagRootID );
	tcPreferencesTree->Expand( nSoundRootID );
 	tcPreferencesTree->Expand( nSoundRootID );
	tcPreferencesTree->Expand( nStreamingRootID );

	//--------------------------//
	//--- Sound -> Crossfader ---//
	//---------------------------//
	chkCrossfade			= new wxCheckBox( this, -1, _("Crossfade on new song (seconds)"), 			wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkCrossfadeSeek		= new wxCheckBox( this, -1, _("Crossfade on track seek (seconds)"), 		wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkCrossfadePauseResume	= new wxCheckBox( this, -1, _("Crossfade on pause or resume (seconds)"),	wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkCrossfadeStop		= new wxCheckBox( this, -1, _("Crossfade on stop (seconds)"),				wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkCrossfadeExit		= new wxCheckBox( this, -1, _("Crossfade on program exit (seconds)"),		wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	tcDuration 				= new wxTextCtrl( this, -1, _(""), 											wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	tcSeekDuration 			= new wxTextCtrl( this, -1, _(""), 											wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	tcPauseResumeDuration	= new wxTextCtrl( this, -1, _(""), 											wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	tcStopDuration			= new wxTextCtrl( this, -1, _(""), 											wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	tcExitDuration			= new wxTextCtrl( this, -1, _(""), 											wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	//--- crossfader sizer ---//
	fsCrossfader = new wxFlexGridSizer( 5, 2, 2, 2 );
	fsCrossfader->Add( chkCrossfade				);
	fsCrossfader->Add( tcDuration 				);
	fsCrossfader->Add( chkCrossfadeSeek			);
	fsCrossfader->Add( tcSeekDuration		 	);
	fsCrossfader->Add( chkCrossfadePauseResume	);
	fsCrossfader->Add( tcPauseResumeDuration	);
	fsCrossfader->Add( chkCrossfadeStop			);
	fsCrossfader->Add( tcStopDuration			);
	fsCrossfader->Add( chkCrossfadeExit			);
	fsCrossfader->Add( tcExitDuration			);

	//---------------------------------//
	//--- Sound -> Crossfader Sizer ---//
	//---------------------------------//
	vsSound_Crossfader = new wxBoxSizer( wxVERTICAL );
	vsSound_Crossfader->Add( fsCrossfader,	0, wxALL, 4  );

	//-----------------------//
	//--- Sound -> Driver ---//
	//-----------------------//
	//--- output driver ---//
	wxStaticText *stOutputDrv = new wxStaticText	( this, -1, _("Ouput Driver:"), wxPoint( 0, 0 ), wxSize( -1, -1 ), wxALIGN_LEFT );
	cmbOutputDrv = new wxComboBox ( this, MUSIK_PREFERENCES_OUTPUT_DRV, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
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
	wxStaticText *stSndDevice = new wxStaticText	( this, -1, _("Sound Device:"), wxPoint( 0, 0 ), wxSize( -1, -1 ), wxALIGN_LEFT );
	cmbSndDevice	= new wxComboBox ( this, MUSIK_PREFERENCES_SND_DEVICE, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );	
	//--- playrate ---//
	wxStaticText *stPlayRate = new wxStaticText( this, -1, _("Playback Rate (hz):"), wxPoint( 0, 0 ), wxSize( -1, -1 ), wxALIGN_LEFT );
	cmbPlayRate  = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbPlayRate->Append ( wxT("48000") );
	cmbPlayRate->Append ( wxT("44100") );
	cmbPlayRate->Append ( wxT("22050") );
	cmbPlayRate->Append ( wxT("11025") );
	cmbPlayRate->Append ( wxT("8000") );
	//--- buffer length ---//
	tcBufferLength = new wxTextCtrl	( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( 32, -1 ) );
	wxStaticText *stBufferLength = new wxStaticText	( this, -1, _(" second buffer length"), wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	//--- max channels ---//
	tcMaxChannels = new wxTextCtrl	( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( 32, -1 ) );
	wxStaticText *stMaxChannels = new wxStaticText( this, -1, _("Maximum sound channels:"), wxPoint( 0, 0 ), wxSize( -1, -1 ), wxALIGN_LEFT );
	
	//Use_MPEGACCURATE_ForMP3VBRFiles
	chkUse_MPEGACCURATE_ForMP3VBRFiles = new wxCheckBox( this, -1, _("Use MPEGACCURATE for Mp3-VBR Files"),		wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	
	//-----------------------------//
	//--- Sound -> Driver Sizer ---//
	//-----------------------------//

	wxFlexGridSizer *fsSound_Driver = new wxFlexGridSizer( 6, 2, 2, 2 );
	fsSound_Driver->Add( stOutputDrv, 0, wxCENTER | wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
	fsSound_Driver->Add( cmbOutputDrv, 1, wxCENTER, 0 );
	fsSound_Driver->Add( stSndDevice, 0, wxCENTER | wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
	fsSound_Driver->Add( cmbSndDevice, 1, wxCENTER, 0 );
	fsSound_Driver->Add( stPlayRate, 0, wxCENTER | wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
	fsSound_Driver->Add( cmbPlayRate, 1, wxCENTER, 0 );	
	fsSound_Driver->Add( stBufferLength, 0, wxCENTER | wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
	fsSound_Driver->Add( tcBufferLength, 1, wxCENTER, 0 );
	fsSound_Driver->Add( stMaxChannels, 0, wxCENTER | wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
	fsSound_Driver->Add( tcMaxChannels, 1, wxCENTER, 0 );

	vsSound_Driver = new wxBoxSizer( wxVERTICAL );
	vsSound_Driver->Add( fsSound_Driver,	0, wxALL, 4  );
	vsSound_Driver->Add( chkUse_MPEGACCURATE_ForMP3VBRFiles );

	//----------------------------//
	//--- Options -> Selection ---//
	//----------------------------//
	//--- activity box 1 ---//
	wxStaticText *stActivityBox1 = new wxStaticText( this, -1, _("Selection Box 1:"), wxPoint( 0, 0 ), wxSize( -1, -1 ), wxALIGN_LEFT );
	cmbActivityBox1 = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbActivityBox1->Append ( _("None") );	
	cmbActivityBox1->Append ( _("Artist") );
	cmbActivityBox1->Append ( _("Album") );
	cmbActivityBox1->Append ( _("Genre") );
	cmbActivityBox1->Append ( _("Year") );
	//--- activity box 2 ---//
	wxStaticText *stActivityBox2 = new wxStaticText( this, -1, _("Selection Box 2:"), wxPoint( 0, 0 ), wxSize( -1, -1), wxALIGN_LEFT );
	cmbActivityBox2 = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbActivityBox2->Append ( _("None") );	
	cmbActivityBox2->Append ( _("Artist") );
	cmbActivityBox2->Append ( _("Album") );
	cmbActivityBox2->Append ( _("Genre") );
	cmbActivityBox2->Append ( _("Year") );
	//--- activity box 3 ---//
	wxStaticText *stActivityBox3 = new wxStaticText( this, -1, _("Selection Box 3:"), wxPoint( 0, 0 ), wxSize( -1, -1), wxALIGN_LEFT );
	cmbActivityBox3 = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbActivityBox3->Append ( _("None") );
	cmbActivityBox3->Append ( _("Artist") );
	cmbActivityBox3->Append ( _("Album") );
	cmbActivityBox3->Append ( _("Genre") );
	cmbActivityBox3->Append ( _("Year") );
	//--- activity box 4 ---//
	wxStaticText *stActivityBox4 = new wxStaticText( this, -1, _("Selection Box 4:"), wxPoint( 0, 0 ), wxSize( -1, -1), wxALIGN_LEFT );
	cmbActivityBox4 = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbActivityBox4->Append ( _("None") );
	cmbActivityBox4->Append ( _("Artist") );
	cmbActivityBox4->Append ( _("Album") );
	cmbActivityBox4->Append ( _("Genre") );
	cmbActivityBox4->Append ( _("Year") );
	//--- selection style ---//
	wxStaticText *stSelStyle = new wxStaticText( this, -1, _("Selection Style:" ), wxPoint( 0, 0 ), wxSize( -1, -1 ), wxALIGN_LEFT );
	cmbSelStyle = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbSelStyle->Append( _( "Standard" ) );
	cmbSelStyle->Append( _( "Sloppy" ) );
	cmbSelStyle->Append( _( "Highlight") );

	//-----------------------------------//
	//--- Options -> Selections Sizer ---//
	//-----------------------------------//
	vsOptions_Selections = new wxFlexGridSizer ( 4, 2, 2, 2 );
	vsOptions_Selections->Add( stActivityBox1,	0, wxCENTER | wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
	vsOptions_Selections->Add( cmbActivityBox1,	1, wxCENTER, 0 );
	vsOptions_Selections->Add( stActivityBox2,	0, wxCENTER | wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
	vsOptions_Selections->Add( cmbActivityBox2,	1, wxCENTER, 0 );
	vsOptions_Selections->Add( stActivityBox3,	0, wxCENTER | wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
	vsOptions_Selections->Add( cmbActivityBox3,	1, wxCENTER, 0 );
	vsOptions_Selections->Add( stActivityBox4,	0, wxCENTER | wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
	vsOptions_Selections->Add( cmbActivityBox4,	1, wxCENTER, 0 );
	vsOptions_Selections->Add( stSelStyle,		0, wxCENTER | wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
	vsOptions_Selections->Add( cmbSelStyle,		1, wxCENTER, 0 );

	//--------------------------//
	//--- Options -> General ---//
	//--------------------------//
	chkAutoScan				=	new wxCheckBox( this, -1,	_("Automatically scan for new songs on startup"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkShowAllSongs			=	new wxCheckBox( this, -1,	_("Selecting library shows all songs in playlist"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkBlankSwears			=	new wxCheckBox( this, -1,	_("Censor common swearwords"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkSortArtistWithoutPrefix	=	new wxCheckBox( this, -1,	_("Sort Artist without prefix"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkPlaylistStripes		=	new wxCheckBox( this, -1,	_("Show \"stripes\" in playlist"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkActivityBoxStripes	=	new wxCheckBox( this, -1,	_("Show \"stripes\" in selection boxes"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	chkSourcesBoxStripes	=	new wxCheckBox( this, -1,	_("Show \"stripes\" in sources box"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );

	btnPlaylistStripeColour	=	new wxButton( this, MUSIK_PREFERENCES_PLAYLIST_STRIPE_COLOUR,	_("Set Color"), wxPoint( 0, 0 ), wxSize( 64, 24 ) );
	btnActivityStripeColour	=	new wxButton( this, MUSIK_PREFERENCES_ACTIVITY_STRIPE_COLOUR,	_("Set Color"), wxPoint( 0, 0 ), wxSize( 64, 24 ) );
	btnSourcesStripeColour	=	new wxButton( this, MUSIK_PREFERENCES_SOURCES_STRIPE_COLOUR,	_("Set Color"), wxPoint( 0, 0 ), wxSize( 64, 24 ) );

	//--------------------------------//
	//--- Options -> General Sizer ---//
	//--------------------------------//
	vsOptions_Interface = new wxBoxSizer( wxVERTICAL );
	vsOptions_Interface->Add( chkAutoScan,				0, wxALL, 4 );
	vsOptions_Interface->Add( chkShowAllSongs,			0, wxALL, 4 );
	vsOptions_Interface->Add( chkBlankSwears,			0, wxALL, 4 );
	vsOptions_Interface->Add( chkSortArtistWithoutPrefix,0, wxALL, 4 );
	vsOptions_Interface->Add( chkPlaylistStripes,		0, wxALL, 4 );
	vsOptions_Interface->Add( btnPlaylistStripeColour,	0, wxALL, 4 );
	vsOptions_Interface->Add( chkActivityBoxStripes,	0, wxALL, 4 );
	vsOptions_Interface->Add( btnActivityStripeColour,	0, wxALL, 4 );
	vsOptions_Interface->Add( chkSourcesBoxStripes,		0, wxALL, 4 );
	vsOptions_Interface->Add( btnSourcesStripeColour,	0, wxALL, 4 );

	//---------------------------//
	//--- Options -> Playlist ---//
	//---------------------------//

	chkRatingEnable			= new wxCheckBox(	this, -1, _("Rating"),			wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	chkTrackEnable			= new wxCheckBox(	this, -1, _("Track Number:"),	wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	chkTitleEnable			= new wxCheckBox(	this, -1, _("Title:"),			wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	chkArtistEnable			= new wxCheckBox(	this, -1, _("Artist:"),			wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	chkAlbumEnable			= new wxCheckBox(	this, -1, _("Album:"),			wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	chkYearEnable			= new wxCheckBox(	this, -1, _("Year:"),			wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	chkGenreEnable			= new wxCheckBox(	this, -1, _("Genre:"),			wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	chkTimesPlayedEnable	= new wxCheckBox(	this, -1, _("Times Played:"),	wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	chkLastPlayedEnable		= new wxCheckBox(	this, -1, _("Last Played:"),	wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	chkTimeEnable			= new wxCheckBox(	this, -1, _("Time:"),			wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	chkBitrateEnable		= new wxCheckBox(	this, -1, _("Bitrate:"),		wxPoint( 0, 0 ), wxSize( -1, -1 ) );
	chkFilenameEnable		= new wxCheckBox(	this, -1, _("Filename:"),		wxPoint( 0, 0 ), wxSize( -1, -1 ) );

	//--- rating combo box ---//
	cmbRatingStatic	= new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbRatingStatic->Append( _("Static")  );
	cmbRatingStatic->Append( _("Dynamic") );

	//--- track num combo box ---//
	cmbTrackStatic	= new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbTrackStatic->Append( _("Static")  );
	cmbTrackStatic->Append( _("Dynamic") );

	//--- title combo box ---//
	cmbTitleStatic = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbTitleStatic->Append( _("Static")  );
	cmbTitleStatic->Append( _("Dynamic") );

	//--- artist combo box ---//
	cmbArtistStatic	= new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbArtistStatic->Append	( _("Static")  );
	cmbArtistStatic->Append	( _("Dynamic") );

	//--- album combo box ---//
	cmbAlbumStatic = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbAlbumStatic->Append( _("Static")  );
	cmbAlbumStatic->Append( _("Dynamic") );

	//--- year combo box ---//
	cmbYearStatic = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbYearStatic->Append( _("Static")  );
	cmbYearStatic->Append( _("Dynamic") );

	//--- genre combo box ---//
	cmbGenreStatic = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbGenreStatic->Append( _("Static")  );
	cmbGenreStatic->Append( _("Dynamic") );

	//--- times played combo box ---//
	cmbTimesPlayedStatic = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbTimesPlayedStatic->Append( _("Static")  );
	cmbTimesPlayedStatic->Append( _("Dynamic") );

	//--- last played combo box ---//
	cmbLastPlayedStatic	= new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbLastPlayedStatic->Append( _("Static")  );
	cmbLastPlayedStatic->Append( _("Dynamic") );

	//--- time combo box ---//
	cmbTimeStatic = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbTimeStatic->Append( _("Static")  );
	cmbTimeStatic->Append( _("Dynamic") );

	//--- bitrate combo box ---//
	cmbBitrateStatic = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbBitrateStatic->Append( _("Static")  );
	cmbBitrateStatic->Append( _("Dynamic") );

	//--- filename ---//
	cmbFilenameStatic = new wxComboBox( this, -1, wxT(""), wxPoint( 0, 0 ), wxSize( -1, -1 ), 0, NULL, wxCB_READONLY );
	cmbFilenameStatic->Append( _("Static")  );
	cmbFilenameStatic->Append( _("Dynamic") );

	//---------------------------------//
	//--- Options -> Playlist Sizer ---//
	//---------------------------------//
	vsOptions_Playlist = new wxFlexGridSizer( 12, 2, 2, 2 );
	vsOptions_Playlist->Add( chkRatingEnable,		0, wxALIGN_CENTER_VERTICAL | wxADJUST_MINSIZE );
	vsOptions_Playlist->Add( cmbRatingStatic,		0 );
	vsOptions_Playlist->Add( chkTrackEnable,		0, wxALIGN_CENTER_VERTICAL | wxADJUST_MINSIZE );
	vsOptions_Playlist->Add( cmbTrackStatic,		0 );
	vsOptions_Playlist->Add( chkTitleEnable,		0, wxALIGN_CENTER_VERTICAL | wxADJUST_MINSIZE );
	vsOptions_Playlist->Add( cmbTitleStatic,		0 );
	vsOptions_Playlist->Add( chkArtistEnable,		0, wxALIGN_CENTER_VERTICAL | wxADJUST_MINSIZE );
	vsOptions_Playlist->Add( cmbArtistStatic,		0 );
	vsOptions_Playlist->Add( chkAlbumEnable,		0, wxALIGN_CENTER_VERTICAL | wxADJUST_MINSIZE );
	vsOptions_Playlist->Add( cmbAlbumStatic,		0 );
	vsOptions_Playlist->Add( chkYearEnable,			0, wxALIGN_CENTER_VERTICAL | wxADJUST_MINSIZE );
	vsOptions_Playlist->Add( cmbYearStatic,			0 );
	vsOptions_Playlist->Add( chkGenreEnable,		0, wxALIGN_CENTER_VERTICAL | wxADJUST_MINSIZE );
	vsOptions_Playlist->Add( cmbGenreStatic,		0 );
	vsOptions_Playlist->Add( chkTimesPlayedEnable,	0, wxALIGN_CENTER_VERTICAL | wxADJUST_MINSIZE );
	vsOptions_Playlist->Add( cmbTimesPlayedStatic,	0 );
	vsOptions_Playlist->Add( chkLastPlayedEnable,	0, wxALIGN_CENTER_VERTICAL | wxADJUST_MINSIZE );
	vsOptions_Playlist->Add( cmbLastPlayedStatic,	0 );
	vsOptions_Playlist->Add( chkTimeEnable,			0, wxALIGN_CENTER_VERTICAL | wxADJUST_MINSIZE );
	vsOptions_Playlist->Add( cmbTimeStatic,			0 );
	vsOptions_Playlist->Add( chkBitrateEnable,		0, wxALIGN_CENTER_VERTICAL | wxADJUST_MINSIZE );
	vsOptions_Playlist->Add( cmbBitrateStatic,		0 );
	vsOptions_Playlist->Add( chkFilenameEnable,		0, wxALIGN_CENTER_VERTICAL | wxADJUST_MINSIZE );
	vsOptions_Playlist->Add( cmbFilenameStatic,		0 );

	//-------------------------//
	//--- options -> tunage ---//
	//-------------------------//
	chkTunageWriteFile	= new wxCheckBox( this, -1, _("Enable"),			wxPoint(0,0), wxSize(-1,-1) );
	chkTunageAppendFile = new wxCheckBox( this, -1, _("Append to file"),			wxPoint(0,0), wxSize(-1,-1) );
	chkTunagePostURL	= new wxCheckBox( this, -1, _("Enable"),			wxPoint(0,0), wxSize(-1,-1) );
	chkTunageRunApp		= new wxCheckBox( this, -1, _("Enable"),				wxPoint(0,0), wxSize(-1,-1) );
	chkTunageRunOnStop	= new wxCheckBox( this, -1, _("Update when Musik stops"),	wxPoint(0,0), wxSize(-1,-1) );

	wxStaticText *stTunageFilename	= new wxStaticText( this, -1, _("Filename:"),		wxPoint(0,0), wxSize( 75, -1 ), wxALIGN_LEFT );
	wxStaticText *stTunageFileLine	= new wxStaticText( this, -1, _("Line to write:"),	wxPoint(0,0), wxSize( 75, -1 ), wxALIGN_LEFT );
	wxStaticText *stTunageURL		= new wxStaticText( this, -1, _("URL:"),			wxPoint(0,0), wxSize( 75, -1 ), wxALIGN_LEFT );
	wxStaticText *stTunageCmdLine	= new wxStaticText( this, -1, _("Command line:"),	wxPoint(0,0), wxSize( 75, -1 ), wxALIGN_LEFT );
	wxStaticText *stTunageStoppedText = new wxStaticText( this, -1, _("Stopped Text:"),	wxPoint(0,0), wxSize( 75, -1 ), wxALIGN_LEFT );

	tcTunageFilename	= new wxTextCtrl( this, -1, _(""),	wxPoint(0,0), wxSize( -1, -1 ) );
	tcTunageFileLine	= new wxTextCtrl( this, -1, _(""),	wxPoint(0,0), wxSize( -1, -1 ) );
	tcTunageURL			= new wxTextCtrl( this, -1, _(""),	wxPoint(0,0), wxSize( -1, -1 ) );
	tcTunageCmdLine		= new wxTextCtrl( this, -1, _(""),	wxPoint(0,0), wxSize( -1, -1 ) );
	tcTunageStoppedText = new wxTextCtrl( this, -1, _(""),	wxPoint(0,0), wxSize( -1, -1 ) );

	wxBoxSizer *hsTunageFilename = new wxBoxSizer( wxHORIZONTAL );
	hsTunageFilename->Add( stTunageFilename, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 2 );
	hsTunageFilename->Add( tcTunageFilename, 1, wxEXPAND, 0 );
	wxBoxSizer *hsTunageFileLine = new wxBoxSizer( wxHORIZONTAL );
	hsTunageFileLine->Add( stTunageFileLine, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 2 );
	hsTunageFileLine->Add( tcTunageFileLine, 1, wxEXPAND, 0 );
	wxBoxSizer *hsTunageURL = new wxBoxSizer( wxHORIZONTAL );
	hsTunageURL->Add( stTunageURL, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 2 );
	hsTunageURL->Add( tcTunageURL, 1, wxEXPAND, 0 );
	wxBoxSizer *hsTunageCmdLine = new wxBoxSizer( wxHORIZONTAL );
	hsTunageCmdLine->Add( stTunageCmdLine, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 2 );
	hsTunageCmdLine->Add( tcTunageCmdLine, 1, wxEXPAND, 0 );
	wxBoxSizer *hsTunageStoppedText = new wxBoxSizer( wxHORIZONTAL );
	hsTunageStoppedText->Add( stTunageStoppedText, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 2 );
	hsTunageStoppedText->Add( tcTunageStoppedText, 1, wxEXPAND, 0 );

	
	sbTunageFile = new wxStaticBox( this, -1, _("Write to file") );
	sbTunageURL = new wxStaticBox( this, -1, _("Post to a website") );
	sbTunageApp = new wxStaticBox( this, -1, _("Run a program") );
	sbTunageMisc = new wxStaticBox( this, -1, _("Misc.") );

	wxStaticBoxSizer *vsTunageFile = new wxStaticBoxSizer( sbTunageFile, wxVERTICAL );
	vsTunageFile->Add( chkTunageWriteFile,		0, wxALL, 2 );
	vsTunageFile->Add( hsTunageFilename,		0, wxALL | wxEXPAND, 2 );
	vsTunageFile->Add( hsTunageFileLine,		0, wxALL | wxEXPAND, 2 );
	vsTunageFile->Add( chkTunageAppendFile,		0, wxALL, 2 );
	
	wxStaticBoxSizer *vsTunageURL = new wxStaticBoxSizer( sbTunageURL, wxVERTICAL );
	vsTunageURL->Add( chkTunagePostURL,		0, wxALL, 2 );
	vsTunageURL->Add( hsTunageURL,				0, wxALL | wxEXPAND, 2 );
	
	wxStaticBoxSizer *vsTunageApp = new wxStaticBoxSizer( sbTunageApp, wxVERTICAL );
	vsTunageApp->Add( chkTunageRunApp,			0, wxALL, 2 );
	vsTunageApp->Add( hsTunageCmdLine,			0, wxALL | wxEXPAND, 2 );
	
	wxStaticBoxSizer *vsTunageMisc = new wxStaticBoxSizer( sbTunageMisc, wxVERTICAL );
	vsTunageMisc->Add( chkTunageRunOnStop,		0, wxALL, 2 );
	vsTunageMisc->Add( hsTunageStoppedText,		0, wxALL | wxEXPAND, 2 );

	vsOptions_Tunage = new wxBoxSizer( wxVERTICAL );
	vsOptions_Tunage->Add( vsTunageFile,		0, wxALL | wxEXPAND, 2 );
	vsOptions_Tunage->Add( vsTunageURL,			0, wxALL | wxEXPAND, 2 );
	vsOptions_Tunage->Add( vsTunageApp,			0, wxALL | wxEXPAND, 2 );
	vsOptions_Tunage->Add( vsTunageMisc,		0, wxALL | wxEXPAND, 2 );
	
	//---------------------------//
	//--- Streaming -> Buffer ---//
	//---------------------------//

	wxStaticText *stStreamingBufferSize			= new wxStaticText( this, -1, _("Buffer Size (bytes):"),		wxPoint(0,0), wxSize(-1, -1 ), wxALIGN_LEFT );
	wxStaticText *stStreamingPreBufferPercent	= new wxStaticText( this, -1, _("Prebuffering (%):"),	wxPoint(0,0), wxSize( -1, -1 ), wxALIGN_LEFT );
	wxStaticText *stStreamingReBufferPercent	= new wxStaticText( this, -1, _("Rebuffering (%):"),			wxPoint(0,0), wxSize( -1, -1 ), wxALIGN_LEFT );

	tcStreamingBufferSize		= new wxTextCtrl( this, -1, _(""),	wxPoint(0,0), wxSize( -1, -1 ) );
	tcStreamingPreBufferPercent	= new wxTextCtrl( this, -1, _(""),	wxPoint(0,0), wxSize( -1, -1 ) );
	tcStreamingReBufferPercent	= new wxTextCtrl( this, -1, _(""),	wxPoint(0,0), wxSize( -1, -1 ) );
  
	vsStreaming_Buffer = new wxFlexGridSizer( 3,2,2,2 );

	vsStreaming_Buffer->Add( stStreamingBufferSize );
	vsStreaming_Buffer->Add( tcStreamingBufferSize );
	vsStreaming_Buffer->Add( stStreamingPreBufferPercent );
	vsStreaming_Buffer->Add( tcStreamingPreBufferPercent );
	vsStreaming_Buffer->Add( stStreamingReBufferPercent );
	vsStreaming_Buffer->Add( tcStreamingReBufferPercent );

 	//--------------------------------//
	//--- Streaming -> ProxyServer ---//
	//--------------------------------//
	chkUseProxyServer		= new wxCheckBox	( this, -1,	_("Use Proxy server"), wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	wxStaticText *stProxyServer			= new wxStaticText( this, -1, _("Proxy server adress:"),		wxPoint(0,0), wxSize(-1, -1 ), wxALIGN_LEFT );
	wxStaticText *stProxyServerPort	= new wxStaticText( this, -1, _("Proxy server port:"),	wxPoint(0,0), wxSize( -1, -1 ), wxALIGN_LEFT );
	wxStaticText *stProxyServerUser	= new wxStaticText( this, -1, _("Proxy server user:"),			wxPoint(0,0), wxSize( -1, -1 ), wxALIGN_LEFT );
	wxStaticText *stProxyServerPassword	= new wxStaticText( this, -1, _("Proxy server password:"),			wxPoint(0,0), wxSize( -1, -1 ), wxALIGN_LEFT );

	tcProxyServer			= new wxTextCtrl( this, -1, _(""),	wxPoint(0,0), wxSize( -1, -1 ) );
	tcProxyServerPort		= new wxTextCtrl( this, -1, _(""),	wxPoint(0,0), wxSize( -1, -1 ) );
	tcProxyServerUser		= new wxTextCtrl( this, -1, _(""),	wxPoint(0,0), wxSize( -1, -1 ) );
	tcProxyServerPassword	= new wxTextCtrl( this, -1, _(""),	wxPoint(0,0), wxSize( -1, -1 ) );
	
	wxFlexGridSizer * fsProxySizer		= new wxFlexGridSizer( 4,2,2,2 );

	fsProxySizer->Add(stProxyServer);
	fsProxySizer->Add(tcProxyServer);
	fsProxySizer->Add(stProxyServerPort);
	fsProxySizer->Add(tcProxyServerPort);
	fsProxySizer->Add(stProxyServerUser);
	fsProxySizer->Add(tcProxyServerUser);
	fsProxySizer->Add(stProxyServerPassword);
	fsProxySizer->Add(tcProxyServerPassword);

	vsStreaming_ProxyServer	= new wxBoxSizer( wxVERTICAL );

	vsStreaming_ProxyServer->Add( chkUseProxyServer,		0, wxALL, 2 );
	vsStreaming_ProxyServer->Add( fsProxySizer,		0, wxALL | wxEXPAND, 4 );
	//--------------------------//
	//--- Tagging -> General ---//
	//--------------------------//
	wxStaticText *stActivityTag =	new wxStaticText( this, -1, _("Selection Boxes (artist/album/etc):"), wxPoint( 0, 0 ), wxSize( 50, -1 ), wxALIGN_LEFT );
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
	//--- information ---//
	wxString sInfo = 
		wxString( _("\nAuto Rename syntax:\n\n") ) + 
		wxString( wxString( MUSIK_PATH_SEPARATOR ) + _( " - Directory Separator\n" ) ) +
		wxString( _("%1 - Song Title\n")				) + 
		wxString( _("%2 - Artist Name\n")				) +
		wxString( _("%3 - Album Name\n")				) +
		wxString( _("%4 - Genre\n")						) +
		wxString( _("%5 - Year\n")						) +
		wxString( _("%6 - Track Number\n\n")			) +
		wxString( _("Musik will not delete empty directories!" ) );
	wxStaticText *stInfo = new wxStaticText( this, -1, sInfo, wxPoint( 0, 0 ), wxSize( -1, -1), wxALIGN_LEFT );

	//---------------------------------//
	//--- Tagging -> Auto Tag Sizer ---//
	//---------------------------------//
	vsTagging_Auto = new wxBoxSizer( wxVERTICAL );
	vsTagging_Auto->Add	( hsRename, 0, wxALL | wxEXPAND, 4 );
	vsTagging_Auto->Add ( stInfo, 0, wxADJUST_MINSIZE | wxALL | wxEXPAND, 4 );

	//----------------------//
	//--- System Buttons ---//
	//----------------------//
	btnCancel =	new wxButton( this, MUSIK_PREFERENCES_CANCEL,	_("Cancel"),	wxPoint( 0, 0 ), wxSize( 64, 24 ) );
	btnApply =	new wxButton( this, MUSIK_PREFERENCES_APPLY,	_("Apply"),		wxPoint( 0, 0 ), wxSize( 64, 24 ) );
	btnOK =		new wxButton( this, MUSIK_PREFERENCES_OK,		_("OK"),		wxPoint( 0, 0 ), wxSize( 64, 24 ) );

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
	hsSplitter->Add( vsOptions_Playlist,	5 );
	hsSplitter->Add( vsOptions_Tunage,		5 );
	hsSplitter->Add( vsSound_Crossfader,	5 );
	hsSplitter->Add( vsSound_Driver,		5 );
	hsSplitter->Add( vsTagging_General,		5 );
	hsSplitter->Add( vsTagging_Auto,		5 );
	hsSplitter->Add( vsStreaming_Buffer,	5 );
	hsSplitter->Add( vsStreaming_ProxyServer,	5 );
	
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
void MusikPrefsFrame::OnTreeChange		( wxTreeEvent& WXUNUSED(event) )	{	UpdatePrefsPanel();		}
void MusikPrefsFrame::OnOutputChanged	( wxCommandEvent& WXUNUSED(event) )	{	FindDevices();			}
void MusikPrefsFrame::OnClickOK			( wxCommandEvent& WXUNUSED(event) )	{	Close( false );			}
void MusikPrefsFrame::OnClickApply		( wxCommandEvent& WXUNUSED(event) )	{	SavePrefs();			}
void MusikPrefsFrame::OnClickCancel		( wxCommandEvent& WXUNUSED(event) )	{	Close( true );			}
void MusikPrefsFrame::OnClose			( wxCloseEvent& WXUNUSED(event) )	{	Close( true );			}

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
	chkShowAllSongs->SetValue		( g_Prefs.nShowAllSongs );
	chkBlankSwears->SetValue		( g_Prefs.nBlankSwears );
	chkSortArtistWithoutPrefix->SetValue( g_Prefs.nSortArtistWithoutPrefix );
	chkPlaylistStripes->SetValue	( g_Prefs.nPLStripes );
	chkActivityBoxStripes->SetValue	( g_Prefs.nActStripes );
	chkSourcesBoxStripes->SetValue	( g_Prefs.nSourcesStripes );

	btnPlaylistStripeColour->SetBackgroundColour( StringToColour( g_Prefs.sPLStripeColour ) );
	btnActivityStripeColour->SetBackgroundColour( StringToColour( g_Prefs.sActStripeColour ) );
	btnSourcesStripeColour->SetBackgroundColour( StringToColour( g_Prefs.sSourcesStripeColour ) );

	//-----------------------------//
	//--- options -> selections ---//
	//-----------------------------//
	cmbSelStyle->SetSelection		( g_Prefs.eSelStyle );
	cmbActivityBox1->SetSelection	( g_Prefs.nActBox1 );
	cmbActivityBox2->SetSelection	( g_Prefs.nActBox2 );
	cmbActivityBox3->SetSelection	( g_Prefs.nActBox3 );
	cmbActivityBox4->SetSelection	( g_Prefs.nActBox4 );

	//---------------------------//
	//--- options -> playlist ---//
	//---------------------------//
	chkRatingEnable->SetValue			( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_RATING]			);
	chkTrackEnable->SetValue			( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TRACK]			);
	chkTitleEnable->SetValue			( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TITLE]			);
	chkArtistEnable->SetValue			( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_ARTIST]			);
	chkAlbumEnable->SetValue			( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_ALBUM]			);
	chkYearEnable->SetValue				( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_YEAR]			);
	chkGenreEnable->SetValue			( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_GENRE]			);
	chkTimesPlayedEnable->SetValue		( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TIMES_PLAYED]	);
	chkLastPlayedEnable->SetValue		( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_LAST_PLAYED]		);
	chkTimeEnable->SetValue				( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TIME]			);
	chkBitrateEnable->SetValue			( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_BITRATE]			);
	chkFilenameEnable->SetValue			( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_FILENAME]		);		

	cmbRatingStatic->SetSelection		( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_RATING]			);
	cmbTrackStatic->SetSelection		( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_TRACK]			);
	cmbTitleStatic->SetSelection		( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_TITLE]			);
	cmbArtistStatic->SetSelection		( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_ARTIST]			);
	cmbAlbumStatic->SetSelection		( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_ALBUM]			);
	cmbYearStatic->SetSelection			( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_YEAR]			);
	cmbGenreStatic->SetSelection		( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_GENRE]			);
	cmbTimesPlayedStatic->SetSelection	( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_TIMES_PLAYED]	);
	cmbLastPlayedStatic->SetSelection	( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_LAST_PLAYED]	);
	cmbTimeStatic->SetSelection			( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_TIME]			);
	cmbBitrateStatic->SetSelection		( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_BITRATE]		);
	cmbFilenameStatic->SetSelection		( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_FILENAME]		);

	//-------------------------//
	//--- options -> tunage ---//
	//-------------------------//
	chkTunageWriteFile->SetValue		( g_Prefs.nTunageWriteFile );
	chkTunageAppendFile->SetValue		( g_Prefs.nTunageAppendFile );
	chkTunagePostURL->SetValue			( g_Prefs.nTunagePostURL );
	chkTunageRunApp->SetValue			( g_Prefs.nTunageRunApp );
	chkTunageRunOnStop->SetValue		( g_Prefs.nTunageRunOnStop );
	tcTunageFilename->SetValue			( g_Prefs.sTunageFilename );
	tcTunageFileLine->SetValue			( g_Prefs.sTunageFileLine );
	tcTunageURL->SetValue				( g_Prefs.sTunageURL );
	tcTunageCmdLine->SetValue			( g_Prefs.sTunageCmdLine );
	tcTunageStoppedText->SetValue		( g_Prefs.sTunageStoppedText );

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

	//---------------------------//
	//--- sound -> crossfader ---//
	//---------------------------//

	float		fDuration;
	wxString	sDuration;

	chkCrossfade->SetValue				( g_Prefs.nFadeEnable );
	fDuration =							(float)g_Prefs.nFadeDuration / 1000;
	sDuration.sprintf					( wxT("%.1f"), fDuration );
	tcDuration->SetValue				( sDuration );

	chkCrossfadeSeek->SetValue			( g_Prefs.nFadeSeekEnable );
	fDuration =							(float)g_Prefs.nFadeSeekDuration / 1000;
	sDuration.sprintf					( wxT("%.1f"), fDuration );
	tcSeekDuration->SetValue			( sDuration );

	chkCrossfadePauseResume->SetValue	( g_Prefs.nFadePauseResumeEnable );
	fDuration =							(float)g_Prefs.nFadePauseResumeDuration / 1000;
	sDuration.sprintf					( wxT("%.1f"), fDuration );
	tcPauseResumeDuration->SetValue		( sDuration );

	chkCrossfadeStop->SetValue			( g_Prefs.nFadeStopEnable );
	fDuration =							(float)g_Prefs.nFadeStopDuration / 1000;
	sDuration.sprintf					( wxT("%.1f"), fDuration );
	tcStopDuration->SetValue			( sDuration );

	chkCrossfadeExit->SetValue			( g_Prefs.nFadeExitEnable );
	fDuration =							(float)g_Prefs.nFadeExitDuration / 1000;
	sDuration.sprintf					( wxT("%.1f"), fDuration );
	tcExitDuration->SetValue			( sDuration );

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
	tcMaxChannels->SetValue			( IntTowxString( g_Prefs.nSndMaxChan ) );
	chkUse_MPEGACCURATE_ForMP3VBRFiles->SetValue(g_Prefs.nUse_MPEGACCURATE_ForMP3VBRFiles);
	//---------------------------//
	//--- streaming -> buffer ---//
	//---------------------------//
	tcStreamingBufferSize->SetValue			( IntTowxString( g_Prefs.nStreamingBufferSize ) );
	tcStreamingPreBufferPercent->SetValue	( IntTowxString( g_Prefs.nStreamingPreBufferPercent ) );
	tcStreamingReBufferPercent->SetValue	( IntTowxString( g_Prefs.nStreamingReBufferPercent ) );

	//---------------------------------//
	//--- streaming -> proxy server ---//
	//---------------------------------//
	chkUseProxyServer->SetValue( g_Prefs.bUseProxyServer );
	tcProxyServer->SetValue( g_Prefs.sProxyServer );		
	tcProxyServerPort->SetValue( g_Prefs.sProxyServerPort );		
	tcProxyServerUser->SetValue( g_Prefs.sProxyServerUser );	
	tcProxyServerPassword->SetValue( g_Prefs.sProxyServerPassword );	
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
	hsSplitter->Show( vsOptions_Playlist,	false );
	hsSplitter->Show( vsOptions_Tunage,		false );
	sbTunageFile->Show( false );
	sbTunageURL->Show( false );
	sbTunageApp->Show( false );
	sbTunageMisc->Show( false );
	hsSplitter->Show( vsSound_Crossfader,	false );
	hsSplitter->Show( vsSound_Driver,		false );
	hsSplitter->Show( vsTagging_General,	false );
	hsSplitter->Show( vsTagging_Auto,		false );
	hsSplitter->Show( vsStreaming_Buffer,		false );
 	hsSplitter->Show( vsStreaming_ProxyServer,		false );

}

void MusikPrefsFrame::UpdatePrefsPanel()
{
	if( tcPreferencesTree->GetSelection() == nPlaybackID )
	{
		HidePanels();
		hsSplitter->Show( vsSound_Crossfader, true );
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
	else if ( tcPreferencesTree->GetSelection() == nPlaylistID )
	{
		HidePanels();
		hsSplitter->Show( vsOptions_Playlist, true );
	}
	else if ( tcPreferencesTree->GetSelection() == nTunageID )
	{
		HidePanels();
		hsSplitter->Show( vsOptions_Tunage, true );
		sbTunageFile->Show( true );
		sbTunageURL->Show( true );
		sbTunageApp->Show( true );
		sbTunageMisc->Show( true );
	}
 	else if ( tcPreferencesTree->GetSelection() == nStreamingBufferID )
	{
		HidePanels();
		hsSplitter->Show( vsStreaming_Buffer, true );
	}
 	else if ( tcPreferencesTree->GetSelection() == nStreamingProxyServerID )
	{
		HidePanels();
		hsSplitter->Show( vsStreaming_ProxyServer, true );
	}

	this->Layout();
}

void MusikPrefsFrame::OnTranslateKeys( wxKeyEvent& event )
{
	if ( event.GetKeyCode() == WXK_ESCAPE )
		Close( true );
	
	// we have to event.Skip() here because otherwise the text controls
	// won't allow you to enter certain chars like space, colon, etc.
	event.Skip();
}

void MusikPrefsFrame::SavePrefs()
{
	bool bRestartFMOD		= false;
	bool bShowUnselChange	= false;
	bool bActivityChange	= false;
	bool bPlaymodeChange	= false;
	bool bPlaylistUpdate = false;
	bool bActivityUpdate = false;

	//--------------------------//
	//--- Options -> general ---//
	//--------------------------//
	g_Prefs.nAutoAdd		= chkAutoScan->GetValue();
	g_Prefs.nShowAllSongs	= chkShowAllSongs->GetValue();
	g_Prefs.nBlankSwears	= chkBlankSwears->GetValue();

	if(g_Prefs.nSortArtistWithoutPrefix != chkSortArtistWithoutPrefix->GetValue())
	{
		bPlaylistUpdate = true;
		bActivityUpdate = true;
		g_Prefs.nSortArtistWithoutPrefix = chkSortArtistWithoutPrefix->GetValue();
	}
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
		g_PlaylistBox->Update();

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
	if ( cmbSelStyle->GetSelection() != g_Prefs.eSelStyle )
	{
		g_Prefs.eSelStyle = (EMUSIK_ACTIVITY_SELECTION_TYPE)cmbSelStyle->GetSelection();
		bShowUnselChange = true;
	}

	if ( cmbActivityBox1->GetSelection() != g_Prefs.nActBox1 )
	{
		g_Prefs.nActBox1 = (EMUSIK_ACTIVITY_TYPE)cmbActivityBox1->GetSelection();
		bActivityChange = true;
	}

	if ( cmbActivityBox2->GetSelection() != g_Prefs.nActBox2 )
	{
		g_Prefs.nActBox2 = (EMUSIK_ACTIVITY_TYPE)cmbActivityBox2->GetSelection();
		bActivityChange = true;
	}

	if ( cmbActivityBox3->GetSelection() != g_Prefs.nActBox3 )
	{
		g_Prefs.nActBox3 = (EMUSIK_ACTIVITY_TYPE)cmbActivityBox3->GetSelection();
		bActivityChange = true;
	}

	if ( cmbActivityBox4->GetSelection() != g_Prefs.nActBox4 )
	{
		g_Prefs.nActBox4 = (EMUSIK_ACTIVITY_TYPE)cmbActivityBox4->GetSelection();
		bActivityChange = true;
	}

	//---------------------------//
	//--- options -> playlist ---//
	//---------------------------//
	bool bResetColumns = false;
	//--- enable / disable ---//
	if ( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_RATING] != chkRatingEnable->GetValue() )
	{
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_RATING] = chkRatingEnable->GetValue();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TRACK] != chkTrackEnable->GetValue() )
	{
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TRACK]	= chkTrackEnable->GetValue();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TITLE] != chkTitleEnable->GetValue() )
	{
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TITLE]	= chkTitleEnable->GetValue();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_ARTIST] != chkArtistEnable->GetValue() )
	{
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_ARTIST] = chkArtistEnable->GetValue();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_ALBUM] != chkAlbumEnable->GetValue() )
	{
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_ALBUM]	= chkAlbumEnable->GetValue();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_YEAR] != chkYearEnable->GetValue() )
	{
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_YEAR] = chkYearEnable->GetValue();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_GENRE] != chkGenreEnable->GetValue() )
	{
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_GENRE] = chkGenreEnable->GetValue();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TIMES_PLAYED] != chkTimesPlayedEnable->GetValue() )
	{
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TIMES_PLAYED] = chkTimesPlayedEnable->GetValue();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_LAST_PLAYED] != chkLastPlayedEnable->GetValue() )
	{
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_LAST_PLAYED] = chkLastPlayedEnable->GetValue();	
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TIME] != chkTimeEnable->GetValue() )
	{
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TIME] = chkTimeEnable->GetValue();	
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_BITRATE] != chkBitrateEnable->GetValue() )
	{
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_BITRATE] = chkBitrateEnable->GetValue();	
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_FILENAME] != chkFilenameEnable->GetValue() )
	{
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_FILENAME] = chkFilenameEnable->GetValue();
		bResetColumns = true;
	}

	//--- standard / dynamic ---//
	if ( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_RATING] != cmbRatingStatic->GetSelection() )
	{
		g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_RATING] = cmbRatingStatic->GetSelection();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_TRACK] != cmbTrackStatic->GetSelection() )
	{
		g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_TRACK] = cmbTrackStatic->GetSelection();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_TITLE] != cmbTitleStatic->GetSelection() )
	{
		g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_TITLE] = cmbTitleStatic->GetSelection();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_ARTIST] != cmbArtistStatic->GetSelection() )
	{
		g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_ARTIST] = cmbArtistStatic->GetSelection();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_ALBUM] != cmbAlbumStatic->GetSelection() )
	{
		g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_ALBUM] = cmbAlbumStatic->GetSelection();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_YEAR] != cmbYearStatic->GetSelection() )
	{
		g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_YEAR] = cmbYearStatic->GetSelection();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_GENRE] != cmbGenreStatic->GetSelection() )
	{
		g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_GENRE] = cmbGenreStatic->GetSelection();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_TIMES_PLAYED] != cmbTimesPlayedStatic->GetSelection() )
	{
		g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_TIMES_PLAYED] = cmbTimesPlayedStatic->GetSelection();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_LAST_PLAYED] != cmbLastPlayedStatic->GetSelection() )
	{
		g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_LAST_PLAYED] = cmbLastPlayedStatic->GetSelection();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_TIME] != cmbTimeStatic->GetSelection() )
	{
		g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_TIME] = cmbTimeStatic->GetSelection();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_BITRATE] != cmbBitrateStatic->GetSelection() )
	{
		g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_BITRATE] = cmbBitrateStatic->GetSelection();
		bResetColumns = true;
	}
	if ( g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_FILENAME] != cmbFilenameStatic->GetSelection() )
	{
		g_Prefs.nPlaylistColumnDynamic[PLAYLISTCOLUMN_FILENAME] = cmbFilenameStatic->GetSelection();
		bResetColumns = true;
	}

	//-------------------------//
	//--- options -> tunage ---//
	//-------------------------//
	g_Prefs.nTunageWriteFile	= chkTunageWriteFile->GetValue();
	g_Prefs.nTunageAppendFile	= chkTunageAppendFile->GetValue();
	g_Prefs.nTunagePostURL		= chkTunagePostURL->GetValue();
	g_Prefs.nTunageRunApp		= chkTunageRunApp->GetValue();
	g_Prefs.nTunageRunOnStop	= chkTunageRunOnStop->GetValue();
	g_Prefs.sTunageFilename		= tcTunageFilename->GetValue();
	g_Prefs.sTunageFileLine		= tcTunageFileLine->GetValue();
	g_Prefs.sTunageURL			= tcTunageURL->GetValue();
	g_Prefs.sTunageCmdLine		= tcTunageCmdLine->GetValue();
	g_Prefs.sTunageStoppedText	= tcTunageStoppedText->GetValue();


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

	//---------------------------//
	//--- sound -> crossfader ---//
	//---------------------------//
	g_Prefs.nFadeEnable = chkCrossfade->GetValue();
	g_Prefs.nFadeSeekEnable = chkCrossfadeSeek->GetValue();
	g_Prefs.nFadePauseResumeEnable = chkCrossfadePauseResume->GetValue();
	g_Prefs.nFadeStopEnable = chkCrossfadeStop->GetValue();
	g_Prefs.nFadeExitEnable = chkCrossfadeExit->GetValue();
	
	double fDuration;
	int nDuration;
	
	fDuration = StringToDouble( tcDuration->GetValue() );
	nDuration = ( int )( fDuration * 1000 );
	g_Prefs.nFadeDuration = nDuration;
	
    fDuration = StringToDouble( tcSeekDuration->GetValue() );
	nDuration = ( int )( fDuration * 1000 );
	g_Prefs.nFadeSeekDuration = nDuration;

	fDuration = StringToDouble( tcPauseResumeDuration->GetValue() );
	nDuration = ( int )( fDuration * 1000 );
	g_Prefs.nFadePauseResumeDuration = nDuration;
	
	fDuration = StringToDouble( tcStopDuration->GetValue() );
	nDuration = ( int )( fDuration * 1000 );
	g_Prefs.nFadeStopDuration = nDuration;
	
	fDuration = StringToDouble( tcExitDuration->GetValue() );
	nDuration = ( int )( fDuration * 1000 );
	g_Prefs.nFadeExitDuration = nDuration;
	
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
	if ( wxStringToInt( tcMaxChannels->GetValue() ) != g_Prefs.nSndMaxChan )
	{
		g_Prefs.nSndMaxChan = wxStringToInt( tcMaxChannels->GetValue() );
		bRestartFMOD = true;
	}
	double fLength = StringToDouble( tcBufferLength->GetValue() );
	int nLength = ( int )( fLength * 1000 );
	g_Prefs.nSndBuffer = nLength;

	g_Prefs.nUse_MPEGACCURATE_ForMP3VBRFiles = chkUse_MPEGACCURATE_ForMP3VBRFiles->GetValue();

	//---------------------------//
	//--- streaming -> buffer ---//
	//---------------------------//
	bool bNetBufferSettingChanged = false;
 	if ( wxStringToInt( tcStreamingBufferSize->GetValue( ) ) != g_Prefs.nStreamingBufferSize )
	{
		g_Prefs.nStreamingBufferSize = wxStringToInt( tcStreamingBufferSize->GetValue( ) );
		bNetBufferSettingChanged = true;
	}
 	if ( wxStringToInt( tcStreamingPreBufferPercent->GetValue( ) ) != g_Prefs.nStreamingPreBufferPercent )
	{
		g_Prefs.nStreamingPreBufferPercent = wxStringToInt( tcStreamingPreBufferPercent->GetValue( ) );
		bNetBufferSettingChanged = true;
	}
 	if ( wxStringToInt( tcStreamingReBufferPercent->GetValue( ) ) != g_Prefs.nStreamingReBufferPercent )
	{
		g_Prefs.nStreamingReBufferPercent = wxStringToInt( tcStreamingReBufferPercent->GetValue( )  );
		bNetBufferSettingChanged = true;
	}
	//---------------------------------//
	//--- streaming -> proxy server ---//
	//---------------------------------//
	g_Prefs.bUseProxyServer = chkUseProxyServer->GetValue();
	g_Prefs.sProxyServer = tcProxyServer->GetValue(); 
	g_Prefs.sProxyServer = tcProxyServer->GetValue(  );		
	g_Prefs.sProxyServerPort = tcProxyServerPort->GetValue(  );		
	g_Prefs.sProxyServerUser = tcProxyServerUser->GetValue(  );	
	g_Prefs.sProxyServerPassword = tcProxyServerPassword->GetValue(  );	


	//--- save ---//
	g_Prefs.SavePrefs();

	//--- if we need to restart fmod ---//
	if ( bRestartFMOD )
		g_Player.InitializeFMOD( FMOD_INIT_RESTART );
	else if(bNetBufferSettingChanged)
	{
		g_Player.InitFMOD_NetBuffer();
	}

	g_Player.InitFMOD_ProxyServer();
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

	if ( bResetColumns )
		g_PlaylistBox->PlaylistCtrl().ResetColumns( true, true );
}
