/*
 *  MusikPrefsFrame.h
 *
 *  Preferences frame
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
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
	void OnTreeChange		( wxCommandEvent& WXUNUSED(event) );
	void OnOutputChanged	( wxCommandEvent& WXUNUSED(event) );
	void OnClickOK			( wxCommandEvent& WXUNUSED(event) );
	void OnClickApply		( wxCommandEvent& WXUNUSED(event) );
	void OnClickCancel		( wxCommandEvent& WXUNUSED(event) );
	void OnClose			( wxCommandEvent& WXUNUSED(event) );
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
	wxCheckBox	*chkAutoDelta;
	wxCheckBox	*chkExtendedPlaylist;
	wxCheckBox	*chkShowAllSongs;
	wxCheckBox	*chkBlankSwears;
	wxCheckBox	*chkPlaylistStripes;
	wxButton	*btnPlaylistStripeColour;
	wxCheckBox	*chkActivityBoxStripes;
	wxButton	*btnActivityStripeColour;
	wxCheckBox	*chkSourcesBoxStripes;
	wxButton	*btnSourcesStripeColour;

	//-----------------------------//
	//--- options -> selections ---//
	//-----------------------------//
	wxComboBox	*cmbActivityBox1;
	wxComboBox	*cmbActivityBox2;
	wxComboBox	*cmbActivityBox3;
	wxComboBox	*cmbActivityBox4;
	wxComboBox	*cmbSelStyle;

	//-----------------------------//
	//--- options -> playlist	---//
	//-----------------------------//

	wxCheckBox* chkRatingEnable;
	wxCheckBox* chkTrackEnable;
	wxCheckBox* chkTitleEnable;
	wxCheckBox* chkArtistEnable;
	wxCheckBox* chkAlbumEnable;
	wxCheckBox* chkYearEnable;
	wxCheckBox* chkGenreEnable;
	wxCheckBox* chkTimesPlayedEnable;
	wxCheckBox* chkLastPlayedEnable;
	wxCheckBox* chkTimeEnable;
	wxCheckBox* chkBitrateEnable;
	wxCheckBox* chkFilenameEnable;

	wxComboBox* cmbTrackStatic;
	wxComboBox* cmbTitleStatic;
	wxComboBox* cmbArtistStatic;
	wxComboBox* cmbAlbumStatic;
	wxComboBox* cmbYearStatic;
	wxComboBox* cmbGenreStatic;
	wxComboBox* cmbTimesPlayedStatic;
	wxComboBox* cmbLastPlayedStatic;
	wxComboBox* cmbTimeStatic;
	wxComboBox* cmbBitrateStatic;
	wxComboBox* cmbFilenameStatic;

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
	wxCheckBox	*chkRepeat;
	wxCheckBox	*chkShuffle;
	wxCheckBox	*chkCrossfade;
	wxTextCtrl	*tcDuration;
	wxCheckBox	*chkCrossfadeSeek;
	wxTextCtrl	*tcSeekDuration;

	//-----------------------//
	//--- sound -> driver ---//
	//-----------------------//
	wxComboBox	*cmbOutputDrv;
	wxComboBox	*cmbSndDevice;
	wxComboBox	*cmbPlayRate;
	wxTextCtrl	*tcBufferLength;

	//--------------//
	//--- sizers ---//
	//--------------//
	wxBoxSizer	*vsSound_Driver;
	wxBoxSizer	*vsSound_Playback;
	wxBoxSizer	*vsOptions_Selections;
	wxBoxSizer	*vsOptions_Playlist;
	wxBoxSizer	*vsOptions_Interface;
	wxBoxSizer	*vsOptions_Misc;
	wxBoxSizer	*vsTagging_General;
	wxBoxSizer	*vsTagging_Auto;
	wxBoxSizer	*hsSplitter;
	wxBoxSizer	*hsActivityBox1;
	wxBoxSizer	*hsActivityBox2;
	wxBoxSizer	*hsActivityBox3;
	wxBoxSizer	*hsActivityBox4;
	wxBoxSizer	*hsSelStyle;
	wxBoxSizer	*hsSelectionStyle;
	wxBoxSizer	*hsBufferLength;
	wxBoxSizer	*hsDuration;
	wxBoxSizer	*hsSeekDuration;
	wxBoxSizer	*vsOutputDrv;	
	wxBoxSizer	*vsSndDevice;
	wxBoxSizer	*vsPlayRate;
	wxBoxSizer	*hsPLRating;
	wxBoxSizer	*hsPLTrack;
	wxBoxSizer	*hsPLTitle;
	wxBoxSizer	*hsPLArtist;
	wxBoxSizer	*hsPLAlbum;
	wxBoxSizer	*hsPLYear;
	wxBoxSizer	*hsPLGenre;
	wxBoxSizer	*hsPLTimesPlayed;
	wxBoxSizer	*hsPLLastPlayed;
	wxBoxSizer	*hsPLTime;
	wxBoxSizer	*hsPLBitrate;
	wxBoxSizer	*hsPLFilename;
	wxBoxSizer	*hsSysButtons;
	wxBoxSizer	*vsTopSizer;

	DECLARE_EVENT_TABLE()
private:

	//--------------------------//
	//--- selection tracking ---//
	//--------------------------//
	wxTreeItemId nRootID;
	wxTreeItemId nSoundRootID;
	wxTreeItemId nOptionsRootID;
	wxTreeItemId nTagRootID;
	wxTreeItemId nPlaybackID;
	wxTreeItemId nDriverID;
	wxTreeItemId nSelectionsID;
	wxTreeItemId nPlaylistID;
	wxTreeItemId nInterfaceID;	
	wxTreeItemId nGeneralTagID;
	wxTreeItemId nAutoTagID;
};

#endif
