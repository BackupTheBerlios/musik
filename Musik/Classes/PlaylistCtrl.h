/*
 *  PlaylistCtrl.h
 *
 *  A control that acts as a playlist.
 *  Inherited from a wxListCtrl
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef PLAYLISTCTRL_H
#define PLAYLISTCTRL_H

//--- definition CMusikSongArray is here ---//
#include "MusikLibrary.h"

//--- wx ---//
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
#include "wx/listctrl.h"
#include <wx/dnd.h>

class MusikPlaylistRenameThread;
class MusikPlaylistRetagThread;

class CPlaylistCtrl : public wxListCtrl
{
public:
	//--------------------------------//
	//--- constructor / destructor ---//
	//--------------------------------//
	CPlaylistCtrl( wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size );
	~CPlaylistCtrl();

	//--------------//
	//--- events ---//
	//--------------//
	//--- deleting ---//
	void OnDelSel		( wxCommandEvent& WXUNUSED(event) )	{ DelSelSongs();	}
	void OnDelFiles		( wxCommandEvent& WXUNUSED(event) )	{ DelSelFiles();	}
	void OnDelFilesDB	( wxCommandEvent& WXUNUSED(event) )	{ DelSelFilesDB();	}
	void OnRenameFiles	( wxCommandEvent& WXUNUSED(event) )	{ RenameSelFiles();	}
	void OnRetagFiles	( wxCommandEvent& WXUNUSED(event) ) { RetagSelFiles();	}
	//--- rating ---//
	void UnrateSel		( wxCommandEvent& WXUNUSED(event) )	{ RateSel( 0 ); }
	void Rate1Sel		( wxCommandEvent& WXUNUSED(event) )	{ RateSel( 1 ); }
	void Rate2Sel		( wxCommandEvent& WXUNUSED(event) )	{ RateSel( 2 ); }
	void Rate3Sel		( wxCommandEvent& WXUNUSED(event) )	{ RateSel( 3 ); }
	void Rate4Sel		( wxCommandEvent& WXUNUSED(event) )	{ RateSel( 4 ); }
	void Rate5Sel		( wxCommandEvent& WXUNUSED(event) )	{ RateSel( 5 ); }
	//--- tagging ---//
	void ClickTitle		( wxCommandEvent& WXUNUSED(event) )	{ EditTag( 0 );	}
	void ClickTrackNum	( wxCommandEvent& WXUNUSED(event) )	{ EditTag( 1 );	}
	void ClickArtist	( wxCommandEvent& WXUNUSED(event) )	{ EditTag( 2 );	}
	void ClickAlbum		( wxCommandEvent& WXUNUSED(event) )	{ EditTag( 3 );	}
	void ClickGenre		( wxCommandEvent& WXUNUSED(event) )	{ EditTag( 4 ); }
	void ClickYear		( wxCommandEvent& WXUNUSED(event) )	{ EditTag( 5 ); }
	//--- other ---//
	void UpdateSel		( wxListEvent&		event			);
	void ShowMenu		( wxCommandEvent&	WXUNUSED(event) );
	void BeginDrag		( wxEvent&			WXUNUSED(event)	);
	void TranslateKeys	( wxKeyEvent&		pEvent			);
	void BeginDragCol	( wxListEvent&		event			);
	void EndDragCol		( wxListEvent&		event			);
	void PlaySel		( wxListEvent&		WXUNUSED(event)	);

	//------------------------//
	//--- threading events ---//
	//------------------------//
	void OnThreadStart	( wxCommandEvent& WXUNUSED(event) );
	void OnThreadEnd		( wxCommandEvent& WXUNUSED(event) );
	void OnThreadProg		( wxCommandEvent& WXUNUSED(event) );

	//------------//
	//--- gets ---//
	//------------//
	int				GetIndex		() { return nCurSel; }
	wxString		GetSubitemText	( int nItem, int Subitem );
	wxString		GetAllFiles		();
	wxString		GetSelFiles		();
	wxArrayInt		GetSelItems		();
	wxArrayString	GetSelFilesList	();
	wxArrayString	GetAllFilesList ();
	CMusikSongArray GetSelSongs		();
	wxString		GetFilename		( int nItem );
	int				GetTotalPlayingTimeInSeconds();
	wxString		GetTotalFilesize();
	
	//------------//
	//--- sets ---//
	//------------//
	void			SetSelFirst			();
	void			SetIndex			( int n ){ nCurSel = n; }

	//--------------//
	//--- others ---//
	//--------------//
	void ResynchItem		( int item, int lastitem = -1, bool refreshonly = true );
	void Update				( bool bSelFirstItem = true );
	void RescaleColumns		( );
	void ResetColumns		( bool update = false );
	void RateSel			( int nVal );
	void EditTag			( int i );
	void DelSelSongs		( );
	void DelSelFiles		( );
	void DelSelFilesDB		( );
	void RenameSelFiles		( );
	void RetagSelFiles		( );
	bool ViewDirtyTags		( );
	void ToggleIcons		( );
	void ShowIcons			( );
	void SaveColumns		( );
	void FindColumnOrder	( );

	//---------------------------------------------//
	//---          drag and drop stuff.         ---//
	//--- these get set when dnd is initialized ---//
	//---  and should get cleaned up when done  ---//
	//---------------------------------------------//
	//--- functions ---//
	void DNDSetCurSel	();
	void DNDDelSongs	();
	void DNDInsertItems	( wxString sFilename, int nType );
	int	 DNDGetItemPos	( wxString sFilename );
	bool DNDIsSel		( int nVal );
	void DNDDone		( int nNewPos );

	//-----------------//
	//--- threading ---//
	//-----------------//
	void SetProgress			( int n )				{ m_Progress = n;			}
	void SetProgressType		( int n )				{ m_ProgressType = n;		}
	void SetActiveThread		( wxThread* thread )	{ m_ActiveThread = thread;	}

	int GetProgress				()						{ return m_Progress;		}
	int GetProgressType			()						{ return m_ProgressType;	}
	wxThread* GetActiveThread	()						{ return m_ActiveThread;	}

	//--- vars ---//
	wxArrayInt		aCurSel;
	CMusikSongArray aCurSelSongs;

	//------------------------------------//
	//--- selection index. gets called ---//
	//--- whenever a selection is made ---//
	//------------------------------------//
	int				nCurSel;

	DECLARE_EVENT_TABLE()
private:
	//-------------------------//
	//--- virtual functions ---//
	//-------------------------//
    virtual	wxString		OnGetItemText	(long item, long column) const;
    virtual int				OnGetItemImage	(long item) const;
    virtual	wxListItemAttr*	OnGetItemAttr	(long item) const;

	//-------------------------------//
	//--- colors for the playlist ---//
	//-------------------------------//
    wxListItemAttr m_LightAttr;
	wxListItemAttr m_DarkAttr;
	wxListItemAttr m_SelectedAttr;

	//--------------------//
	//--- menu objects ---//
	//--------------------//
	wxMenu *playlist_context_menu;
	wxMenu *playlist_context_rating_menu;
	wxMenu *playlist_context_edit_tag_menu;
	wxMenu *playlist_context_delete_menu;

	//--------------------//
	//--- column order ---//
	//--------------------//
	wxArrayInt m_ColumnOrder;

	//--------------//
	//--- thread ---//
	//--------------//
	MusikPlaylistRenameThread*	pRenameThread;
	MusikPlaylistRetagThread*	pRetagThread;
	int m_Progress;
	int m_ProgressType;
	wxThread* m_ActiveThread;
};

class PlaylistDropTarget : public wxTextDropTarget
{
public:
	//-------------------//
	//--- constructor ---//
	//-------------------//
	PlaylistDropTarget( CPlaylistCtrl *pPList )	{ g_PlaylistCtrl = pPList;	}

	//-------------------------//
	//--- virtual functions ---//
	//-------------------------//
    virtual bool		 OnDropText(wxCoord x, wxCoord y, const wxString& text);
	virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);

	//-------------------------//
	//--- utility functions ---//
	//-------------------------//
	void HighlightSel( wxPoint );
private:
	CPlaylistCtrl *g_PlaylistCtrl;	//--- pointer to the playlist ---//
	int nLastHit;					//--- last item hit           ---//
	long n;							//--- new pos                 ---//
};

#endif
