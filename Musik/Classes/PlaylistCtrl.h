/*
 *  PlaylistCtrl.h
 *
 *  A control that acts as a playlist.
 *  Inherited from a wxListCtrl
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef PLAYLISTCTRL_H
#define PLAYLISTCTRL_H


//--- wx ---//
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
//--- definition CMusikSongArray is here ---//
#include "MusikLibrary.h"
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
	void OnDelFiles		( wxCommandEvent& WXUNUSED(event) )	{ DelSelSongs(true,true);	}
	void OnDelFilesDB	( wxCommandEvent& WXUNUSED(event) )	{ DelSelSongs(true);	}
	void OnRenameFiles	( wxCommandEvent& WXUNUSED(event) )	{ RenameSelFiles();	}
	void OnRetagFiles	( wxCommandEvent& WXUNUSED(event) ) { RetagSelFiles();	}
	//--- rating ---//
	void OnRateSel		( wxCommandEvent& event );
	void OnUpdateUIRateSel ( wxUpdateUIEvent &event);
	//--- tagging ---//
	void OnClickEditTag		( wxCommandEvent& event );
	//--- columns ---//
	void OnDisplayMenu	( wxCommandEvent& event				);
	void OnUpdateUIDisplayMenu ( wxUpdateUIEvent &event);
	void OnDisplaySmart	( wxCommandEvent& WXUNUSED(event)	);
	void OnColumnClick	( wxListEvent& event );
	//--- other ---//
	void UpdateSel		( wxListEvent&		event			);
	void ShowMenu		( wxCommandEvent&	WXUNUSED(event) );
	void BeginDrag		( wxEvent&			WXUNUSED(event)	);
	void TranslateKeys	( wxKeyEvent&		pEvent			);
	void EndDragCol		( wxListEvent&		event			);
	void PlaySel		( wxListEvent&		WXUNUSED(event)	);

	//------------------------//
	//--- threading events ---//
	//------------------------//
	void OnThreadStart	( wxCommandEvent& WXUNUSED(event) );
	void OnThreadEnd	( wxCommandEvent& WXUNUSED(event) );
	void OnThreadProg	( wxCommandEvent& WXUNUSED(event) );

	//------------//
	//--- gets ---//
	//------------//
	int				GetIndex		() { return nCurSel; }
	wxString		GetSubitemText	( int nItem, int Subitem );
	wxString		GetAllFiles		();
	wxString		GetSelFiles		();
	wxArrayInt		GetSelItems		();
	void			GetSelFilesList	( wxArrayString & aResult );
	void			GetAllFilesList ( wxArrayString & aResult );
	void			GetSelSongs		( CMusikSongArray & aResult );
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
	void Update				( bool bSelFirstItem = true,bool  bRescaleColumns = true );
	void RescaleColumns		( bool bFreeze = true, bool bSave = false, bool bAutoFit = false );
	void ResetColumns		( bool update = false, bool rescale = false );
	void RateSel			( int nVal );
	void EditTag			( int i );
	void DelSelSongs		( bool bDeleteFromDB = false, bool bDeleteFromComputer = false );
	void RenameSelFiles		( );
	void RetagSelFiles		( );
	bool ViewDirtyTags		( );
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
	int DisplayEventId2ColumnId(int evid);
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
	wxListItemAttr m_SelectedLightAttr;
	wxListItemAttr m_SelectedDarkAttr;

	//--------------------//
	//--- menu objects ---//
	//--------------------//
	wxMenu *playlist_context_menu;
	wxMenu *playlist_context_rating_menu;
	wxMenu *playlist_context_edit_tag_menu;
	wxMenu *playlist_context_delete_menu;
	wxMenu *playlist_context_display_menu;

	//--------------------//
	//--- column order ---//
	//--------------------//
	wxArrayInt m_ColumnOrder;
	size_t m_Overflow;
	bool m_ColSaveNeeded;

	wxArrayInt m_aColumnSorting;

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
