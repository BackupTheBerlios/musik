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
#include <wx/config.h> 
#include <wx/confbase.h>
#include <wx/fileconf.h> 

template <class T>
struct Value
{
	typedef T valuetyp;
	T val;
	Value(const T& v)
		:val(v)
	{
	}
	Value()
	{
	}
	operator  const T&()	const
	{
		return val;
	}
	operator   T&()	
	{
		return val;
	}
	T* operator &()	
	{
		return &val;
	}
	void operator =(T & v)	
	{
		val = v;
	}

};
template <class T>
struct NullEncoder
{
	static void EncodeValue(T & v)
	{
		v;
	}
	static void DecodeValue(T & v)
	{
		v;
	}
};


template <class T , int MYSIZE,int MYBASE = 0,class T1 =T,class V_ENC = NullEncoder<T> >
class CConfigSettingArray 
{
public:
	CConfigSettingArray(wxConfigBase * pConfig,const wxString &sName, const T1 Default[])
		:m_pConfig(pConfig)
		,m_sName(sName)

	{
		for(size_t i = 0; i < MYSIZE;i++)
		{
			m_pConfig->Read(wxString::Format(m_sName, i + MYBASE),&m_Values[i], Default[i]);
			V_ENC::DecodeValue(m_Values[i]);
		}
	}
	~CConfigSettingArray()
	{
		for(size_t i = 0; i < MYSIZE;i++)
		{
			V_ENC::EncodeValue(m_Values[i]);
			m_pConfig->Write(wxString::Format(m_sName, i + MYBASE),m_Values[i]);
		}
	}
	const T1 &operator [](int i)	 const
	{
		return m_Values[i];
	}

	T1& operator [] (int i)
	{
		return *(T1*)(&static_cast<T&>(m_Values[i]));
	}
protected:
private:
	wxConfigBase * m_pConfig;
	wxString m_sName;
	T m_Values[MYSIZE];
};




template <class T , class T1 = T,class V_ENC = NullEncoder<T> >
class CConfigSetting : public T
{
public:
	CConfigSetting(wxConfigBase * pConfig,const wxString &sName, const T1 &Default)
		:m_pConfig(pConfig)
		,m_sName(sName)

	{
		m_pConfig->Read(m_sName,&static_cast<T&>(*this), Default);
		V_ENC::DecodeValue(*this);
	}
	~CConfigSetting()
	{
		V_ENC::EncodeValue(*this);
	   m_pConfig->Write(m_sName,static_cast<T&>(*this));
	}
	operator  T1&()	
	{
		return *(T1*)(&static_cast<T&>(*this));
	}

	void operator=(const T1 & v)
	{
		*(T1*)(&static_cast<T&>(*this)) = v;
	}
protected:
private:
	wxConfigBase * m_pConfig;
	wxString m_sName;
};


typedef	 CConfigSetting<Value<bool> > CConfigSettingBool;
typedef	 CConfigSetting<Value<int> > CConfigSettingInt;
typedef	 CConfigSetting<wxString> CConfigSettingString;



//-------------------------------------------------//
//--- if we're in windows there is a problem	---//
//--- writing a backslash "\" character to		---//
//--- file, then later reading it. so it is		---//
//--- stored as a forward slash "/"				---//
//-------------------------------------------------//

struct PathEncoder
{
	static void EncodeValue(wxString & v)
	{
	#ifdef __WXMSW__
		v.Replace( wxT( "\\" ), wxT( "/" ), true );
	#endif
	}
	static void DecodeValue(wxString & v)
	{
	#ifdef __WXMSW__
		v.Replace( wxT("/"), wxT("\\"), true );;
	#endif
	}
};

typedef CConfigSetting<wxString,wxString,PathEncoder>  CConfigSettingPath;


#if defined __WXMSW__
#define CONFIG_NAME wxT( ".Musik\\Musik.ini" )
#elif defined __WXGTK__
#define CONFIG_NAME wxT( "MusikPrefs" )
#endif


static const bool DefaultPlaylistColumnEnable[NPLAYLISTCOLUMNS] = {
	true,
	true,
	true,
	true,
	true,
	false,
	true,
	false,
	false,
	true,
	false,
	false,
	false,
	false
};
static const int DefaultPlaylistColumnSize[NPLAYLISTCOLUMNS]   ={
		50,
		50,
		40,
		20,
		20,
		50,
		20,
		50,
		50,
		50,
		50,
		40,
		40,
		40
};

