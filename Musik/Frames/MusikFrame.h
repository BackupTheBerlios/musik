/*
 *  MusikFrame.h
 *
 *  Musik's primary frame
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef MUSIK_FRAME_H
#define MUSIK_FRAME_H

//--- objects ---//
#include "../Classes/PlaylistCtrl.h"
#include "../Classes/ActivityBox.h"
#include "../Classes/SourcesBox.h"
#include "../Classes/NowPlayingCtrl.h"
//--- crossfader, other threads ---//
#include "../Threads/MusikThreads.h"

#include <wx/socket.h>
#ifdef wxHAS_TASK_BAR_ICON
#include "wx/taskbar.h"
//class CNowPlayingCtrl;
class MusikLibraryFrame;

class MusikTaskBarIcon: public wxTaskBarIcon
{
public:
	MusikTaskBarIcon(wxFrame * frame) {m_pFrame = frame;};

	void RestoreFrame();
    void OnRButtonUp(wxEvent&);
    void OnLButtonDClick(wxEvent&);
    void OnMenuRestore(wxCommandEvent&);
    void OnMenuHide(wxCommandEvent&);
    void OnMenuPlayPause(wxCommandEvent&);
    void OnMenuPrev(wxCommandEvent&);
    void OnMenuNext(wxCommandEvent&);
    void OnMenuStop(wxCommandEvent&);
    void OnMenuExit(wxCommandEvent&);
    void OnMenuSetNewIcon(wxCommandEvent&);

DECLARE_EVENT_TABLE()

private:
	wxFrame *m_pFrame;
};

#endif

class MusikFrame : public wxFrame
{
public:
	MusikFrame();
	~MusikFrame();

	//--------------//	
	//--- events ---//
	//--------------//
	void OnMove						( wxMoveEvent&	WXUNUSED(event) );
	void OnMaximize					( wxMaximizeEvent&	WXUNUSED(event) );
	void OnIconize					( wxIconizeEvent&	WXUNUSED(event) );
	void OnSize						( wxSizeEvent	&	WXUNUSED(event) );
	void OnClose					( wxCloseEvent&		WXUNUSED(event) );
	void OnMenuClose				( wxCommandEvent&	WXUNUSED(event) ){ Close();};
	void OnMenuAbout				( wxCommandEvent&	event);

	void OnSetupPaths				( wxCommandEvent&	WXUNUSED(event) );
	void OnPreferences				( wxCommandEvent&	event			);
	void OnFX						( wxCommandEvent&	event			);

	void OnStayOnTop				( wxCommandEvent&	WXUNUSED(event) );
	void OnUpdateUIStayOnTop		( wxUpdateUIEvent&	event			);

	void OnSourcesState				( wxCommandEvent&	WXUNUSED(event) );
	void OnUpdateUISourcesState		( wxUpdateUIEvent&	event			);
	void OnActivitiesState			( wxCommandEvent&	WXUNUSED(event) );
	void OnUpdateUIActivitiesState	( wxUpdateUIEvent&	event			);
	void OnPlaylistInfoState		( wxCommandEvent&	WXUNUSED(event) );
	void OnUpdateUIPlaylistInfoState( wxUpdateUIEvent&	event			);

	void OnSimpleQueryDlg			( wxCommandEvent&	WXUNUSED(event) );
	void OnSimpleQueryEdit			( wxCommandEvent&	WXUNUSED(event) );
	void OnCustomQuery				( wxCommandEvent&	WXUNUSED(event) );

	void OnViewDirtyTags			( wxCommandEvent&	WXUNUSED(event) );
	void OnWriteTags				( wxCommandEvent&	WXUNUSED(event) );
	void OnWriteTagsClearDirty		( wxCommandEvent&	WXUNUSED(event) );

	void OnTranslateKeys			( wxKeyEvent&		event			);
	void OnServerEvent				( wxSocketEvent&	event			);
	
	void OnStartProgress			( wxCommandEvent&	WXUNUSED(event) );
	void OnUpdateProgress			( wxCommandEvent&	WXUNUSED(event) );
	void OnEndProgress				( wxCommandEvent&	WXUNUSED(event) );
	
	void OnSashDraggedSourcesBox	(wxSashEvent & ev);
	void OnSashDraggedActivityCtrl	(wxSashEvent & ev);
	//-------------------------//
	//--- virtual overrides ---//
	//-------------------------//
	virtual void SetTitle(const wxString& title);


	//------------------------------//
	//---     activity boxes     ---//
	//---  playlist info control ---//
	//---        playlist        ---//
	//---    playlist control    ---//
	//---    now playing info    ---//
	//---      and sources       ---//
	//--- defined in the globals ---//
	//------------------------------//

	//--- sources bitmaps ---//
	wxBitmap		bmpLibrary;
	wxBitmap		bmpPlaylist;
	wxBitmap		bmpDynamic;
	wxBitmap		bmpNowplaying;
	wxBitmap		bmpNetstream;

	//--- rating bitmaps ---//
	wxBitmap		bmpRating[(MUSIK_MAX_RATING - MUSIK_MIN_RATING) + 1];

	//--- sizers ---//
	wxBoxSizer *vsTopBottom;

	CNowPlayingCtrl		*m_pNowPlayingCtrl;

	//----------------------------------------------------------------//
	//--- Routines dealing with events. List box's clicked, menu   ---//
	//--- items selected, playlist buttons pushed, dialog resized, ---//
	//--- activating, etc. Implmentation for these routines will   ---//
	//--- be will be found in MusikDlgEvents.cpp.                  ---//
	//----------------------------------------------------------------//

	//--- library ---//
	void	LibraryCustomQuery		();
	void	LibrarySimpleQuery		( wxString sQuery );
	void	LibrarySimpleQueryDlg	();
	void	LibrarySimpleQueryEdit	();

	void	AutoUpdate	( const wxArrayString & Filenames = wxArrayString(),unsigned long flags = 0);
	//--- tag related ---//
	void	WriteTags				();

	//--------------------------------------------------------------------//
	//--- User interface routines that deal with creating controls     ---//
	//--- on the fly and the like. Implementation for these will be    ---//
	//--- found in MusikFrame.cpp							           ---//
	//--------------------------------------------------------------------//

	//--- custom layout ---//
	void TogglePlaylistInfo	();
	void ShowPlaylistInfo	();
	void ToggleSources		();
	void ToggleActivities	();
	void ShowSources		();
	void ShowActivityArea	( bool bShow = true );
	void EnableProgress		( bool show = true );
	void SetStayOnTop( bool bStayOnTop );

	//--- images and fonts ---//
	void GetFonts();
	void GetListCtrlFont();
	void LoadImageLists();
	void DeleteImageLists();

	//--- sets ---//
	void SetProgress			( int n )				{ m_Progress = n;			}
	void SetProgressType		( int n )				{ m_ProgressType = n;		}
	void SetActiveThread		( wxThread* thread )	{ m_ActiveThread = thread;	}

	//--- gets ---//
	int  GetProgress			()						{ return m_Progress;		}
	int  GetProgressType		()						{ return m_ProgressType;	}
	wxThread* GetActiveThread	()						{ return m_ActiveThread;	}

	DECLARE_EVENT_TABLE()
protected:
	void CreateMainMenu();
#ifdef __WXMSW__
protected:
    	long MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
#endif
private:
#ifdef wxHAS_TASK_BAR_ICON
	MusikTaskBarIcon* m_pTaskBarIcon;
#endif
	//--- threads and thread related ---//
	int m_Progress;
	int m_ProgressType;
	wxThread* m_ActiveThread;
	MusikWriteDirtyThread* pWriteDirtyThread;
	
	wxTextCtrl* m_TextSimpleQuery;
	wxGauge	  *	m_pProgressGauge;

	wxString m_customQuery;

	wxSashLayoutWindow *m_pBottomPanel;

};

#endif
