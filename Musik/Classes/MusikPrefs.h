/*
 *  MusikPrefs.h
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

#ifndef MUSIK_PREFS_H
#define MUSIK_PREFS_H

//--- wx core stuff ---//
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 
#include "../MusikGlobals.h"

class CMusikPrefs
{
public:
	CMusikPrefs() {}
	~CMusikPrefs() {}

	void LoadPrefs();
	void SavePrefs();

	int nFirstRun;
	int nAutoAdd;
	int nShowAllSongs;

	int nSelStyle;

	int nSndOutput;
	int nSndDevice;
	int nSndRate;
	int nSndBuffer;

	int nSndVolume;
	int nUseEQ;

	int nActBox1;
	int nActBox2;
	int nActBox3;
	int nActBox4;

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
	int nShowRatings;

	int nStayOnTop;

	int nActBoxWrite;
	int nActBoxClear;
	int nActBoxRename;

	int nTagDlgWrite;
	int nTagDlgClear;
	int nTagDlgRename;

	int nRepeat;
	int nShuffle;

	int nFadeEnable;
	int nFadeSeekEnable;
	int nFadeDuration;
	int nFadeSeekDuration;
	int nFadeStopEnable;

	int nAutoDelta;

	int nExtendedPlaylist;

	int nBlankSwears;

	int nWebServerEnable;
	int nWebServerPort;

	wxString sFramePlacement;

	wxString sAutoRename;
	wxString sAutoTag;

	int nPlaylistColumSize[NPLAYLISTCOLUMS];
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