static const bool DefaultPlaylistColumnDynamic[NPLAYLISTCOLUMNS] = {
		false,
		false,
		true,
		true,
		true,
		false,
		true,
		false,
		false,
		false,
		false,
		true,
		false,
		false
};
static const EMUSIK_ACTIVITY_TYPE DefaultActBoxType[ActivityBoxesMaxCount] = {MUSIK_LBTYPE_ARTISTS,MUSIK_LBTYPE_ALBUMS,MUSIK_LBTYPE_NULL,MUSIK_LBTYPE_NULL};
#pragma warning(disable : 4355)	
class CMusikPrefs  : public wxFileConfig
{
public:
	CMusikPrefs()
		:wxFileConfig( CONFIG_NAME )
		,bFirstRun(this,wxT("FirstRun"),true)
		,bAutoAdd(this,wxT( "AddFilesonStartup" ),false)
		,bAutoPlayOnAppStart(this,wxT( "AutoPlayOnAppStart" ),false)
		,eSelStyle(this,wxT( "SelectionStyle" ),MUSIK_SELECTION_TYPE_STANDARD)
		,bShowAllSongs(this,wxT( "SelectingLibraryDisplaysSongs" ),true)
		,nSndOutput(this,wxT( "OutputDriver" ),0)
		,nSndDevice(this,wxT( "SoundDevice" ),0)
		,nSndRate(this,wxT( "PlaybackRate" ),44100)
		,nSndMaxChan(this,wxT( "MaximumChannels" ),4)
		,nSndBuffer(this,wxT( "SoundBufferMilliseconds" ),1000)
		,nSndVolume(this,wxT( "SoundVolume" ),200)

		,bUseEQ(this,wxT( "UseEQ" ),false)
		,bUsePitch(this,wxT( "UsePitch" ),false)
		,bPLStripes(this,wxT( "PlaylistStripes" ),true)
		,bActStripes(this,wxT( "ActivityStripes" ),false)
		,bSourcesStripes(this,wxT( "SourcesStripes" ),false)
		,sPLStripeColour(this,wxT( "PlaylistStripeColour" ),wxT("244, 244, 244"))
		,sActStripeColour(this,wxT( "ActivityStripeColour" ),wxT("244, 244, 244"))
		,sSourcesStripeColour(this,wxT( "SourcesStripeColour" ),wxT("244, 244, 244"))
		,bShowPLInfo(this,wxT( "ShowPlaylistInfo" ),true)
		,bShowSources(this,wxT( "ShowSources" ),true)
		,bShowSourcesIcons(this,wxT( "ShowSourcesIcons" ),true)
		,bShowActivities(this,wxT( "ShowActivities" ),true)
		,bStayOnTop(this,wxT( "StayOnTop" ),false)
		,bActBoxWrite(this,wxT( "ActivityBoxWrite" ),false)
		,bActBoxClear(this,wxT( "ActivityBoxClear" ),false)
		,bActBoxRename(this,wxT( "ActivityBoxRename" ),false)

		,bTagDlgWrite(this,wxT( "TagDialogWrite" ),false)
		,bTagDlgClear(this,wxT( "TagDialogClear" ),false)
		,bTagDlgRename(this,wxT( "TagDialogRename" ),false)

		,ePlaymode(this,wxT( "Playmode" ),MUSIK_PLAYMODE_NORMAL)
		,nMaxShuffleHistory(this,wxT( "MaxShuffleHistory" ),500)
		,nAutoDjDoNotPlaySongPlayedTheLastNHours(this,wxT( "AutoDjDoNotPlaySongPlayedTheLastNHours" ),5*24)
		,nAutoDJChooseSongsToPlayInAdvance(this,wxT( "AutoDJChooseSongsToPlayInAdvance" ),15)
		,bGlobalFadeEnable(this,wxT( "GlobalFaderEnabled" ),true)
		,bFadeEnable(this,wxT( "CrossfaderEnabled" ),true)
		,bFadeSeekEnable(this,wxT( "CrossfaderSeekEnabled" ),true)
		,bFadePauseResumeEnable(this,wxT( "CrossfaderPauseResumeEnabled" ),true)
		,bFadeStopEnable(this,wxT( "CrossfaderStopEnabled" ),true)
		,bFadeExitEnable(this,wxT( "CrossfaderExitEnabled" ),true)
		,nFadeDuration(this,wxT( "CrossfaderDuration" ),2000)
		,nFadeSeekDuration(this,wxT( "CrossfaderSeekDuration" ),500)
		,nFadePauseResumeDuration(this,wxT( "CrossfaderPauseResumeDuration" ),500)
		,nFadeStopDuration(this,wxT( "CrossfaderStopDuration" ),1000)
		,nFadeExitDuration(this,wxT( "CrossfaderExitDuration" ),2000)

