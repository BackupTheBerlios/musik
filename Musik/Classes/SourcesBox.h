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

enum EMUSIK_LIBRARY_TYPE
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

class CSourcesListBox : public CMusikListCtrl
{
public:
	CSourcesListBox( wxWindow *parent );
	~CSourcesListBox();

	//--------------//
	//--- events ---//
	//--------------//
	void ShowMenu				( wxContextMenuEvent& WXUNUSED(event) );
	void CreateCurPlaylist		( wxCommandEvent& WXUNUSED(event) );
	void StandardPlaylist		( wxCommandEvent& WXUNUSED(event) );
	void DynamicPlaylist		( wxCommandEvent& WXUNUSED(event) );
	void NetStream				( wxCommandEvent& WXUNUSED(event) );
	void EditQuery				( wxCommandEvent& WXUNUSED(event) );
	void Delete					( wxCommandEvent& WXUNUSED(event) )	{ DelSel();		};
	void Rename					( wxCommandEvent& WXUNUSED(event) )	{ RenameSel();	};
	void ToggleIconsEvt			( wxCommandEvent& WXUNUSED(event) );
	void CopyFiles				( wxCommandEvent& WXUNUSED(event) );
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
	void DelSel					( );
	void RenameSel				( );
	int  GetIndex				( )						{ return m_CurSel;		}
	int  GetDragIndex			( )						{ return m_DragIndex;	}
	void ResetAll				( )						{ ClearAll(); InsertColumn( 0, _( "Sources" ) );	}	
	void ShowIconsChecked		( bool bCheck );
	int  GetSelType				( )						{ return  GetType( m_CurSel );	}
	int  GetType				( long index ) const ;
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
	bool PlaylistToFile				( wxString sName, wxString* sItems, int type, bool bDelOld = true );
	wxString PromptDynamicPlaylist	( wxString sQuery );
	wxString PromptNetStreamAddress	( wxString sAddress );
	wxString GetPlaylistName		( int nIndex );

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

	//--- popup menus ---//
	wxMenu	*sources_context_menu;
	wxMenu	*sources_context_menu_new;

	//--- if deleting, make sure events don't go through ---//
	bool m_Deleting;

	//--- some of these may go ---//
	int			m_CurSel;
	int			m_DragIndex;
};

class CSourcesBox : public wxSashLayoutWindow
{
public:
	CSourcesBox( wxWindow *parent );
	~CSourcesBox();

	//-------------------------------------------------------------//
	//--- abastraction, so other controls can use g_SourcesCtrl ---//
	//--- only place needed functions here to reduce overhead	---//
	//-------------------------------------------------------------//
	int  GetSelType				( )	{ return pListBox->GetSelType();	}
	void Update					( ) { pListBox->Update();				}
	void UpdateCurrent			( ) { pListBox->UpdateSel(pListBox->GetIndex()); }
	void SelectLibrary			( ) { pListBox->UpdateSel((size_t) -2); }
	void OnSashDragged	(wxSashEvent & ev);

	wxString PromptDynamicPlaylist( wxString sQuery )	{ return pListBox->PromptDynamicPlaylist( sQuery ); }
	
	DECLARE_EVENT_TABLE()
private:
	CSourcesListBox *pListBox;
	wxBoxSizer		*pSizer;
};

class SourcesDropTarget : public wxTextDropTarget
{
public:
	SourcesDropTarget( CSourcesListBox *pBox )	{ m_SourcesListBox = pBox; }

    virtual bool			OnDropText(wxCoord x, wxCoord y, const wxString& text);
	virtual wxDragResult	OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
	void HighlightSel( wxPoint );

private:
	CSourcesListBox* m_SourcesListBox;
	int nLastHit;
};

#endif
