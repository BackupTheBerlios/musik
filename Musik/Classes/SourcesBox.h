/*
 *  SourcesBox.h
 *
 *  The "Sources Box" control
 *	This is the control visible on the left side of the main frame.
 *	It's main function is playlist management.
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef MUSIK_SOURCES_BOX
#define MUSIK_SOURCES_BOX

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 

#include "ActivityBox.h"
#include <wx/laywin.h>

#include <wx/dnd.h>

enum EMUSIK_SOURCES_TYPE
{
	MUSIK_SOURCES_LIBRARY = 0,
    MUSIK_SOURCES_PLAYLIST_STANDARD,
	MUSIK_SOURCES_PLAYLIST_DYNAMIC,
	MUSIK_SOURCES_CDROM_DRIVE,
	MUSIK_SOURCES_NETSTREAM,
	MUSIK_SOURCES_NOW_PLAYING,
	MUSIK_SOURCES_NONE
};

#include "MusikListCtrl.h"

class SourcesDropTarget;

class CSourcesListBox : public CMusikListCtrl
{
public:
	CSourcesListBox( wxWindow *parent );
	~CSourcesListBox();

	//--------------//
	//--- events ---//
	//--------------//
	void ShowMenu				( wxContextMenuEvent& event );
	void CreateCurPlaylist		( wxCommandEvent& event );
	void OnClearPlayerlist		( wxCommandEvent& event );
	void StandardPlaylist		( wxCommandEvent& event );
	void DynamicPlaylist		( wxCommandEvent& event );
	void NetStream				( wxCommandEvent& event );
	void EditQuery				( wxCommandEvent& event );
	void EditURL				( wxCommandEvent& event );
	void Delete					( wxCommandEvent& WXUNUSED(event) )	{ DelSel();		};
	void Rename					( wxCommandEvent& WXUNUSED(event) )	{ RenameSel();	};
	void ToggleIconsEvt			( wxCommandEvent& event );
	void CopyFiles				( wxCommandEvent& event );
	void BeginDrag				( wxListEvent& event );
	void OnUpdateSel			( wxListEvent& event );
	void BeginEditLabel			( wxListEvent& event );
	void EndEditLabel			( wxListEvent& event );
	void TranslateKeys			( wxListEvent& event );
	void OnSourcesColSize		( wxListEvent& event ){ event.Veto(); }

	//------------------------//
	//--- sources list i/o ---//
	//------------------------//
	void Create ( );
	void Load	( );
	void Save	( );
	void Update	( );

	//--------------------------//
	//--- standard functions ---//
	//--------------------------//
	wxMenu * CreateContextMenu();
	void DelSel					( );
	void RenameSel				( );
	int  GetIndex				( )						{ return m_CurSel;		}
	int  GetDragIndex			( )						{ return m_DragIndex;	}
	void ResetAll				( )						{ ClearAll(); InsertColumn( 0, _( "Sources" ) );	}	
	EMUSIK_SOURCES_TYPE  GetSelType				( )						{ return  GetType( m_CurSel );	}
	EMUSIK_SOURCES_TYPE  GetType				( long index ) const ;
	bool GetTypeAsString		( int nType, wxString &sType ) const ;
	int  GetItemImage			( long index );
	
	void RescanPlaylistDir		( );
	void ShowIcons				( );
	void ToggleIcons			( );
	void UpdateSel				( size_t index );

	//-------------------------//
	//--- playlist routines ---//
	//-------------------------//
	void AddMissing			( const wxArrayString & playlists );
	void NewPlaylist		( wxString sName, wxString sVal, int nType );
	bool PlaylistExists		( wxString sName, int type = MUSIK_SOURCES_PLAYLIST_STANDARD );
	void FilenameToSources	( wxString* sFilename );
	void SourcesToFilename	( wxString* sSources, int type = MUSIK_SOURCES_PLAYLIST_STANDARD );
	int  FindInSources		( wxString sName, int nType );
	wxString GetSourceEntry(long i) const;
	//---standard / dynamic ---//
	bool CreateStdPlaylist			( wxString sName, wxString sSongs );
	bool CreateDynPlaylist			( wxString sName );
	bool CreateNetStream			( wxString sName);
    void AppendStdPlaylist			( wxString sName, wxString sSongs );
	void RewriteStdPlaylist			();
	void UpdateDynPlaylist			( int nIndex );
	void LoadStdPlaylist			( wxString sName, wxArrayString & aReturn );
	wxString LoadDynPlaylist		( wxString sName );
	void LoadNetStream				(wxString sName, CMusikSong & song );
	void UpdateNetStream			( int nIndex );
	bool PlaylistToFile				( wxString sName, wxString* sItems, int type, bool bDelOld = true );
	wxString PromptDynamicPlaylist	( wxString sQuery );
	wxString PromptNetStreamAddress	( const wxString & sAddress );
	wxString GetPlaylistName		( int nIndex );
	bool AddSourceContentToNowPlaying(int nIndex);
	DECLARE_EVENT_TABLE()
protected:
	void RescaleColumns			( );
	virtual bool OnRescaleColumns() {RescaleColumns(); return true;}
private:

	//--- virtual functions ---//
    virtual	wxString		OnGetItemText	(long item, long column) const;
	virtual int				OnGetItemImage	(long item) const;
    virtual	wxListItemAttr*	OnGetItemAttr	(long item) const;

	//--- stripe colors ---//
	wxListItemAttr m_LightBold;
	wxListItemAttr m_Light;
	wxListItemAttr m_DarkBold;
	wxListItemAttr m_Dark;


	//--- if deleting, make sure events don't go through ---//
	bool m_Deleting;

	//--- some of these may go ---//
	int			m_CurSel;
	int			m_DragIndex;

	wxArrayString m_SourcesList;

	friend class SourcesDropTarget;
};

class CSourcesBox : public wxSashLayoutWindow
{
public:
	CSourcesBox( wxWindow *parent );
	~CSourcesBox();

	//-------------------------------------------------------------//
	//--- abstraction, so other controls can use g_SourcesCtrl ---//
	//--- only place needed functions here to reduce overhead	---//
	//-------------------------------------------------------------//
	int  GetSelType				( )	{ return pListBox->GetSelType();	}
	void Update					( ) { pListBox->Update();				}
	void UpdateCurrent			( ) { pListBox->UpdateSel(pListBox->GetIndex()); }
	void SelectLibrary			( bool bSwitchView = true ) { pListBox->UpdateSel(bSwitchView ? (size_t) -2:(size_t) -4); }
	void SelectNowPlaying		( ) { pListBox->UpdateSel((size_t) -3); }
	void OnSashDragged	(wxSashEvent & ev);

	wxString PromptDynamicPlaylist( wxString sQuery )	{ return pListBox->PromptDynamicPlaylist( sQuery ); }
	
	DECLARE_EVENT_TABLE()
private:
	CSourcesListBox *pListBox;
};


#endif