		,bUse_MPEGACCURATE_ForMP3VBRFiles(this,wxT( "Use_MPEGACCURATE_ForMP3VBRFiles" ),true)
		,bBlankSwears(this,wxT( "BlankSwears" ),false)
		,bSortArtistWithoutPrefix(this,wxT( "BlankSwears" ),false)

		,sAutoRename(this,wxT( "AutoRenameMask" ),wxT( "%6 - %2 - %1" ))
		,sAutoTag(this,wxT( "AutoTagMask" ),wxT( "%a/%b/%n - %t|%a/%b - %n - %t|%a - %b - %n - %t|0" ))
		,bAutoTagConvertUnderscoresToSpaces(this,wxT( "AutoTagConvertUnderscoresToSpaces" ),false)
		,bWebServerEnable(this,wxT( "EnableWebserver" ),false)
		,nWebServerPort(this,wxT( "Webserverport" ),6395)
		,sFramePlacement(this,wxT( "FramePlacement" ),wxT("0,0,800,600,0,0"))

#ifdef wxHAS_TASK_BAR_ICON
		,bHideOnMinimize(this,wxT("HideOnMinimize"),false )
#endif
		,nSourceBoxWidth(this,wxT( "SourceBoxWidth" ),130)
		,nActivityCtrlHeight(this,wxT( "ActivityCtrlHeight" ),120)
		,bPlaylistSmartColumns(this,wxT( "SmartPlaylistColumns" ),true)

		,bTunageWriteFile(this,wxT( "TunageWriteFile" ),false)
		,sTunageFilename(this,wxT( "TunageFilename" ),wxT("") )
		,sTunageFileLine(this,wxT( "TunageFileLine" ),wxT("") )
		,bTunageAppendFile(this,wxT( "TunageAppendFile" ),false)
		,bTunagePostURL(this,wxT( "TunagePostURL" ),false)
		,sTunageURL(this,wxT( "TunageURL" ),wxT(""))
		,bTunageRunApp(this,wxT( "TunageRunApp" ),false)
		,sTunageCmdLine(this,wxT( "TunageCmdLine" ),wxT(""))
		,bTunageRunOnStop(this,wxT( "TunageRunOnStop" ),false)
		,sTunageStoppedText(this,wxT( "TunageStoppedText" ),wxString(MUSIKAPPNAME) + _(" is not running") )

		,bUseProxyServer(this,wxT( "UseProxyServer" ),false	)			
		,sProxyServer(this,wxT( "ProxyServer" ),wxT(""))			
		,sProxyServerPort(this,wxT( "ProxyServerPort" ),wxT(""))		
		,sProxyServerUser(this,wxT( "ProxyServerUser" ),wxT(""))		
		,sProxyServerPassword(this,wxT( "ProxyServerPassword" ),wxT(""))	
		,nStreamingBufferSize(this,wxT( "StreamingBufferSize" ),64000)		
		,nStreamingPreBufferPercent(this,wxT( "StreamingPreBufferPercent" ),60)
		,nStreamingReBufferPercent(this,wxT( "StreamingReBufferPercent" ),80)

		,bPlaylistColumnEnable(this,wxT( "PlaylistColumn%dEnable"	),DefaultPlaylistColumnEnable)
		,nPlaylistColumnSize(this,wxT( "PlaylistColumn%dSize"	),DefaultPlaylistColumnSize)
		,bPlaylistColumnDynamic(this,wxT( "PlaylistColumn%dDynamic"	),DefaultPlaylistColumnDynamic)

		,nActBoxType(this,	wxT( "ActivityBox%d" )   ,DefaultActBoxType)
	{
		LoadPrefs();
	}
	~CMusikPrefs() 
	{
		SavePrefs();
	}

	void LoadPrefs();
	void SavePrefs();

	CConfigSettingBool bFirstRun;
	CConfigSettingBool bAutoPlayOnAppStart;
	CConfigSettingBool bAutoAdd;
	CConfigSettingBool bShowAllSongs;

	CConfigSetting<Value<int>,EMUSIK_ACTIVITY_SELECTION_TYPE> eSelStyle;

