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

#include "ListHeader.h"

#include <wx/dnd.h>

enum EMUSIK_LIBRARY_TYPE
{
	MUSIK_SOURCES_LIBRARY = 0,
    MUSIK_SOURCES_PLAYLIST_STANDARD,
	MUSIK_SOURCES_PLAYLIST_DYNAMIC,
	MUSIK_SOURCES_CDROM_DRIVE,
	MUSIK_SOURCES_ALARM,
	MUSIK_SOURCES_NONE
};

#include "wx/listctrl.h"

class CSourcesListBox : public wxListCtrl
{
public:
	CSourcesListBox( wxPanel *parent, wxWindowID id );
	~CSourcesListBox();

	//--------------//
	//--- events ---//
	//--------------//
	void ShowMenu				( wxCommandEvent& WXUNUSED(event) );
	void CreateCurPlaylist		( wxCommandEvent& WXUNUSED(event) );
	void StandardPlaylist		( wxCommandEvent& WXUNUSED(event) );
	void DynamicPlaylist		( wxCommandEvent& WXUNUSED(event) );
	void EditQuery				( wxCommandEvent& WXUNUSED(event) );
	void Delete					( wxCommandEvent& WXUNUSED(event) )	{ DelSel();		};
	void Rename					( wxCommandEvent& WXUNUSED(event) )	{ RenameSel();	};
	void ToggleIconsEvt			( wxCommandEvent& WXUNUSED(event) );
	void CopyFiles				( wxCommandEvent& WXUNUSED(event) );
	void BeginDrag				( wxListEvent& event );
	void OnUpdateSel			( wxListEvent& pEvent );
	void BeginEditLabel			( wxListEvent& pEvent );
	void EndEditLabel			( wxListEvent& pEvent );
	void TranslateKeys			( wxListEvent& pEvent );

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
	int  GetType				( long index );
	int  GetItemImage			( long index );
	void RescaleColumns			( );
	void RescanPlaylistDir		( );
	void ShowIcons				( );
	void ToggleIcons			( );
	void UpdateSel				( size_t index );

	//-------------------------//
	//--- playlist routines ---//
	//-------------------------//
	void AddMissing			( wxArrayString playlists );
	void NewPlaylist		( wxString sName, wxString sVal, int nType );
	bool PlaylistExists		( wxString sName, int type = MUSIK_SOURCES_PLAYLIST_STANDARD );
	void FilenameToSources	( wxString* sFilename );
	void SourcesToFilename	( wxString* sSources, int type = MUSIK_SOURCES_PLAYLIST_STANDARD );
	int  FindInSources		( wxString sName, int nType );

	//---standard / dynamic ---//
	bool CreateStdPlaylist			( wxString sName, wxString sSongs );
	bool CreateDynPlaylist			( wxString sName );
    void AppendStdPlaylist			( wxString sName, wxString sSongs );
	void RewriteStdPlaylist			();
	void UpdateDynPlaylist			( int nIndex );
	wxArrayString LoadStdPlaylist	( wxString sName );
	wxString LoadDynPlaylist		( wxString sName );
	bool PlaylistToFile				( wxString sName, wxString* sItems, int type, bool bDelOld = true );
	wxString PromptDynamicPlaylist	( wxString sQuery );
	wxString GetPlaylistName		( int nIndex );

	DECLARE_EVENT_TABLE()

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
	wxString	m_SourcesEditStr;
	int			m_SourcesEditIndex;
	int			m_CurSel;
	int			m_DragIndex;
};

class CSourcesBox : public wxPanel
{
public:
	CSourcesBox( wxWindow *parent, wxWindowID id );
	~CSourcesBox();

	//-------------------------------------------------------------//
	//--- abastraction, so other controls can use g_SourcesCtrl ---//
	//--- only place needed functions here to reduce overhead	---//
	//-------------------------------------------------------------//
	int  GetSelType				( )	{ return pListBox->GetSelType();	}
	void RescaleColumns			( ) { pListBox->RescaleColumns();		}
	void RescanPlaylistDir		( ) { pListBox->RescanPlaylistDir();	}
	void Update					( ) { pListBox->Update();				}

	wxString PromptDynamicPlaylist( wxString sQuery )	{ return pListBox->PromptDynamicPlaylist( sQuery ); }

private:
	CListHeader		*pHeader;
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
