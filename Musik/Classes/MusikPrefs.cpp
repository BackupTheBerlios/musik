/*
 *  MusikPrefs.cpp
 *
 *  Class to handle saving / loading
 *  of all preferences.
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "MusikPrefs.h"

//--- wx config stuff ---//
#include <wx/config.h> 
#include <wx/confbase.h>
#include <wx/fileconf.h> 

#include "../MusikGlobals.h"
#include "../MusikUtils.h"

#if defined __WXMSW__
	#define CONFIG_NAME wxT( ".Musik\\Musik.ini" )
#elif defined __WXGTK__
	#define CONFIG_NAME wxT( "MusikPrefs" )
#endif

void CMusikPrefs::LoadPrefs()
{
	wxFileConfig *config;
	config = new wxFileConfig( CONFIG_NAME );
	config->Read( wxT( "SelectionStyle" ),							&nSelStyle,				0						);
	config->Read( wxT( "OutputDriver" ),							&nSndOutput,			0						);
	config->Read( wxT( "SoundDevice" ),								&nSndDevice,			0						);
	config->Read( wxT( "PlaybackRate" ),							&nSndRate,				44100					);
	config->Read( wxT( "SoundVolume" ),								&nSndVolume,			200						);
	config->Read( wxT( "UseEQ" ),									&nUseEQ,				0						);
	config->Read( wxT( "SoundBufferMilliseconds" ),					&nSndBuffer,			1000					);
	config->Read( wxT( "ActivityBox1" ),							&nActBox1,				1						);
	config->Read( wxT( "ActivityBox2" ),							&nActBox2,				2						);
	config->Read( wxT( "ActivityBox3" ),							&nActBox3,				0						);
	config->Read( wxT( "ActivityBox4" ),							&nActBox4,				0						);
	config->Read( wxT( "Repeat" ),									&nRepeat,				1						);
	config->Read( wxT( "Shuffle" ),									&nShuffle,				0						);
	config->Read( wxT( "CrossfaderEnabled" ),						&nFadeEnable,			1						);
	config->Read( wxT( "CrossfaderDurationMilliseconds" ),			&nFadeDuration,			2000					);
	config->Read( wxT( "CrossfaderSeekEnabled" ),					&nFadeSeekEnable,		1						);
	config->Read( wxT( "CrossfaderSeekDurationMilliseconds" ),		&nFadeSeekDuration,		200						);
	config->Read( wxT( "ShowPlaylistInfo" ),						&nShowPLInfo,			1						);
	config->Read( wxT( "ShowSources" ),								&nShowSources,			1						);
	config->Read( wxT( "ShowSourcesIcons" ),						&nShowSourcesIcons,		1						);
	config->Read( wxT( "ShowActivities" ),							&nShowActivities,		1						);
	config->Read( wxT( "ShowRating" ),								&nShowRatings,			1						);
	config->Read( wxT( "StayOnTop" ),								&nStayOnTop,			0						);
	config->Read( wxT( "AutomaticallyScanDelta" ),					&nAutoDelta,			1						);
	config->Read( wxT( "ActivityBoxWrite" ),						&nActBoxWrite,			0						);
	config->Read( wxT( "ActivityBoxClear" ),						&nActBoxClear,			0						);
	config->Read( wxT( "ActivityBoxRename" ),						&nActBoxRename,			0						);
	config->Read( wxT( "TagDialogWrite" ),							&nTagDlgWrite,			0						);
	config->Read( wxT( "TagDialogClear" ),							&nTagDlgClear,			0						);
	config->Read( wxT( "TagDialogRename" ),							&nTagDlgRename,			0						);
	config->Read( wxT( "AutoRenameMask" ),							&sAutoRename,			wxT( "%6 - %2 - %1" )	);
	config->Read( wxT( "AutoTagMask" ),								&sAutoTag,				wxT( "%1 - %2" )		);
	config->Read( wxT( "FirstRun" ),								&nFirstRun,				1						);
	config->Read( wxT( "AddFilesonStartup" ),						&nAutoAdd,				1						);
	config->Read( wxT( "SelectingLibraryDisplaysSongs" ),			&nShowAllSongs,			1						);
	config->Read( wxT( "PlaylistStripes" ),							&nPLStripes,			1						);
	config->Read( wxT( "ActivityStripes" ),							&nActStripes,			0						);
	config->Read( wxT( "SourcesStripes" ),							&nSourcesStripes,		0						);
	config->Read( wxT( "PlaylistStripeColour" ),					&sPLStripeColour,		wxT("244, 244, 244")	);
	config->Read( wxT( "ActivityStripeColour" ),					&sActStripeColour,		wxT("244, 244, 244")	);
	config->Read( wxT( "SourcesStripeColour" ),						&sSourcesStripeColour,	wxT("244, 244, 244")	);
	config->Read( wxT( "BlankSwears" ),								&nBlankSwears,			0						);
	config->Read( wxT( "EnableWebserver" ),							&nWebServerEnable,		0						);
	config->Read( wxT( "Webserverport" ),							&nWebServerPort,		6395					);
	config->Read( wxT( "FramePlacement" ),							&sFramePlacement,		wxT("0,0,800,600,0,0")	);

	//--- playlist columns ---//
	for( int i = 0;i < NPLAYLISTCOLUMNS; i++ )
		config->Read( wxString::Format(wxT("PlaylistColumn%dSize"),i),&nPlaylistColumnSize[i],50);

	g_FX.LoadBands( config );


	//-------------------------------------------------//
	//--- if we're in windows there is a problem	---//
	//--- writing a backslash "\" character to		---//
	//--- file, then later reading it. so it is		---//
	//--- stored as a forward slash "/"				---//
	//-------------------------------------------------//
	#ifdef __WXMSW__
		sAutoRename.Replace( wxT( "/" ), wxT( "\\" ), true );
		sAutoTag.Replace( wxT( "/" ), wxT( "\\" ), true );
	#endif

	//--- apply unweildy defaults ---//
	if ( sPLStripeColour.IsEmpty()		) sPLStripeColour		= wxT("244, 244, 244");
	if ( sActStripeColour.IsEmpty()		) sActStripeColour		= wxT("244, 244, 244");
	if ( sSourcesStripeColour.IsEmpty()	) sSourcesStripeColour	= wxT("244, 244, 244");

	delete config;
	SavePrefs();
}

void CMusikPrefs::SavePrefs()
{
	//-------------------------------------------------//
	//--- if we're in windows there is a problem	---//
	//--- writing a backslash "\" character to		---//
	//--- file, then later reading it. so it is		---//
	//--- stored as a forward slash "/"				---//
	//-------------------------------------------------//
	#ifdef __WXMSW__
		sAutoRename.Replace( wxT( "\\" ), wxT( "/" ), true );
		sAutoTag.Replace( wxT( "\\" ), wxT( "/" ), true );
	#endif

	wxFileConfig *config;
	config = new wxFileConfig( CONFIG_NAME );
	config->Write( wxT( "SelectionStyle" ),							nSelStyle				);
	config->Write( wxT( "OutputDriver" ),							nSndOutput				);
	config->Write( wxT( "SoundDevice" ),							nSndDevice				);
	config->Write( wxT( "PlaybackRate" ),							nSndRate				);
	config->Write( wxT( "SoundVolume" ),							nSndVolume				);
	config->Write( wxT( "UseEQ" ),									nUseEQ					);
	config->Write( wxT( "SoundBufferMilliseconds" ),				nSndBuffer				);
	config->Write( wxT( "ActivityBox1" ),							nActBox1				);
	config->Write( wxT( "ActivityBox2" ),							nActBox2				);
	config->Write( wxT( "ActivityBox3" ),							nActBox3				);
	config->Write( wxT( "ActivityBox4" ),							nActBox4				);
	config->Write( wxT( "Repeat" ),									nRepeat					);
	config->Write( wxT( "Shuffle" ),								nShuffle				);
	config->Write( wxT( "CrossfaderEnabled" ),						nFadeEnable				);
	config->Write( wxT( "CrossfaderDurationMilliseconds" ),			nFadeDuration			);
	config->Write( wxT( "CrossfaderSeekEnabled" ),					nFadeSeekEnable			);
	config->Write( wxT( "CrossfaderSeekDurationMilliseconds" ),		nFadeSeekDuration		);
	config->Write( wxT( "ShowPlaylistInfo" ),						nShowPLInfo				);
	config->Write( wxT( "ShowSources" ),							nShowSources			);
	config->Write( wxT( "ShowSourcesIcons" ),						nShowSourcesIcons		);
	config->Write( wxT( "ShowActivities" ),							nShowActivities			);
	config->Write( wxT( "ShowRating" ),								nShowRatings			);
	config->Write( wxT( "StayOnTop" ),								nStayOnTop				);
	config->Write( wxT( "AutomaticallyScanDelta" ),					nAutoDelta				);
	config->Write( wxT( "ActivityBoxWrite" ),						nActBoxWrite			);
	config->Write( wxT( "ActivityBoxClear" ),						nActBoxClear			);
	config->Write( wxT( "ActivityBoxRename" ),						nActBoxRename			);
	config->Write( wxT( "TagDialogWrite" ),							nTagDlgWrite			);
	config->Write( wxT( "TagDialogClear" ),							nTagDlgClear			);
	config->Write( wxT( "TagDialogRename" ),						nTagDlgRename			);
	config->Write( wxT( "AutoRenameMask" ),							sAutoRename				);
	config->Write( wxT( "AutoTagMask" ),							sAutoTag				);
	config->Write( wxT( "FirstRun" ),								nFirstRun				);
	config->Write( wxT( "AddFilesonStartup" ),						nAutoAdd				);
	config->Write( wxT( "SelectingLibraryDisplaysSongs" ),			nShowAllSongs			);
	config->Write( wxT( "PlaylistStripes" ),						nPLStripes				);
	config->Write( wxT( "ActivityStripes" ),						nActStripes				);
	config->Write( wxT( "SourcesStripes" ),							nSourcesStripes			);
	config->Write( wxT( "PlaylistStripeColour" ),					sPLStripeColour			);
	config->Write( wxT( "ActivityStripeColour" ),					sActStripeColour		);
	config->Write( wxT( "SourcesStripeColour" ),					sSourcesStripeColour	);
	config->Write( wxT( "BlankSwears" ),							nBlankSwears			);
	config->Write( wxT( "EnableWebserver" ),						nWebServerEnable		);
	config->Write( wxT( "WebserverPort" ),							nWebServerPort			);
	config->Write( wxT( "FramePlacement" ),							sFramePlacement			);

	//--- playlist columns ---//
	for( int i = 0; i < NPLAYLISTCOLUMNS; i++ )
		config->Write( wxString::Format(wxT("PlaylistColumn%dSize"),i),nPlaylistColumnSize[i]);

	g_FX.SaveBands( config );

	delete config;

	#ifdef __WXMSW__
		sAutoRename.Replace( wxT( "/" ), wxT( "\\" ), true );
		sAutoTag.Replace( wxT( "/" ), wxT( "\\" ), true );
	#endif
}

void CMusikPaths::Load()
{
	//--- create the file if it doesn't exist ---//
	if ( !wxFileExists( MUSIK_PATHS_FILENAME ) )
	{
		wxTextFile Out;	
		Out.Create( MUSIK_PATHS_FILENAME );
	}

	//--- once the file exists, load it ---//
	wxTextFile In( MUSIK_PATHS_FILENAME );
	In.Open();
	if ( !In.IsOpened() )
		return;

	m_Paths.Clear();
	for ( size_t i = 0; i < In.GetLineCount(); i++ )
	{
		wxString sCheck = In.GetLine( i );
		sCheck.Trim( false ); sCheck.Trim( true );
		m_Paths.Add( In.GetLine( i ) );
	}
	In.Close();
    
	return;
}

void CMusikPaths::Save()
{
	wxRemoveFile( MUSIK_PATHS_FILENAME );

	wxTextFile Out;	
	Out.Create( MUSIK_PATHS_FILENAME );
	Out.Open();
	
	if ( !Out.IsOpened() )
		return;
	
	for ( size_t i = 0; i < m_Paths.GetCount(); i++ )
		Out.AddLine( m_Paths.Item( i ) );

	Out.Write( Out.GuessType() );
	Out.Close();	

	return;
}