	CConfigSettingInt nSndOutput;
	CConfigSettingInt nSndDevice;
	CConfigSettingInt nSndRate;
	CConfigSettingInt nSndBuffer;
	CConfigSettingInt nSndMaxChan;
 	CConfigSettingInt nSndVolume;

	CConfigSettingBool bUseEQ;
	CConfigSettingBool bUsePitch;

	CConfigSettingArray<int,ActivityBoxesMaxCount,1,EMUSIK_ACTIVITY_TYPE> nActBoxType;

	CConfigSettingBool bPLStripes;
	CConfigSettingBool bActStripes;
	CConfigSettingBool bSourcesStripes;

	CConfigSettingString sPLStripeColour;
	CConfigSettingString sActStripeColour;
	CConfigSettingString sSourcesStripeColour;

	CConfigSettingBool bShowPLInfo;
	CConfigSettingBool bShowSources;
	CConfigSettingBool bShowSourcesIcons;
	CConfigSettingBool bShowActivities;
	CConfigSettingBool bStayOnTop;

	CConfigSettingBool bActBoxWrite;
	CConfigSettingBool bActBoxClear;
	CConfigSettingBool bActBoxRename;

	CConfigSettingBool bTagDlgWrite;
	CConfigSettingBool bTagDlgClear;
	CConfigSettingBool bTagDlgRename;

	CConfigSetting<Value<int>,EMUSIK_PLAYMODE> ePlaymode;
	CConfigSettingBool bGlobalFadeEnable;
	CConfigSettingBool bFadeEnable;
	CConfigSettingBool bFadeSeekEnable;
	CConfigSettingBool bFadePauseResumeEnable;
	CConfigSettingBool bFadeStopEnable;
	CConfigSettingBool bFadeExitEnable;

	CConfigSettingInt nFadeDuration;
	CConfigSettingInt nFadeSeekDuration;
	CConfigSettingInt nFadePauseResumeDuration;
	CConfigSettingInt nFadeStopDuration;
	CConfigSettingInt nFadeExitDuration;

	CConfigSettingBool bUse_MPEGACCURATE_ForMP3VBRFiles;

	CConfigSettingBool bBlankSwears;
	CConfigSettingBool bSortArtistWithoutPrefix;
	
	CConfigSettingBool bWebServerEnable;
	CConfigSettingInt nWebServerPort;

	CConfigSettingString sFramePlacement;
	CConfigSettingInt nSourceBoxWidth;
	CConfigSettingInt nActivityCtrlHeight;
#ifdef wxHAS_TASK_BAR_ICON
	CConfigSettingBool bHideOnMinimize;
#endif
	CConfigSettingPath sAutoRename;
	CConfigSettingPath sAutoTag;
    CConfigSettingBool bAutoTagConvertUnderscoresToSpaces;

	CConfigSettingBool bPlaylistSmartColumns;

	CConfigSettingArray<bool,NPLAYLISTCOLUMNS>  bPlaylistColumnEnable;
	CConfigSettingArray<int,NPLAYLISTCOLUMNS> nPlaylistColumnSize;
	CConfigSettingArray<bool,NPLAYLISTCOLUMNS> bPlaylistColumnDynamic;

	CConfigSettingBool bTunageWriteFile;
	CConfigSettingPath sTunageFilename;
	CConfigSettingString sTunageFileLine;
	CConfigSettingBool bTunageAppendFile;
	CConfigSettingBool bTunagePostURL;
	CConfigSettingString sTunageURL;
	CConfigSettingBool bTunageRunApp;
	CConfigSettingPath sTunageCmdLine;
	CConfigSettingBool bTunageRunOnStop;
	CConfigSettingString sTunageStoppedText;

	CConfigSettingBool bUseProxyServer;			
	CConfigSettingString sProxyServer;			
	CConfigSettingString sProxyServerPort;		
	CConfigSettingString sProxyServerUser;		
	CConfigSettingString sProxyServerPassword;	
	CConfigSettingInt nStreamingBufferSize;		
	CConfigSettingInt nStreamingPreBufferPercent;
	CConfigSettingInt nStreamingReBufferPercent;

	wxString sDataPath;

	CConfigSettingInt nMaxShuffleHistory;
	CConfigSettingInt nAutoDjDoNotPlaySongPlayedTheLastNHours;
	CConfigSettingInt nAutoDJChooseSongsToPlayInAdvance;

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

#pragma warning( default : 4355)
#endif
