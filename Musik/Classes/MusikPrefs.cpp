/*
 *  MusikPrefs.cpp
 *
 *  Class to handle saving / loading
 *  of all preferences.
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
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
	wxString sDefaultAutoTag = wxT( "%a/%b/%n - %t|%a/%b - %n - %t|%a - %b - %n - %t|0" );
	wxFileConfig *config;
	config = new wxFileConfig( CONFIG_NAME );
	config->Read( wxT( "SelectionStyle" ),						(int*)&eSelStyle,			0						);
	config->Read( wxT( "OutputDriver" ),						&nSndOutput,				0						);
	config->Read( wxT( "SoundDevice" ),							&nSndDevice,				0						);
	config->Read( wxT( "PlaybackRate" ),						&nSndRate,					44100					);
	config->Read( wxT( "MaximumChannels" ),						&nSndMaxChan,				4						);
	config->Read( wxT( "SoundVolume" ),							&nSndVolume,				200						);
	config->Read( wxT( "UseEQ" ),								&nUseEQ,					0						);
	config->Read( wxT( "UsePitch" ),							&nUsePitch,					0						);
	config->Read( wxT( "SoundBufferMilliseconds" ),				&nSndBuffer,				1000					);
	config->Read( wxT( "ActivityBox1" ),						(int*)&nActBox1,			1						);
	config->Read( wxT( "ActivityBox2" ),						(int*)&nActBox2,			2						);
	config->Read( wxT( "ActivityBox3" ),						(int*)&nActBox3,			0						);
	config->Read( wxT( "ActivityBox4" ),						(int*)&nActBox4,			0						);
	config->Read( wxT( "Repeat" ),								&nRepeat,					1						);
	config->Read( wxT( "Shuffle" ),								&nShuffle,					0						);
	config->Read( wxT( "GlobalFaderEnabled" ),					&nGlobalFadeEnable,			1						);
	config->Read( wxT( "CrossfaderEnabled" ),					&nFadeEnable,				1						);
	config->Read( wxT( "CrossfaderSeekEnabled" ),				&nFadeSeekEnable,			1						);
	config->Read( wxT( "CrossfaderPauseResumeEnabled" ),		&nFadePauseResumeEnable,	1						);
	config->Read( wxT( "CrossfaderStopEnabled" ),				&nFadeStopEnable,			1						);
	config->Read( wxT( "CrossfaderExitEnabled" ),				&nFadeExitEnable,			1						);
	config->Read( wxT( "CrossfaderDuration" ),					&nFadeDuration,				2000					);
	config->Read( wxT( "CrossfaderSeekDuration" ),				&nFadeSeekDuration,			500						);
	config->Read( wxT( "CrossfaderPauseResumeDuration" ),		&nFadePauseResumeDuration,	500						);
	config->Read( wxT( "CrossfaderStopDuration" ),				&nFadeStopDuration,			1000					);
	config->Read( wxT( "CrossfaderExitDuration" ),				&nFadeExitDuration,			2000					);
	config->Read( wxT( "ShowPlaylistInfo" ),					&nShowPLInfo,				1						);
	config->Read( wxT( "ShowSources" ),							&nShowSources,				1						);
	config->Read( wxT( "ShowSourcesIcons" ),					&nShowSourcesIcons,			1						);
	config->Read( wxT( "ShowActivities" ),						&nShowActivities,			1						);
	config->Read( wxT( "StayOnTop" ),							&nStayOnTop,				0						);
	config->Read( wxT( "ActivityBoxWrite" ),					&nActBoxWrite,				0						);
	config->Read( wxT( "ActivityBoxClear" ),					&nActBoxClear,				0						);
	config->Read( wxT( "ActivityBoxRename" ),					&nActBoxRename,				0						);
	config->Read( wxT( "TagDialogWrite" ),						&nTagDlgWrite,				0						);
	config->Read( wxT( "TagDialogClear" ),						&nTagDlgClear,				0						);
	config->Read( wxT( "TagDialogRename" ),						&nTagDlgRename,				0						);
	config->Read( wxT( "AutoRenameMask" ),						&sAutoRename,				wxT( "%6 - %2 - %1" )	);
	config->Read( wxT( "AutoTagMask" ),							&sAutoTag,					sDefaultAutoTag			);
	config->Read( wxT( "FirstRun" ),							&nFirstRun,					1						);
	config->Read( wxT( "AddFilesonStartup" ),					&nAutoAdd,					1						);
	config->Read( wxT( "SelectingLibraryDisplaysSongs" ),		&nShowAllSongs,				1						);
	config->Read( wxT( "PlaylistStripes" ),						&nPLStripes,				1						);
	config->Read( wxT( "ActivityStripes" ),						&nActStripes,				0						);
	config->Read( wxT( "SourcesStripes" ),						&nSourcesStripes,			0						);
	config->Read( wxT( "PlaylistStripeColour" ),				&sPLStripeColour,			wxT("244, 244, 244")	);
	config->Read( wxT( "ActivityStripeColour" ),				&sActStripeColour,			wxT("244, 244, 244")	);
	config->Read( wxT( "SourcesStripeColour" ),					&sSourcesStripeColour,		wxT("244, 244, 244")	);
	config->Read( wxT( "BlankSwears" ),							&nBlankSwears,				0						);
	config->Read( wxT( "EnableWebserver" ),						&nWebServerEnable,			0						);
	config->Read( wxT( "Webserverport" ),						&nWebServerPort,			6395					);
	config->Read( wxT( "FramePlacement" ),						&sFramePlacement,			wxT("0,0,800,600,0,0")	);
	config->Read( wxT( "SmartPlaylistColumns" ),				&nPlaylistSmartColumns,		1						);
	config->Read( wxT( "Use_MPEGACCURATE_ForMP3VBRFiles" ),		&nUse_MPEGACCURATE_ForMP3VBRFiles,	1				);
	//-----------------------------------------------------//
	//--- we gotta load these by hand (not with a loop)	---//
	//--- so at one point we can mess with column		---//
	//--- ordering. sorry gunnar. ;)					---//
	//-----------------------------------------------------//
	//--- rating, 50 pixels, enabled, static ---//
	config->Read( wxString::Format( wxT( "PlaylistColumn%dEnable"	),	PLAYLISTCOLUMN_RATING		),	&nPlaylistColumnEnable	[PLAYLISTCOLUMN_RATING],		1	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dSize"		),	PLAYLISTCOLUMN_RATING		),	&nPlaylistColumnSize	[PLAYLISTCOLUMN_RATING],		50	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dDynamic"	),	PLAYLISTCOLUMN_RATING		),	&nPlaylistColumnDynamic	[PLAYLISTCOLUMN_RATING],		0	);
	//--- track number, 50 pixels, enabled, static ---//
	config->Read( wxString::Format( wxT( "PlaylistColumn%dEnable"	),	PLAYLISTCOLUMN_TRACK		),	&nPlaylistColumnEnable	[PLAYLISTCOLUMN_TRACK],			1	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dSize"		),	PLAYLISTCOLUMN_TRACK		),	&nPlaylistColumnSize	[PLAYLISTCOLUMN_TRACK],			50	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dDynamic"	),	PLAYLISTCOLUMN_TRACK		),	&nPlaylistColumnDynamic	[PLAYLISTCOLUMN_TRACK],			0	);
	//--- title, 40%, enabled, dynamic ---//
	config->Read( wxString::Format( wxT( "PlaylistColumn%dEnable"	),	PLAYLISTCOLUMN_TITLE		),	&nPlaylistColumnEnable	[PLAYLISTCOLUMN_TITLE],			1	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dSize"		),	PLAYLISTCOLUMN_TITLE		),	&nPlaylistColumnSize	[PLAYLISTCOLUMN_TITLE],			40	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dDynamic"	),	PLAYLISTCOLUMN_TITLE		),	&nPlaylistColumnDynamic	[PLAYLISTCOLUMN_TITLE],			1	);
	//--- artist, 20%, enabled, dynamic ---//
	config->Read( wxString::Format( wxT( "PlaylistColumn%dEnable"	),	PLAYLISTCOLUMN_ARTIST		),	&nPlaylistColumnEnable	[PLAYLISTCOLUMN_ARTIST],		1	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dSize"		),	PLAYLISTCOLUMN_ARTIST		),	&nPlaylistColumnSize	[PLAYLISTCOLUMN_ARTIST],		20	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dDynamic"	),	PLAYLISTCOLUMN_ARTIST		),	&nPlaylistColumnDynamic	[PLAYLISTCOLUMN_ARTIST],		1	);
	//--- album, 20%, enabled, dynamic ---//
	config->Read( wxString::Format( wxT( "PlaylistColumn%dEnable"	),	PLAYLISTCOLUMN_ALBUM		),	&nPlaylistColumnEnable	[PLAYLISTCOLUMN_ALBUM],			1	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dSize"		),	PLAYLISTCOLUMN_ALBUM		),	&nPlaylistColumnSize	[PLAYLISTCOLUMN_ALBUM],			20	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dDynamic"	),	PLAYLISTCOLUMN_ALBUM		),	&nPlaylistColumnDynamic	[PLAYLISTCOLUMN_ALBUM],			1	);
	//--- year, 50 pixels, disabled, static ---//
	config->Read( wxString::Format( wxT( "PlaylistColumn%dEnable"	),	PLAYLISTCOLUMN_YEAR			),	&nPlaylistColumnEnable	[PLAYLISTCOLUMN_YEAR],			0	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dSize"		),	PLAYLISTCOLUMN_YEAR			),	&nPlaylistColumnSize	[PLAYLISTCOLUMN_YEAR],			50	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dDynamic"	),	PLAYLISTCOLUMN_YEAR			),	&nPlaylistColumnDynamic	[PLAYLISTCOLUMN_YEAR],			0	);
	//--- genre, 20%, enabled, dynamic ---//
	config->Read( wxString::Format( wxT( "PlaylistColumn%dEnable"	),	PLAYLISTCOLUMN_GENRE		),	&nPlaylistColumnEnable	[PLAYLISTCOLUMN_GENRE],			1	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dSize"		),	PLAYLISTCOLUMN_GENRE		),	&nPlaylistColumnSize	[PLAYLISTCOLUMN_GENRE],			20	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dDynamic"	),	PLAYLISTCOLUMN_GENRE		),	&nPlaylistColumnDynamic	[PLAYLISTCOLUMN_GENRE],			1	);
	//--- times played, 50, disabled, static ---//
	config->Read( wxString::Format( wxT( "PlaylistColumn%dEnable"	),	PLAYLISTCOLUMN_TIMES_PLAYED	),	&nPlaylistColumnEnable	[PLAYLISTCOLUMN_TIMES_PLAYED],	0	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dSize"		),	PLAYLISTCOLUMN_TIMES_PLAYED	),	&nPlaylistColumnSize	[PLAYLISTCOLUMN_TIMES_PLAYED],	50	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dDynamic"	),	PLAYLISTCOLUMN_TIMES_PLAYED	),	&nPlaylistColumnDynamic	[PLAYLISTCOLUMN_TIMES_PLAYED],	0	);
	//--- last played, 50, disabled, static ---//
	config->Read( wxString::Format( wxT( "PlaylistColumn%dEnable"	),	PLAYLISTCOLUMN_LAST_PLAYED	),	&nPlaylistColumnEnable	[PLAYLISTCOLUMN_LAST_PLAYED],	0	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dSize"		),	PLAYLISTCOLUMN_LAST_PLAYED	),	&nPlaylistColumnSize	[PLAYLISTCOLUMN_LAST_PLAYED],	50	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dDynamic"	),	PLAYLISTCOLUMN_LAST_PLAYED	),	&nPlaylistColumnDynamic	[PLAYLISTCOLUMN_LAST_PLAYED],	0	);
	//--- time, 50, enabled, static ---//
	config->Read( wxString::Format( wxT( "PlaylistColumn%dEnable"	),	PLAYLISTCOLUMN_TIME			),	&nPlaylistColumnEnable	[PLAYLISTCOLUMN_TIME],			1	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dSize"		),	PLAYLISTCOLUMN_TIME			),	&nPlaylistColumnSize	[PLAYLISTCOLUMN_TIME],			50	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dDynamic"	),	PLAYLISTCOLUMN_TIME			),	&nPlaylistColumnDynamic	[PLAYLISTCOLUMN_TIME],			0	);
	//--- bitrate, 50, disabled, static ---//
	config->Read( wxString::Format( wxT( "PlaylistColumn%dEnable"	),	PLAYLISTCOLUMN_BITRATE		),	&nPlaylistColumnEnable	[PLAYLISTCOLUMN_BITRATE],		0	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dSize"		),	PLAYLISTCOLUMN_BITRATE		),	&nPlaylistColumnSize	[PLAYLISTCOLUMN_BITRATE],		50	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dDynamic"	),	PLAYLISTCOLUMN_BITRATE		),	&nPlaylistColumnDynamic	[PLAYLISTCOLUMN_BITRATE],		0	);
	//--- filename, 40%, disabled, dynamic ---//
	config->Read( wxString::Format( wxT( "PlaylistColumn%dEnable"	),	PLAYLISTCOLUMN_FILENAME		),	&nPlaylistColumnEnable	[PLAYLISTCOLUMN_FILENAME],		0	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dSize"		),	PLAYLISTCOLUMN_FILENAME		),	&nPlaylistColumnSize	[PLAYLISTCOLUMN_FILENAME],		40	);
	config->Read( wxString::Format( wxT( "PlaylistColumn%dDynamic"	),	PLAYLISTCOLUMN_FILENAME		),	&nPlaylistColumnDynamic	[PLAYLISTCOLUMN_FILENAME],		1	);		

	config->Read( wxT("TunageWriteFile"),		&nTunageWriteFile,		0 );
	config->Read( wxT("TunageFilename"),		&sTunageFilename,		wxT("") );
	config->Read( wxT("TunageFileLine"),		&sTunageFileLine,		wxT("") );
	config->Read( wxT("TunageAppendFile"),		&nTunageAppendFile,		0 );
	config->Read( wxT("TunagePostURL"),			&nTunagePostURL,		0 );
	config->Read( wxT("TunageURL"),				&sTunageURL,			wxT("") );
	config->Read( wxT("TunageRunApp"),			&nTunageRunApp,			0 );
	config->Read( wxT("TunageCmdLine"),			&sTunageCmdLine,		wxT("") );
	config->Read( wxT("TunageRunOnStop"),		&nTunageRunOnStop,		1 );
	config->Read( wxT("TunageStoppedText"),		&sTunageStoppedText,	wxT("Musik is not running") );


	g_FX.LoadBands( config );


	//-------------------------------------------------//
	//--- if we're in windows there is a problem	---//
	//--- writing a backslash "\" character to		---//
	//--- file, then later reading it. so it is		---//
	//--- stored as a forward slash "/"				---//
	//-------------------------------------------------//
	#ifdef __WXMSW__
		sAutoRename.Replace( wxT("/"), wxT("\\"), true );
		sAutoTag.Replace( wxT("/"), wxT("\\"), true );
		sTunageFilename.Replace( wxT("/"), wxT("\\"), true );
		sTunageCmdLine.Replace( wxT("/"), wxT("\\"), true );
	#endif

	//--- apply unweildy defaults ---//
	if ( sPLStripeColour.IsEmpty()		) sPLStripeColour		= wxT("244, 244, 244");
	if ( sActStripeColour.IsEmpty()		) sActStripeColour		= wxT("244, 244, 244");
	if ( sSourcesStripeColour.IsEmpty()	) sSourcesStripeColour	= wxT("244, 244, 244");

	delete config;
	SavePrefs();
}

void CMusikPrefs::ResetColumns()
{
	//--- rating, 50 pixels, enabled, static ---//
	nPlaylistColumnEnable	[PLAYLISTCOLUMN_RATING] = 1;
	nPlaylistColumnSize		[PLAYLISTCOLUMN_RATING]	= 50;
	nPlaylistColumnDynamic	[PLAYLISTCOLUMN_RATING]	= 0;

	//--- track number, 50 pixels, enabled, static ---//
	nPlaylistColumnEnable	[PLAYLISTCOLUMN_TRACK]	= 1;
	nPlaylistColumnSize		[PLAYLISTCOLUMN_TRACK]	= 50;
	nPlaylistColumnDynamic	[PLAYLISTCOLUMN_TRACK]	= 0;

	//--- title, 40%, enabled, dynamic ---//
	nPlaylistColumnEnable	[PLAYLISTCOLUMN_TITLE]	= 1;
	nPlaylistColumnSize		[PLAYLISTCOLUMN_TITLE]	= 40;
	nPlaylistColumnDynamic	[PLAYLISTCOLUMN_TITLE]	= 1;
	
	//--- artist, 20%, enabled, dynamic ---//
	nPlaylistColumnEnable	[PLAYLISTCOLUMN_ARTIST]	= 1;
	nPlaylistColumnSize		[PLAYLISTCOLUMN_ARTIST]	= 20;
	nPlaylistColumnDynamic	[PLAYLISTCOLUMN_ARTIST]	= 1;

	//--- album, 20%, enabled, dynamic ---//
	nPlaylistColumnEnable	[PLAYLISTCOLUMN_ALBUM]	= 1;
	nPlaylistColumnSize		[PLAYLISTCOLUMN_ALBUM]	= 20;
	nPlaylistColumnDynamic	[PLAYLISTCOLUMN_ALBUM]	= 1;
	
	//--- year, 50 pixels, disabled, static ---//
	nPlaylistColumnEnable	[PLAYLISTCOLUMN_YEAR]	= 0;
	nPlaylistColumnSize		[PLAYLISTCOLUMN_YEAR]	= 50;
	nPlaylistColumnDynamic	[PLAYLISTCOLUMN_YEAR]	= 0;

	//--- genre, 20%, enabled, dynamic ---//
	nPlaylistColumnEnable	[PLAYLISTCOLUMN_GENRE]	= 1;
	nPlaylistColumnSize		[PLAYLISTCOLUMN_GENRE]	= 20;
	nPlaylistColumnDynamic	[PLAYLISTCOLUMN_GENRE]	= 1;
	
	//--- times played, 50, disabled, static ---//
	nPlaylistColumnEnable	[PLAYLISTCOLUMN_TIMES_PLAYED]	= 0;
	nPlaylistColumnSize		[PLAYLISTCOLUMN_TIMES_PLAYED]	= 50;
	nPlaylistColumnDynamic	[PLAYLISTCOLUMN_TIMES_PLAYED]	= 0;

	//--- last played, 50, disabled, static ---//
	nPlaylistColumnEnable	[PLAYLISTCOLUMN_LAST_PLAYED]	= 0;
	nPlaylistColumnSize		[PLAYLISTCOLUMN_LAST_PLAYED]	= 50;
	nPlaylistColumnDynamic	[PLAYLISTCOLUMN_LAST_PLAYED]	= 0;
	
	//--- time, 50, enabled, static ---//
	nPlaylistColumnEnable	[PLAYLISTCOLUMN_TIME]	= 1;
	nPlaylistColumnSize		[PLAYLISTCOLUMN_TIME]	= 50;
	nPlaylistColumnDynamic	[PLAYLISTCOLUMN_TIME]	= 0;

	//--- bitrate, 50, disabled, static ---//
	nPlaylistColumnEnable	[PLAYLISTCOLUMN_BITRATE]	= 0;
	nPlaylistColumnSize		[PLAYLISTCOLUMN_BITRATE]	= 50;
	nPlaylistColumnDynamic	[PLAYLISTCOLUMN_BITRATE]	= 0;

	//--- filename, 40%, disabled, dynamic ---//
	nPlaylistColumnEnable	[PLAYLISTCOLUMN_FILENAME]	= 0;
	nPlaylistColumnSize		[PLAYLISTCOLUMN_FILENAME]	= 40;
	nPlaylistColumnDynamic	[PLAYLISTCOLUMN_FILENAME]	= 1;	

	nPlaylistSmartColumns =	1;
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
		sTunageFilename.Replace( wxT("\\"), wxT("/"), true );
		sTunageCmdLine.Replace( wxT("\\"), wxT("/"), true );
	#endif

	wxFileConfig *config;
	config = new wxFileConfig( CONFIG_NAME );
	config->Write( wxT( "SelectionStyle" ),					eSelStyle					);
	config->Write( wxT( "OutputDriver" ),					nSndOutput					);
	config->Write( wxT( "SoundDevice" ),					nSndDevice					);
	config->Write( wxT( "PlaybackRate" ),					nSndRate					);
	config->Write( wxT( "MaximumChannels" ),				nSndMaxChan					);
	config->Write( wxT( "SoundVolume" ),					nSndVolume					);
	config->Write( wxT( "UseEQ" ),							nUseEQ						);
	config->Write( wxT( "UsePitch" ),						nUsePitch					);
	config->Write( wxT( "SoundBufferMilliseconds" ),		nSndBuffer					);
	config->Write( wxT( "ActivityBox1" ),					nActBox1					);
	config->Write( wxT( "ActivityBox2" ),					nActBox2					);
	config->Write( wxT( "ActivityBox3" ),					nActBox3					);
	config->Write( wxT( "ActivityBox4" ),					nActBox4					);
	config->Write( wxT( "Repeat" ),							nRepeat						);
	config->Write( wxT( "Shuffle" ),						nShuffle					);
	config->Write( wxT( "GlobalFaderEnabled" ),				nGlobalFadeEnable			);
	config->Write( wxT( "CrossfaderEnabled" ),				nFadeEnable					);
	config->Write( wxT( "CrossfaderSeekEnabled" ),			nFadeSeekEnable				);
	config->Write( wxT( "CrossfaderPauseResumeEnabled" ),	nFadePauseResumeEnable		);
	config->Write( wxT( "CrossfaderStopEnabled" ),			nFadeStopEnable				);
	config->Write( wxT( "CrossfaderExitEnabled" ),			nFadeExitEnable				);
	config->Write( wxT( "CrossfaderDuration" ),				nFadeDuration				);
	config->Write( wxT( "CrossfaderSeekDuration" ),			nFadeSeekDuration			);
	config->Write( wxT( "CrossfaderPauseResumeDuration" ),	nFadePauseResumeDuration	);
	config->Write( wxT( "CrossfaderStopDuration" ),			nFadeStopDuration			);
	config->Write( wxT( "CrossfaderExitDuration" ),			nFadeExitDuration			);
	config->Write( wxT( "ShowPlaylistInfo" ),				nShowPLInfo					);
	config->Write( wxT( "ShowSources" ),					nShowSources				);
	config->Write( wxT( "ShowSourcesIcons" ),				nShowSourcesIcons			);
	config->Write( wxT( "ShowActivities" ),					nShowActivities				);
	config->Write( wxT( "StayOnTop" ),						nStayOnTop					);
	config->Write( wxT( "ActivityBoxWrite" ),				nActBoxWrite				);
	config->Write( wxT( "ActivityBoxClear" ),				nActBoxClear				);
	config->Write( wxT( "ActivityBoxRename" ),				nActBoxRename				);
	config->Write( wxT( "TagDialogWrite" ),					nTagDlgWrite				);
	config->Write( wxT( "TagDialogClear" ),					nTagDlgClear				);
	config->Write( wxT( "TagDialogRename" ),				nTagDlgRename				);
	config->Write( wxT( "AutoRenameMask" ),					sAutoRename					);
	config->Write( wxT( "AutoTagMask" ),					sAutoTag					);
	config->Write( wxT( "FirstRun" ),						nFirstRun					);
	config->Write( wxT( "AddFilesonStartup" ),				nAutoAdd					);
	config->Write( wxT( "SelectingLibraryDisplaysSongs" ),	nShowAllSongs				);
	config->Write( wxT( "PlaylistStripes" ),				nPLStripes					);
	config->Write( wxT( "ActivityStripes" ),				nActStripes					);
	config->Write( wxT( "SourcesStripes" ),					nSourcesStripes				);
	config->Write( wxT( "PlaylistStripeColour" ),			sPLStripeColour				);
	config->Write( wxT( "ActivityStripeColour" ),			sActStripeColour			);
	config->Write( wxT( "SourcesStripeColour" ),			sSourcesStripeColour		);
	config->Write( wxT( "BlankSwears" ),					nBlankSwears				);
	config->Write( wxT( "EnableWebserver" ),				nWebServerEnable			);
	config->Write( wxT( "WebserverPort" ),					nWebServerPort				);
	config->Write( wxT( "FramePlacement" ),					sFramePlacement				);
	config->Write( wxT( "SmartPlaylistColumns" ),			nPlaylistSmartColumns		);
	config->Write( wxT( "Use_MPEGACCURATE_ForMP3VBRFiles" ),nUse_MPEGACCURATE_ForMP3VBRFiles );
	
	//--- playlist columns ---//
	for( int i = 0; i < NPLAYLISTCOLUMNS; i++ )
	{
		config->Write( wxString::Format( wxT( "PlaylistColumn%dEnable" ),	i ),	nPlaylistColumnEnable[i]	);
		config->Write( wxString::Format( wxT( "PlaylistColumn%dSize" ),		i ),	nPlaylistColumnSize[i]		);
		config->Write( wxString::Format( wxT( "PlaylistColumn%dDynamic" ),	i ),	nPlaylistColumnDynamic[i]	);
	}

	config->Write( wxT("TunageWriteFile"),		nTunageWriteFile );
	config->Write( wxT("TunageFilename"),		sTunageFilename );
	config->Write( wxT("TunageFileLine"),		sTunageFileLine );
	config->Write( wxT("TunageAppendFile"),		nTunageAppendFile );
	config->Write( wxT("TunagePostURL"),		nTunagePostURL );
	config->Write( wxT("TunageURL"),			sTunageURL );
	config->Write( wxT("TunageRunApp"),			nTunageRunApp );
	config->Write( wxT("TunageCmdLine"),		sTunageCmdLine );
	config->Write( wxT("TunageRunOnStop"),		nTunageRunOnStop );
	config->Write( wxT("TunageStoppedText"),	sTunageStoppedText );

	g_FX.SaveBands( config );

	delete config;

	#ifdef __WXMSW__
		sAutoRename.Replace( wxT( "/" ), wxT( "\\" ), true );
		sAutoTag.Replace( wxT( "/" ), wxT( "\\" ), true );
		sTunageFilename.Replace( wxT("/"), wxT("\\"), true );
		sTunageCmdLine.Replace( wxT("/"), wxT("\\"), true );
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

