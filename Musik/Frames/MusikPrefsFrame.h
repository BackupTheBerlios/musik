/*
 *  MusikPrefsFrame.h
 *
 *  Preferences frame
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef MUSIK_PREFS_FRAME_H
#define MUSIK_PREFS_FRAME_H

//--- wx ---//
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 
#include "wx/treectrl.h"

#include "../MusikDefines.h"
#include "../Classes/ActivityBox.h"
enum EMUSIK_PREFERENCES_OBJECT_ID
{
	MUSIK_PREFERENCES_TREE = 0,
	MUSIK_PREFERENCES_OUTPUT_DRV,
	MUSIK_PREFERENCES_SND_DEVICE,
	MUSIK_PREFERENCES_PLAY_RATE,
	MUSIK_PREFERENCES_OK,
	MUSIK_PREFERENCES_APPLY,
	MUSIK_PREFERENCES_CANCEL,
	MUSIK_PREFERENCES_PLAYLIST_STRIPE_COLOUR,
	MUSIK_PREFERENCES_ACTIVITY_STRIPE_COLOUR,
	MUSIK_PREFERENCES_SOURCES_STRIPE_COLOUR
};

class MusikPrefsFrame : public wxFrame
{
public:
	MusikPrefsFrame( wxFrame* pParent, const wxString &sTitle, const wxPoint &pos, const wxSize &size );

	//--------------//
	//--- events ---//
	//--------------//
	void OnTranslateKeys	( wxKeyEvent	 &event );
	void OnTreeChange		( wxTreeEvent& WXUNUSED(event) );
	void OnOutputChanged	( wxCommandEvent& WXUNUSED(event) );
	void OnClickOK			( wxCommandEvent& WXUNUSED(event) );
	void OnClickApply		( wxCommandEvent& WXUNUSED(event) );
	void OnClickCancel		( wxCommandEvent& WXUNUSED(event) );
	void OnClose			( wxCloseEvent& WXUNUSED(event) );
	void OnClickColour		( wxCommandEvent& event );

	//-------------------------//
	//--- utility functions ---//
	//-------------------------//
	void LoadPrefs			();
	void SavePrefs			();
	void FindDevices		();
	void HidePanels			();
	void UpdatePrefsPanel	();
	void Close				( bool bCancel );

	//--------------------//
	//--- tree control ---//
	//--------------------//
	wxTreeCtrl	*tcPreferencesTree;

	//----------------------//
	//--- system buttons ---//
	//----------------------//
	wxButton	*btnOK;
	wxButton	*btnApply;
	wxButton	*btnCancel;

	//--------------------------//
	//--- options -> general ---//
	//--------------------------//
	wxCheckBox	*chkAutoScan;
	wxCheckBox	*chkAutoPlayOnAppStart;
#ifdef wxHAS_TASK_BAR_ICON
	wxCheckBox	*chkHideOnMinimize;
#endif
	wxCheckBox	*chkExtendedPlaylist;
	wxCheckBox	*chkShowAllSongs;
	wxCheckBox	*chkBlankSwears;
	wxCheckBox	*chkSortArtistWithoutPrefix;
	wxCheckBox	*chkPlaylistStripes;
	wxButton	*btnPlaylistStripeColour;
	wxCheckBox	*chkActivityBoxStripes;
	wxButton	*btnActivityStripeColour;
	wxCheckBox	*chkSourcesBoxStripes;
	wxButton	*btnSourcesStripeColour;

	//-----------------------------//
	//--- options -> selections ---//
	//-----------------------------//
	wxComboBox	*cmbActivityBoxes[ActivityBoxesMaxCount];
	wxComboBox	*cmbSelStyle;

	//-----------------------------//
	//--- options -> playlist	---//
	//-----------------------------//

	wxCheckBox* chkPLColumnEnable[NPLAYLISTCOLUMNS];

	wxComboBox*	cmbPLColumnStatic[NPLAYLISTCOLUMNS];


	//-------------------------//
	//--- options -> tunage ---//
	//-------------------------//
	wxCheckBox*	chkTunageWriteFile;
	wxCheckBox* chkTunageAppendFile;
	wxCheckBox* chkTunagePostURL;
	wxCheckBox* chkTunageRunApp;
	wxCheckBox* chkTunageRunOnStop;

	wxTextCtrl*	tcTunageFilename;
	wxTextCtrl* tcTunageFileLine;
	wxTextCtrl* tcTunageURL;
	wxTextCtrl* tcTunageCmdLine;
	wxTextCtrl* tcTunageStoppedText;

	wxStaticBox *sbTunageFile;
	wxStaticBox *sbTunageURL;
	wxStaticBox *sbTunageApp;
	wxStaticBox *sbTunageMisc;

	//--------------------------//
	//--- tagging -> general ---//
	//--------------------------//
	wxCheckBox	*chkActivityWrite;
	wxCheckBox	*chkActivityClear;
	wxCheckBox	*chkActivityRename;
	wxCheckBox	*chkTagDlgWrite;
	wxCheckBox	*chkTagDlgClear;
	wxCheckBox	*chkTagDlgRename;

	//---------------------------//
	//--- tagging -> auto tag ---//
	//---------------------------//
	wxTextCtrl	*tcAutoRename;
	wxTextCtrl	*tcAutoTag;

	//-------------------------//
	//--- sound -> playback ---//
	//-------------------------//
	wxCheckBox	*chkCrossfade;
	wxTextCtrl	*tcDuration;
	wxCheckBox	*chkCrossfadeSeek;
	wxTextCtrl	*tcSeekDuration;
	wxCheckBox	*chkCrossfadePauseResume;
	wxTextCtrl	*tcPauseResumeDuration;
	wxCheckBox	*chkCrossfadeStop;
	wxTextCtrl	*tcStopDuration;
	wxCheckBox	*chkCrossfadeExit;
	wxTextCtrl	*tcExitDuration;

	//-----------------------//
	//--- sound -> driver ---//
	//-----------------------//
	wxComboBox	*cmbOutputDrv;
	wxComboBox	*cmbSndDevice;
	wxComboBox	*cmbPlayRate;
	wxTextCtrl	*tcBufferLength;
	wxTextCtrl	*tcMaxChannels;
	wxCheckBox	*chkUse_MPEGACCURATE_ForMP3VBRFiles;
	//---------------------------//
	//--- streaming -> buffer ---//
	//---------------------------//

	wxTextCtrl*	tcStreamingBufferSize;		
	wxTextCtrl* tcStreamingPreBufferPercent;	
	wxTextCtrl* tcStreamingReBufferPercent;	
	//---------------------------------//
	//--- streaming -> proxy server ---//
	//---------------------------------//
	wxCheckBox* chkUseProxyServer;
	wxTextCtrl*	tcProxyServer;		
	wxTextCtrl* tcProxyServerPort;	
	wxTextCtrl* tcProxyServerUser;	
	wxTextCtrl* tcProxyServerPassword;	

	//--------------//
	//--- sizers ---//
	//--------------//
	wxBoxSizer		*vsSound_Driver;
	wxBoxSizer		*vsSound_Crossfader;
	wxFlexGridSizer	*vsOptions_Selections;
	wxFlexGridSizer	*vsOptions_Playlist;
	wxBoxSizer		*vsOptions_Interface;
	wxBoxSizer		*vsOptions_Misc;
	wxBoxSizer		*vsOptions_Tunage;
	wxBoxSizer		*vsTagging_General;
	wxBoxSizer		*vsTagging_Auto;
	wxBoxSizer		*hsSplitter;
	wxBoxSizer		*hsSelectionStyle;
	wxFlexGridSizer	*fsCrossfader;
	wxBoxSizer		*hsSysButtons;
	wxBoxSizer		*vsTopSizer;
	wxFlexGridSizer	*vsStreaming_Buffer;
	wxBoxSizer		*vsStreaming_ProxyServer;

	DECLARE_EVENT_TABLE()
private:

	//--------------------------//
	//--- selection tracking ---//
	//--------------------------//
	wxTreeItemId nRootID;
	wxTreeItemId nSoundRootID;
	wxTreeItemId nOptionsRootID;
	wxTreeItemId nTagRootID;
	wxTreeItemId nTunageID;
	wxTreeItemId nPlaybackID;
	wxTreeItemId nDriverID;
	wxTreeItemId nSelectionsID;
	wxTreeItemId nPlaylistID;
	wxTreeItemId nInterfaceID;	
	wxTreeItemId nGeneralTagID;
	wxTreeItemId nAutoTagID;

	wxTreeItemId nStreamingBufferID;
	wxTreeItemId nStreamingProxyServerID;
};

#endif
