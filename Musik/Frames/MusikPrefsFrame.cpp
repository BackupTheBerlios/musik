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
	: wxFrame( pParent, -1, sTitle, pos, size, wxDEFAULT_FRAME_STYLE|wxRESIZE_BORDER|wxCAPTION | wxTAB_TRAVERSAL | wxFRAME_FLOAT_ON_PARENT | wxFRAME_NO_TASKBAR )
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


	//------------------------------//
	//--- Selection Tree Control ---//
	//------------------------------//
	tcPreferencesTree	= new wxTreeCtrl( this, MUSIK_PREFERENCES_TREE, wxDefaultPosition, wxSize(140,-1), wxTR_HAS_BUTTONS | wxSIMPLE_BORDER );
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
	chkCrossfade			= new wxCheckBox( this, -1, _("Crossfade on new song (seconds)"), 			wxDefaultPosition, wxDefaultSize );
	chkCrossfadeSeek		= new wxCheckBox( this, -1, _("Crossfade on track seek (seconds)"), 		wxDefaultPosition, wxDefaultSize );
	chkCrossfadePauseResume	= new wxCheckBox( this, -1, _("Crossfade on pause or resume (seconds)"),	wxDefaultPosition, wxDefaultSize );
	chkCrossfadeStop		= new wxCheckBox( this, -1, _("Crossfade on stop (seconds)"),				wxDefaultPosition, wxDefaultSize );
	chkCrossfadeExit		= new wxCheckBox( this, -1, _("Crossfade on program exit (seconds)"),		wxDefaultPosition, wxDefaultSize );
	tcDuration 				= new wxTextCtrl( this, -1, _(""), 											wxDefaultPosition, wxDefaultSize );
	tcSeekDuration 			= new wxTextCtrl( this, -1, _(""), 											wxDefaultPosition, wxDefaultSize );
	tcPauseResumeDuration	= new wxTextCtrl( this, -1, _(""), 											wxDefaultPosition, wxDefaultSize );
	tcStopDuration			= new wxTextCtrl( this, -1, _(""), 											wxDefaultPosition, wxDefaultSize );
	tcExitDuration			= new wxTextCtrl( this, -1, _(""), 											wxDefaultPosition, wxDefaultSize );
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
	wxStaticText *stOutputDrv = new wxStaticText	( this, -1, _("Ouput Driver:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	cmbOutputDrv = new wxComboBox ( this, MUSIK_PREFERENCES_OUTPUT_DRV, wxT(""), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
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
	wxStaticText *stSndDevice = new wxStaticText	( this, -1, _("Sound Device:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	cmbSndDevice	= new wxComboBox ( this, MUSIK_PREFERENCES_SND_DEVICE, wxT(""), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );	
	//--- playrate ---//
	wxStaticText *stPlayRate = new wxStaticText( this, -1, _("Playback Rate (hz):"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	cmbPlayRate  = new wxComboBox( this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
	cmbPlayRate->Append ( wxT("48000") );
	cmbPlayRate->Append ( wxT("44100") );
	cmbPlayRate->Append ( wxT("22050") );
	cmbPlayRate->Append ( wxT("11025") );
	cmbPlayRate->Append ( wxT("8000") );
	//--- buffer length ---//
	tcBufferLength = new wxTextCtrl	( this, -1, wxT("") );
	wxStaticText *stBufferLength = new wxStaticText	( this, -1, _(" second buffer length") );
	//--- max channels ---//
	tcMaxChannels = new wxTextCtrl	( this, -1, wxT("") );
	wxStaticText *stMaxChannels = new wxStaticText( this, -1, _("Maximum sound channels:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	
	//Use_MPEGACCURATE_ForMP3VBRFiles
	chkUse_MPEGACCURATE_ForMP3VBRFiles = new wxCheckBox( this, -1, _("Use MPEGACCURATE for Mp3-VBR Files"),		wxDefaultPosition, wxDefaultSize );
	
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

	//-----------------------------------//
	//--- Options -> Selections Sizer ---//
	//-----------------------------------//
	vsOptions_Selections = new wxFlexGridSizer ( 4, 2, 2, 2 );

	//----------------------------//
	//--- Options -> Selection ---//
	//----------------------------//
	const wxString arrSelectionBoxes[] = 
		{
			_("None")  , 
			_("Artist"),
			_("Album") ,
			_("Genre") ,
			_("Year")  
		};

	//--- activity boxes ---//
	for(size_t i = 0; i < WXSIZEOF(cmbActivityBoxes);i++)
	{
		wxStaticText *stActivityBox = new wxStaticText( this, -1, wxString::Format(_("Selection Box %d:"),i + 1), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
		vsOptions_Selections->Add( stActivityBox,	0, wxCENTER | wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
		cmbActivityBoxes[i] = new wxComboBox( this, -1, wxEmptyString, wxDefaultPosition, wxDefaultSize, WXSIZEOF(arrSelectionBoxes), arrSelectionBoxes, wxCB_READONLY );
		vsOptions_Selections->Add( cmbActivityBoxes[i],	1, wxCENTER, 0 );
	}
	//--- selection style ---//
	wxStaticText *stSelStyle = new wxStaticText( this, -1, _("Selection Style:" ), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	cmbSelStyle = new wxComboBox( this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
	cmbSelStyle->Append( _( "Standard" ) );
	cmbSelStyle->Append( _( "Sloppy" ) );
	cmbSelStyle->Append( _( "Highlight") );

	vsOptions_Selections->Add( stSelStyle,		0, wxCENTER | wxRIGHT | wxALIGN_CENTER_VERTICAL, 0 );
	vsOptions_Selections->Add( cmbSelStyle,		1, wxCENTER, 0 );

	//--------------------------//
	//--- Options -> General ---//
	//--------------------------//
	chkAutoPlayOnAppStart	=	new wxCheckBox( this, -1,	_("Automatically play song on startup") );
#ifdef wxHAS_TASK_BAR_ICON
	chkHideOnMinimize		=	new wxCheckBox( this, -1,	_("Hide Window on Mimimize") );
#endif
	chkAutoScan				=	new wxCheckBox( this, -1,	_("Automatically scan for new songs on startup") );
	chkShowAllSongs			=	new wxCheckBox( this, -1,	_("Selecting library shows all songs in playlist") );
	chkBlankSwears			=	new wxCheckBox( this, -1,	_("Censor common swearwords") );
	chkSortArtistWithoutPrefix	=	new wxCheckBox( this, -1,	_("Sort Artist without prefix") );
	chkPlaylistStripes		=	new wxCheckBox( this, -1,	_("Show \"stripes\" in playlist") );
	chkActivityBoxStripes	=	new wxCheckBox( this, -1,	_("Show \"stripes\" in selection boxes") );
	chkSourcesBoxStripes	=	new wxCheckBox( this, -1,	_("Show \"stripes\" in sources box") );

	btnPlaylistStripeColour	=	new wxButton( this, MUSIK_PREFERENCES_PLAYLIST_STRIPE_COLOUR,	_("Set Color") );
	btnActivityStripeColour	=	new wxButton( this, MUSIK_PREFERENCES_ACTIVITY_STRIPE_COLOUR,	_("Set Color") );
	btnSourcesStripeColour	=	new wxButton( this, MUSIK_PREFERENCES_SOURCES_STRIPE_COLOUR,	_("Set Color") );

	//--------------------------------//
	//--- Options -> General Sizer ---//
	//--------------------------------//
	vsOptions_Interface = new wxBoxSizer( wxVERTICAL );
	vsOptions_Interface->Add( chkAutoPlayOnAppStart,	0, wxALL, 4 );
#ifdef wxHAS_TASK_BAR_ICON
	vsOptions_Interface->Add( chkHideOnMinimize,		0, wxALL, 4 );
#endif
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

	//---------------------------------//
	//--- Options -> Playlist Sizer ---//
	//---------------------------------//
	vsOptions_Playlist = new wxFlexGridSizer( 12, 2, 2, 2 );

	//---------------------------//
	//--- Options -> Playlist ---//
	//---------------------------//
	const wxString choicesCMBStatic[] = {  _("Static"),	 _("Dynamic") };

	for(size_t i = 0; i < NPLAYLISTCOLUMNS; i++)
	{
		chkPLColumnEnable[i] = new  wxCheckBox(	this, -1, g_PlaylistColumnLabels[i]+wxT(":"));
		vsOptions_Playlist->Add(chkPLColumnEnable[i],		0, wxALIGN_CENTER_VERTICAL | wxADJUST_MINSIZE );
		cmbPLColumnStatic[i] = new wxComboBox( this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, WXSIZEOF(choicesCMBStatic), choicesCMBStatic, wxCB_READONLY );
		vsOptions_Playlist->Add( cmbPLColumnStatic[i],		0 );
	}

	//-------------------------//
	//--- options -> tunage ---//
	//-------------------------//
	chkTunageWriteFile	= new wxCheckBox( this, -1, _("Enable"),			wxDefaultPosition, wxDefaultSize );
	chkTunageAppendFile = new wxCheckBox( this, -1, _("Append to file"),			wxDefaultPosition, wxDefaultSize );
	chkTunagePostURL	= new wxCheckBox( this, -1, _("Enable"),			wxDefaultPosition, wxDefaultSize );
	chkTunageRunApp		= new wxCheckBox( this, -1, _("Enable"),				wxDefaultPosition, wxDefaultSize );
	chkTunageRunOnStop	= new wxCheckBox( this, -1, _("Update when application stops"),	wxDefaultPosition, wxDefaultSize );

	wxStaticText *stTunageFilename	= new wxStaticText( this, -1, _("Filename:"),		wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	wxStaticText *stTunageFileLine	= new wxStaticText( this, -1, _("Line to write:"),	wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	wxStaticText *stTunageURL		= new wxStaticText( this, -1, _("URL:"),			wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	wxStaticText *stTunageCmdLine	= new wxStaticText( this, -1, _("Command line:"),	wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	wxStaticText *stTunageStoppedText = new wxStaticText( this, -1, _("Stopped Text:"),	wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );

	tcTunageFilename	= new wxTextCtrl( this, -1, _(""),	wxDefaultPosition, wxDefaultSize );
	tcTunageFileLine	= new wxTextCtrl( this, -1, _(""),	wxDefaultPosition, wxDefaultSize );
	tcTunageURL			= new wxTextCtrl( this, -1, _(""),	wxDefaultPosition, wxDefaultSize );
	tcTunageCmdLine		= new wxTextCtrl( this, -1, _(""),	wxDefaultPosition, wxDefaultSize );
	tcTunageStoppedText = new wxTextCtrl( this, -1, _(""),	wxDefaultPosition, wxDefaultSize );

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

	wxStaticText *stStreamingBufferSize			= new wxStaticText( this, -1, _("Buffer Size (bytes):"),		wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	wxStaticText *stStreamingPreBufferPercent	= new wxStaticText( this, -1, _("Prebuffering (%):"),	wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	wxStaticText *stStreamingReBufferPercent	= new wxStaticText( this, -1, _("Rebuffering (%):"),			wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );

	tcStreamingBufferSize		= new wxTextCtrl( this, -1, _(""),	wxDefaultPosition, wxDefaultSize );
	tcStreamingPreBufferPercent	= new wxTextCtrl( this, -1, _(""),	wxDefaultPosition, wxDefaultSize );
	tcStreamingReBufferPercent	= new wxTextCtrl( this, -1, _(""),	wxDefaultPosition, wxDefaultSize );
  
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
	chkUseProxyServer		= new wxCheckBox	( this, -1,	_("Use Proxy server") );
	wxStaticText *stProxyServer			= new wxStaticText( this, -1, _("Proxy server adress:"),		wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	wxStaticText *stProxyServerPort	= new wxStaticText( this, -1, _("Proxy server port:"),	wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	wxStaticText *stProxyServerUser	= new wxStaticText( this, -1, _("Proxy server user:"),			wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	wxStaticText *stProxyServerPassword	= new wxStaticText( this, -1, _("Proxy server password:"),			wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );

	tcProxyServer			= new wxTextCtrl( this, -1, _(""),	wxDefaultPosition, wxDefaultSize );
	tcProxyServerPort		= new wxTextCtrl( this, -1, _(""),	wxDefaultPosition, wxDefaultSize );
	tcProxyServerUser		= new wxTextCtrl( this, -1, _(""),	wxDefaultPosition, wxDefaultSize );
	tcProxyServerPassword	= new wxTextCtrl( this, -1, _(""),	wxDefaultPosition, wxDefaultSize );
	
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
	wxStaticText *stActivityTag =	new wxStaticText( this, -1, _("Selection Boxes (artist/album/etc):"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	chkActivityWrite			=	new wxCheckBox	( this, -1,	_("Write tag to file") );
	chkActivityClear			=	new wxCheckBox	( this, -1,	_("Clear old tag") );
	chkActivityRename			=	new wxCheckBox	( this, -1,	_("Automatically rename file") );
	wxStaticText *stTagDlgTag	=	new wxStaticText( this, -1, _("\nTag Dialog Box:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	chkTagDlgWrite				=	new wxCheckBox	( this, -1,	_("Write tag to file") );
	chkTagDlgClear				=	new wxCheckBox	( this, -1,	_("Clear old tag") );
	chkTagDlgRename				=	new wxCheckBox	( this, -1,	_("Automatically rename file") );

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
	wxStaticText *stRename	= new wxStaticText	( this, -1, _("Rename:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	tcAutoRename			= new wxTextCtrl	( this, -1, wxT("") );
	wxBoxSizer *hsRename	= new wxBoxSizer	( wxHORIZONTAL );
	hsRename->Add ( stRename, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 2 );
	hsRename->Add ( tcAutoRename, 1, wxEXPAND, 0 );
	//--- information ---//
	wxString sInfo = 
		wxString( _("\nAuto Rename syntax:\n\n") ) + 
		wxString(wxFileName::GetPathSeparator() + _( " - Directory Separator\n" ) ) +
		wxString( _("%1 - Song Title\n")				) + 
		wxString( _("%2 - Artist Name\n")				) +
		wxString( _("%3 - Album Name\n")				) +
		wxString( _("%4 - Genre\n")						) +
		wxString( _("%5 - Year\n")						) +
		wxString( _("%6 - Track Number\n\n")			) +
		wxString(MUSIKAPPNAME)							  +	
		wxString( _(" will not delete empty directories!" ) );
	wxStaticText *stInfo = new wxStaticText( this, -1, sInfo, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );

	//---------------------------------//
	//--- Tagging -> Auto Tag Sizer ---//
	//---------------------------------//
	vsTagging_Auto = new wxBoxSizer( wxVERTICAL );
	vsTagging_Auto->Add	( hsRename, 0, wxALL | wxEXPAND, 4 );
	vsTagging_Auto->Add ( stInfo, 0, wxADJUST_MINSIZE | wxALL | wxEXPAND, 4 );

	//----------------------//
	//--- System Buttons ---//
	//----------------------//
	btnCancel =	new wxButton( this, MUSIK_PREFERENCES_CANCEL,	_("Cancel"),	wxDefaultPosition, wxDefaultSize );
	btnApply =	new wxButton( this, MUSIK_PREFERENCES_APPLY,	_("Apply"),		wxDefaultPosition, wxDefaultSize );
	btnOK =		new wxButton( this, MUSIK_PREFERENCES_OK,		_("OK"),		wxDefaultPosition, wxDefaultSize );

	//----------------------------//
	//--- System Buttons Sizer ---//
	//----------------------------//
	hsSysButtons = new wxBoxSizer( wxHORIZONTAL );
	hsSysButtons->Add( btnCancel,	0, wxALIGN_LEFT		);
	hsSysButtons->Add( -1,-1,	1, wxEXPAND			);
	hsSysButtons->Add( btnApply,	0, wxALIGN_RIGHT	);
	hsSysButtons->Add( btnOK,		0, wxALIGN_RIGHT | wxLEFT, 4);

	//-------------------------//
	//--- Hide / Show Sizer ---//
	//-------------------------//
	hsSplitter = new wxBoxSizer( wxHORIZONTAL );
	hsSplitter->Add( tcPreferencesTree,		0, wxEXPAND | wxRIGHT, 8 );
	hsSplitter->Add( vsOptions_Selections,	1 );
	hsSplitter->Add( vsOptions_Interface,	1 );
	hsSplitter->Add( vsOptions_Playlist,	1 );
	hsSplitter->Add( vsOptions_Tunage,		1 );
	hsSplitter->Add( vsSound_Crossfader,	1 );
	hsSplitter->Add( vsSound_Driver,		1 );
	hsSplitter->Add( vsTagging_General,		1 );
	hsSplitter->Add( vsTagging_Auto,		1 );
	hsSplitter->Add( vsStreaming_Buffer,	1 );
	hsSplitter->Add( vsStreaming_ProxyServer,	1 );
	
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
	
	Centre();
	Layout();
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
	chkAutoPlayOnAppStart->SetValue	( wxGetApp().Prefs.bAutoPlayOnAppStart	);
#ifdef wxHAS_TASK_BAR_ICON
	chkHideOnMinimize->SetValue		( wxGetApp().Prefs.bHideOnMinimize	);
#endif
	chkAutoScan->SetValue			( wxGetApp().Prefs.bAutoAdd	);
	chkShowAllSongs->SetValue		( wxGetApp().Prefs.bShowAllSongs );
	chkBlankSwears->SetValue		( wxGetApp().Prefs.bBlankSwears );
	chkSortArtistWithoutPrefix->SetValue( wxGetApp().Prefs.bSortArtistWithoutPrefix );
	chkPlaylistStripes->SetValue	( wxGetApp().Prefs.bPLStripes );
	chkActivityBoxStripes->SetValue	( wxGetApp().Prefs.bActStripes );
	chkSourcesBoxStripes->SetValue	( wxGetApp().Prefs.bSourcesStripes );

	btnPlaylistStripeColour->SetBackgroundColour( StringToColour( wxGetApp().Prefs.sPLStripeColour ) );
	btnActivityStripeColour->SetBackgroundColour( StringToColour( wxGetApp().Prefs.sActStripeColour ) );
	btnSourcesStripeColour->SetBackgroundColour( StringToColour( wxGetApp().Prefs.sSourcesStripeColour ) );

	//-----------------------------//
	//--- options -> selections ---//
	//-----------------------------//
	cmbSelStyle->SetSelection		( wxGetApp().Prefs.eSelStyle.val );
	for(size_t i = 0; i < WXSIZEOF(cmbActivityBoxes);i++)
		cmbActivityBoxes[i]->SetSelection	( wxGetApp().Prefs.nActBoxType[i] );

	//---------------------------//
	//--- options -> playlist ---//
	//---------------------------//
	for(size_t i = 0 ;i < NPLAYLISTCOLUMNS; i ++)
	{
		chkPLColumnEnable[i]->SetValue			( wxGetApp().Prefs.bPlaylistColumnEnable[i]);
		cmbPLColumnStatic[i]->SetSelection		( wxGetApp().Prefs.bPlaylistColumnDynamic[PLAYLISTCOLUMN_RATING]			);
	}


	//-------------------------//
	//--- options -> tunage ---//
	//-------------------------//
	chkTunageWriteFile->SetValue		( wxGetApp().Prefs.bTunageWriteFile );
	chkTunageAppendFile->SetValue		( wxGetApp().Prefs.bTunageAppendFile );
	chkTunagePostURL->SetValue			( wxGetApp().Prefs.bTunagePostURL );
	chkTunageRunApp->SetValue			( wxGetApp().Prefs.bTunageRunApp );
	chkTunageRunOnStop->SetValue		( wxGetApp().Prefs.bTunageRunOnStop );
	tcTunageFilename->SetValue			( wxGetApp().Prefs.sTunageFilename );
	tcTunageFileLine->SetValue			( wxGetApp().Prefs.sTunageFileLine );
	tcTunageURL->SetValue				( wxGetApp().Prefs.sTunageURL );
	tcTunageCmdLine->SetValue			( wxGetApp().Prefs.sTunageCmdLine );
	tcTunageStoppedText->SetValue		( wxGetApp().Prefs.sTunageStoppedText );

	//--------------------------//
	//--- tagging -> general ---//
	//--------------------------//
	chkActivityWrite->SetValue		( wxGetApp().Prefs.bActBoxWrite	);
	chkActivityClear->SetValue		( wxGetApp().Prefs.bActBoxClear	);
	chkActivityRename->SetValue		( wxGetApp().Prefs.bActBoxRename );
	chkTagDlgWrite->SetValue		( wxGetApp().Prefs.bTagDlgWrite	);
	chkTagDlgClear->SetValue		( wxGetApp().Prefs.bTagDlgClear	);
	chkTagDlgRename->SetValue		( wxGetApp().Prefs.bTagDlgRename );

	//---------------------------//
	//--- tagging -> auto tag ---//
	//---------------------------//
	tcAutoRename->SetValue			( wxGetApp().Prefs.sAutoRename	);

	//---------------------------//
	//--- sound -> crossfader ---//
	//---------------------------//

	float		fDuration;
	wxString	sDuration;

	chkCrossfade->SetValue				( wxGetApp().Prefs.bFadeEnable );
	fDuration =							(float)wxGetApp().Prefs.nFadeDuration / 1000;
	sDuration.sprintf					( wxT("%.1f"), fDuration );
	tcDuration->SetValue				( sDuration );

	chkCrossfadeSeek->SetValue			( wxGetApp().Prefs.bFadeSeekEnable );
	fDuration =							(float)wxGetApp().Prefs.nFadeSeekDuration / 1000;
	sDuration.sprintf					( wxT("%.1f"), fDuration );
	tcSeekDuration->SetValue			( sDuration );

	chkCrossfadePauseResume->SetValue	( wxGetApp().Prefs.bFadePauseResumeEnable );
	fDuration =							(float)wxGetApp().Prefs.nFadePauseResumeDuration / 1000;
	sDuration.sprintf					( wxT("%.1f"), fDuration );
	tcPauseResumeDuration->SetValue		( sDuration );

	chkCrossfadeStop->SetValue			( wxGetApp().Prefs.bFadeStopEnable );
	fDuration =							(float)wxGetApp().Prefs.nFadeStopDuration / 1000;
	sDuration.sprintf					( wxT("%.1f"), fDuration );
	tcStopDuration->SetValue			( sDuration );

	chkCrossfadeExit->SetValue			( wxGetApp().Prefs.bFadeExitEnable );
	fDuration =							(float)wxGetApp().Prefs.nFadeExitDuration / 1000;
	sDuration.sprintf					( wxT("%.1f"), fDuration );
	tcExitDuration->SetValue			( sDuration );

	//-----------------------//
	//--- sound -> driver ---//
	//-----------------------//
	FindDevices();
	wxString sSndRate, sLength;
	float fLength;
	cmbOutputDrv->SetSelection		( wxGetApp().Prefs.nSndOutput );
    cmbSndDevice->SetSelection		( wxGetApp().Prefs.nSndDevice );
	sSndRate.sprintf				( wxT("%d"), wxGetApp().Prefs.nSndRate.val );
	fLength =						(float)wxGetApp().Prefs.nSndBuffer / 1000;
	sLength.sprintf					( wxT("%.1f"), fLength );
	tcBufferLength->SetValue		( sLength );
	cmbPlayRate->SetSelection		( cmbPlayRate->FindString ( sSndRate ) );
	tcMaxChannels->SetValue			( IntTowxString( wxGetApp().Prefs.nSndMaxChan ) );
	chkUse_MPEGACCURATE_ForMP3VBRFiles->SetValue(wxGetApp().Prefs.bUse_MPEGACCURATE_ForMP3VBRFiles);
	//---------------------------//
	//--- streaming -> buffer ---//
	//---------------------------//
	tcStreamingBufferSize->SetValue			( IntTowxString( wxGetApp().Prefs.nStreamingBufferSize ) );
	tcStreamingPreBufferPercent->SetValue	( IntTowxString( wxGetApp().Prefs.nStreamingPreBufferPercent ) );
	tcStreamingReBufferPercent->SetValue	( IntTowxString( wxGetApp().Prefs.nStreamingReBufferPercent ) );

	//---------------------------------//
	//--- streaming -> proxy server ---//
	//---------------------------------//
	chkUseProxyServer->SetValue( wxGetApp().Prefs.bUseProxyServer );
	tcProxyServer->SetValue( wxGetApp().Prefs.sProxyServer );		
	tcProxyServerPort->SetValue( wxGetApp().Prefs.sProxyServerPort );		
	tcProxyServerUser->SetValue( wxGetApp().Prefs.sProxyServerUser );	
	tcProxyServerPassword->SetValue( wxGetApp().Prefs.sProxyServerPassword );	
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
	else
		event.Skip();
}

void MusikPrefsFrame::SavePrefs()
{
	bool bRestartFMOD		= false;
	bool bShowUnselChange	= false;
	bool bActivityChange	= false;
	bool bPlaylistUpdate = false;
	bool bActivityUpdate = false;

	//--------------------------//
	//--- Options -> general ---//
	//--------------------------//

	wxGetApp().Prefs.bAutoAdd		= chkAutoScan->GetValue();
	wxGetApp().Prefs.bAutoPlayOnAppStart	= chkAutoPlayOnAppStart->GetValue();
#ifdef wxHAS_TASK_BAR_ICON
	wxGetApp().Prefs.bHideOnMinimize	= chkHideOnMinimize->GetValue();
#endif
	wxGetApp().Prefs.bShowAllSongs	= chkShowAllSongs->GetValue();
	wxGetApp().Prefs.bBlankSwears	= chkBlankSwears->GetValue();

	if(wxGetApp().Prefs.bSortArtistWithoutPrefix != chkSortArtistWithoutPrefix->GetValue())
	{
		bPlaylistUpdate = true;
		bActivityUpdate = true;
		wxGetApp().Prefs.bSortArtistWithoutPrefix = chkSortArtistWithoutPrefix->GetValue();
	}
	if ( chkPlaylistStripes->GetValue() != wxGetApp().Prefs.bPLStripes )
	{
		wxGetApp().Prefs.bPLStripes = chkPlaylistStripes->GetValue();
		bPlaylistUpdate = true;
	}
	if ( ColourToString( btnPlaylistStripeColour->GetBackgroundColour() ) != wxGetApp().Prefs.sPLStripeColour )
	{
		wxGetApp().Prefs.sPLStripeColour = ColourToString( btnPlaylistStripeColour->GetBackgroundColour() );
		bPlaylistUpdate = true;
	}
	if ( bPlaylistUpdate )
		g_PlaylistBox->Update();

	if ( chkActivityBoxStripes->GetValue() != wxGetApp().Prefs.bActStripes )
	{
		wxGetApp().Prefs.bActStripes = chkActivityBoxStripes->GetValue();
		bActivityUpdate = true;
	}
	if ( ColourToString( btnActivityStripeColour->GetBackgroundColour() ) != wxGetApp().Prefs.sActStripeColour )
	{
		wxGetApp().Prefs.sActStripeColour = ColourToString( btnActivityStripeColour->GetBackgroundColour() );
		bActivityUpdate = true;
	}
	if ( bActivityUpdate )
		g_ActivityAreaCtrl->ResetAllContents();

	bool bSourcesUpdate = false;
	if ( chkSourcesBoxStripes->GetValue() != wxGetApp().Prefs.bSourcesStripes )
	{
		wxGetApp().Prefs.bSourcesStripes = chkSourcesBoxStripes->GetValue();
		bSourcesUpdate = true;
	}
	if ( ColourToString( btnSourcesStripeColour->GetBackgroundColour() ) != wxGetApp().Prefs.sSourcesStripeColour )
	{
		wxGetApp().Prefs.sSourcesStripeColour = ColourToString( btnSourcesStripeColour->GetBackgroundColour() );
		bSourcesUpdate = true;
	}
	if ( bSourcesUpdate )
		g_SourcesCtrl->Update();

	//-----------------------------//
    //--- Options -> selections ---//
	//-----------------------------//
	if ( (EMUSIK_ACTIVITY_SELECTION_TYPE)cmbSelStyle->GetSelection() != wxGetApp().Prefs.eSelStyle )
	{
		wxGetApp().Prefs.eSelStyle = (EMUSIK_ACTIVITY_SELECTION_TYPE)cmbSelStyle->GetSelection();
		bShowUnselChange = true;
	}
	for(size_t i = 0; i < WXSIZEOF(cmbActivityBoxes);i++)
	{
		if ( cmbActivityBoxes[i]->GetSelection() != wxGetApp().Prefs.nActBoxType[i] )
		{
			wxGetApp().Prefs.nActBoxType[i] = (EMUSIK_ACTIVITY_TYPE)cmbActivityBoxes[i]->GetSelection();
			bActivityChange = true;
		}
	}


	//---------------------------//
	//--- options -> playlist ---//
	//---------------------------//
	bool bResetColumns = false;
	for(size_t i = 0 ;i < NPLAYLISTCOLUMNS; i ++)
	{
		//--- enable / disable ---//
		if ( wxGetApp().Prefs.bPlaylistColumnEnable[i] != chkPLColumnEnable[i]->GetValue() )
		{
			wxGetApp().Prefs.bPlaylistColumnEnable[i] = chkPLColumnEnable[i]->GetValue();
			bResetColumns = true;
		}
		//--- standard / dynamic ---//
		if ( wxGetApp().Prefs.bPlaylistColumnDynamic[i] != (cmbPLColumnStatic[i]->GetSelection() ? true : false) )
		{
			wxGetApp().Prefs.bPlaylistColumnDynamic[i] = cmbPLColumnStatic[i]->GetSelection()? true : false;
			bResetColumns = true;
		}
	}


	//-------------------------//
	//--- options -> tunage ---//
	//-------------------------//
	wxGetApp().Prefs.bTunageWriteFile	= chkTunageWriteFile->GetValue();
	wxGetApp().Prefs.bTunageAppendFile	= chkTunageAppendFile->GetValue();
	wxGetApp().Prefs.bTunagePostURL		= chkTunagePostURL->GetValue();
	wxGetApp().Prefs.bTunageRunApp		= chkTunageRunApp->GetValue();
	wxGetApp().Prefs.bTunageRunOnStop	= chkTunageRunOnStop->GetValue();
	wxGetApp().Prefs.sTunageFilename		= tcTunageFilename->GetValue();
	wxGetApp().Prefs.sTunageFileLine		= tcTunageFileLine->GetValue();
	wxGetApp().Prefs.sTunageURL			= tcTunageURL->GetValue();
	wxGetApp().Prefs.sTunageCmdLine		= tcTunageCmdLine->GetValue();
	wxGetApp().Prefs.sTunageStoppedText	= tcTunageStoppedText->GetValue();


	//--------------------------//
	//--- tagging -> general ---//
	//--------------------------//
	wxGetApp().Prefs.bActBoxWrite	= chkActivityWrite->GetValue();
	wxGetApp().Prefs.bActBoxClear	= chkActivityClear->GetValue();
	wxGetApp().Prefs.bActBoxRename	= chkActivityRename->GetValue();
	wxGetApp().Prefs.bTagDlgWrite	= chkTagDlgWrite->GetValue();
	wxGetApp().Prefs.bTagDlgClear	= chkTagDlgClear->GetValue();
	wxGetApp().Prefs.bTagDlgRename	= chkTagDlgRename->GetValue();

	//-----------------------//
	//--- tagging -> auto ---//
	//-----------------------//
	wxGetApp().Prefs.sAutoRename = tcAutoRename->GetValue();

	//---------------------------//
	//--- sound -> crossfader ---//
	//---------------------------//
	wxGetApp().Prefs.bFadeEnable = chkCrossfade->GetValue();
	wxGetApp().Prefs.bFadeSeekEnable = chkCrossfadeSeek->GetValue();
	wxGetApp().Prefs.bFadePauseResumeEnable = chkCrossfadePauseResume->GetValue();
	wxGetApp().Prefs.bFadeStopEnable = chkCrossfadeStop->GetValue();
	wxGetApp().Prefs.bFadeExitEnable = chkCrossfadeExit->GetValue();
	
	double fDuration;
	int nDuration;
	
	fDuration = StringToDouble( tcDuration->GetValue() );
	nDuration = ( int )( fDuration * 1000 );
	wxGetApp().Prefs.nFadeDuration = nDuration;
	
    fDuration = StringToDouble( tcSeekDuration->GetValue() );
	nDuration = ( int )( fDuration * 1000 );
	wxGetApp().Prefs.nFadeSeekDuration = nDuration;

	fDuration = StringToDouble( tcPauseResumeDuration->GetValue() );
	nDuration = ( int )( fDuration * 1000 );
	wxGetApp().Prefs.nFadePauseResumeDuration = nDuration;
	
	fDuration = StringToDouble( tcStopDuration->GetValue() );
	nDuration = ( int )( fDuration * 1000 );
	wxGetApp().Prefs.nFadeStopDuration = nDuration;
	
	fDuration = StringToDouble( tcExitDuration->GetValue() );
	nDuration = ( int )( fDuration * 1000 );
	wxGetApp().Prefs.nFadeExitDuration = nDuration;
	
	//-----------------------//
	//--- sound -> driver ---//
	//-----------------------//
	if ( cmbOutputDrv->GetSelection() != wxGetApp().Prefs.nSndOutput )
	{
		wxGetApp().Prefs.nSndOutput = cmbOutputDrv->GetSelection();
		bRestartFMOD = true;
	}
	if ( cmbSndDevice->GetSelection() != wxGetApp().Prefs.nSndDevice )
	{
		wxGetApp().Prefs.nSndDevice = cmbSndDevice->GetSelection();
		bRestartFMOD = true;
	}
	if ( wxStringToInt( cmbPlayRate->GetString( cmbPlayRate->GetSelection() ) ) != wxGetApp().Prefs.nSndRate )
	{
		int nRate = wxStringToInt( cmbPlayRate->GetString( cmbPlayRate->GetSelection() ) );
		wxGetApp().Prefs.nSndRate = nRate;
		bRestartFMOD = true;
	}
	if ( wxStringToInt( tcMaxChannels->GetValue() ) != wxGetApp().Prefs.nSndMaxChan )
	{
		wxGetApp().Prefs.nSndMaxChan = wxStringToInt( tcMaxChannels->GetValue() );
		bRestartFMOD = true;
	}
	double fLength = StringToDouble( tcBufferLength->GetValue() );
	int nLength = ( int )( fLength * 1000 );
	wxGetApp().Prefs.nSndBuffer = nLength;

	wxGetApp().Prefs.bUse_MPEGACCURATE_ForMP3VBRFiles = chkUse_MPEGACCURATE_ForMP3VBRFiles->GetValue();

	//---------------------------//
	//--- streaming -> buffer ---//
	//---------------------------//
	bool bNetBufferSettingChanged = false;
 	if ( wxStringToInt( tcStreamingBufferSize->GetValue( ) ) != wxGetApp().Prefs.nStreamingBufferSize )
	{
		wxGetApp().Prefs.nStreamingBufferSize = wxStringToInt( tcStreamingBufferSize->GetValue( ) );
		bNetBufferSettingChanged = true;
	}
 	if ( wxStringToInt( tcStreamingPreBufferPercent->GetValue( ) ) != wxGetApp().Prefs.nStreamingPreBufferPercent )
	{
		wxGetApp().Prefs.nStreamingPreBufferPercent = wxStringToInt( tcStreamingPreBufferPercent->GetValue( ) );
		bNetBufferSettingChanged = true;
	}
 	if ( wxStringToInt( tcStreamingReBufferPercent->GetValue( ) ) != wxGetApp().Prefs.nStreamingReBufferPercent )
	{
		wxGetApp().Prefs.nStreamingReBufferPercent = wxStringToInt( tcStreamingReBufferPercent->GetValue( )  );
		bNetBufferSettingChanged = true;
	}
	//---------------------------------//
	//--- streaming -> proxy server ---//
	//---------------------------------//
	wxGetApp().Prefs.bUseProxyServer = chkUseProxyServer->GetValue();
	wxGetApp().Prefs.sProxyServer = tcProxyServer->GetValue(); 
	wxGetApp().Prefs.sProxyServer = tcProxyServer->GetValue(  );		
	wxGetApp().Prefs.sProxyServerPort = tcProxyServerPort->GetValue(  );		
	wxGetApp().Prefs.sProxyServerUser = tcProxyServerUser->GetValue(  );	
	wxGetApp().Prefs.sProxyServerPassword = tcProxyServerPassword->GetValue(  );	


	//--- if we need to restart fmod ---//
	if ( bRestartFMOD )
		wxGetApp().Player.InitializeFMOD( FMOD_INIT_RESTART );
	else if(bNetBufferSettingChanged)
	{
		wxGetApp().Player.InitFMOD_NetBuffer();
	}

	wxGetApp().Player.InitFMOD_ProxyServer();

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
