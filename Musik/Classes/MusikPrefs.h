/*
 *  MusikPrefs.h
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

#ifndef MUSIK_PREFS_H
#define MUSIK_PREFS_H

//--- wx core stuff ---//
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 
#include "../MusikGlobals.h"
#include "ActivityBox.h"

class CMusikPrefs
{
public:
	CMusikPrefs() {}
	~CMusikPrefs() {}

	void LoadPrefs();
	void SavePrefs();
	void ResetColumns();

	int nFirstRun;
	int nAutoAdd;
	int nShowAllSongs;

	EMUSIK_ACTIVITY_SELECTION_TYPE eSelStyle;

	int nSndOutput;
	int nSndDevice;
	int nSndRate;
	int nSndBuffer;
	int nSndMaxChan;

	int nSndVolume;
	int nUseEQ;

	EMUSIK_ACTIVITY_TYPE nActBox1;
	EMUSIK_ACTIVITY_TYPE nActBox2;
	EMUSIK_ACTIVITY_TYPE nActBox3;
	EMUSIK_ACTIVITY_TYPE nActBox4;

	int nPLStripes;
	int nActStripes;
	int nSourcesStripes;

	wxString sPLStripeColour;
	wxString sActStripeColour;
	wxString sSourcesStripeColour;

	int nShowPLInfo;
	int nShowSources;
	int nShowSourcesIcons;
	int nShowActivities;
	int nStayOnTop;

	int nActBoxWrite;
	int nActBoxClear;
	int nActBoxRename;

	int nTagDlgWrite;
	int nTagDlgClear;
	int nTagDlgRename;

	int nRepeat;
	int nShuffle;

	int nGlobalFadeEnable;
	int nFadeEnable;
	int nFadeSeekEnable;
	int nFadePauseResumeEnable;
	int nFadeStopEnable;
	int nFadeExitEnable;
	int nFadeDuration;
	int nFadeSeekDuration;
	int nFadePauseResumeDuration;
	int nFadeStopDuration;
	int nFadeExitDuration;

	int nAutoDelta;

	int nBlankSwears;

	int nWebServerEnable;
	int nWebServerPort;

	wxString sFramePlacement;

	wxString sAutoRename;
	wxString sAutoTag;

	int nPlaylistSmartColumns;
	int nPlaylistColumnEnable[NPLAYLISTCOLUMNS];
	int nPlaylistColumnSize[NPLAYLISTCOLUMNS];
	int nPlaylistColumnDynamic[NPLAYLISTCOLUMNS];

	int nTunageWriteFile;
	wxString sTunageFilename;
	wxString sTunageFileLine;
	int nTunageAppendFile;
	int nTunagePostURL;
	wxString sTunageURL;
	int nTunageRunApp;
	wxString sTunageCmdLine;
	int nTunageRunOnStop;
	wxString sTunageStoppedText;
};

class CMusikPaths
{
public:
	CMusikPaths(){}
	~CMusikPaths(){}

	void Load();
	void Save();

	wxString Item( size_t n ){ return m_Paths.Item( n ); }
	size_t GetCount(){ return m_Paths.GetCount(); }
	void Clear(){ m_Paths.Clear(); }
	void Add( wxString s ){ m_Paths.Add( s ); }
	void Remove( size_t nIndex ){ m_Paths.Remove( nIndex ); }

	wxArrayString* GetList(){ return &m_Paths; }

private:
	wxArrayString m_Paths;
};

#endif
