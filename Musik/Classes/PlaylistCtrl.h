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
#include "../ThreadController.h"

#include "PlaylistInfoCtrl.h"
//--- definition CMusikSongArray is here ---//
#include "MusikLibrary.h"

#include <wx/dnd.h>

#include "MusikListCtrl.h"

class MusikPlaylistRenameThread;
class MusikPlaylistRetagThread;
class CPlaylistBox;

class IPlaylistInfo
{
public:
	virtual int				GetTotalPlayingTimeInSeconds()=0;
	virtual double			GetTotalFilesize()=0;
	virtual int				GetCount()=0;

};
class CPlaylistCtrl : public CMusikListCtrl,public IPlaylistInfo
{
public:
	//--------------------------------//
	//--- constructor / destructor ---//
	//--------------------------------//
	CPlaylistCtrl( CPlaylistBox *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size );
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
	void OnDisplayFit	( wxCommandEvent& WXUNUSED(event)	);
	void OnColumnClick	( wxListEvent& event );
	//--- other ---//
	void UpdateSel		( wxListEvent&		event			);
	void ShowMenu		( wxContextMenuEvent&	event );
	void BeginDrag		( wxListEvent&		event	);
	void TranslateKeys	( wxKeyEvent&		pEvent			);
	void EndDragCol		( wxListEvent&		event			);
	void BeginDragCol	( wxListEvent&		event			);
	void PlaySel		( wxListEvent&		WXUNUSED(event)	);


	bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
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
	void			GetSelItems		(wxArrayInt & aResult);
	void			GetSelFilesList	( wxArrayString & aResult );
	void			GetAllFilesList ( wxArrayString & aResult );
	void			GetSelSongs		( CMusikSongArray & aResult );
	wxString		GetFilename		( int nItem );
	CMusikSongArray * GetPlaylist	();
//IPlaylistInfo
	int				GetTotalPlayingTimeInSeconds();
	double			GetTotalFilesize();
	int				GetCount() { return GetItemCount();}
	//------------//
	//--- sets ---//
	//------------//
	
	void			SetIndex			( int n ){ nCurSel = n; }

	//--------------//
	//--- others ---//
	//--------------//
	void ResynchItem		( int item, int lastitem = -1, bool refreshonly = true );
	void ResynchItem		( int item, const CMusikSong & song);
	void Update				( bool bSelFirstItem = true);
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

	int GetProgress				()						{ return m_Progress;		}
	int GetProgressType			()						{ return m_ProgressType;	}

	//--- vars ---//
	wxArrayInt		aCurSel;
	CMusikSongArray aCurSelSongs;

	//------------------------------------//
	//--- selection index. gets called ---//
	//--- whenever a selection is made ---//
	//------------------------------------//
	int				nCurSel;

	DECLARE_EVENT_TABLE()
protected:
	virtual bool OnRescaleColumns() {RescaleColumns(false); return true;}
	void RescaleColumns		( bool bFreeze = true, bool bSave = false, bool bAutoFit = false );


private:

	CPlaylistBox *m_pParent;

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

	bool m_bColDragging;
	wxArrayInt m_aColumnSorting;

	//--------------//
	//--- thread ---//
	//--------------//
	
	CThreadController m_ActiveThreadController;
	int m_Progress;
	int m_ProgressType;

};

class wxDataObjectCompositeEx : public wxDataObjectComposite
{
public:
	wxDataObjectCompositeEx()
	{
		m_dataObjectLast = NULL;
	}

	bool SetData(const wxDataFormat& format,
								size_t len,
							const void *buf)
	{
		m_dataObjectLast = GetObject(format);

		wxCHECK_MSG( m_dataObjectLast, FALSE,
                 wxT("unsupported format in wxDataObjectCompositeEx"));

		return m_dataObjectLast->SetData(len, buf);
	}

	wxDataObjectSimple *GetActualDataObject()
	{
		return m_dataObjectLast;
	}
	private:
		wxDataObjectSimple *m_dataObjectLast;
};



class PlaylistDropTarget : public wxDropTarget
{
public:
	//-------------------//
	//--- constructor ---//
	//-------------------//
	PlaylistDropTarget( CPlaylistCtrl *pPList )	
	{ 
		m_pPlaylistCtrl = pPList;	

		wxDataObjectCompositeEx * dobj = new wxDataObjectCompositeEx;
		dobj->Add(m_pTextDObj = new wxTextDataObject(),true);
		dobj->Add(m_pFileDObj = new wxFileDataObject());
		SetDataObject(dobj);
    }
	wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);
	//-------------------------//
	//--- virtual functions ---//
	//-------------------------//
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);

    virtual bool		 OnDropText(wxCoord x, wxCoord y, const wxString& text);
	virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);

	//-------------------------//
	//--- utility functions ---//
	//-------------------------//
	void HighlightSel( wxPoint );
private:
	wxTextDataObject * m_pTextDObj;
	wxFileDataObject * m_pFileDObj;

	CPlaylistCtrl *m_pPlaylistCtrl;	//--- pointer to the playlist ---//
	int nLastHit;					//--- last item hit           ---//
	long n;							//--- new pos                 ---//
};

class CPlaylistBox : public wxPanel
{
public:
	CPlaylistBox( wxWindow *parent );
	~CPlaylistBox();
	void ShowPlaylistInfo();
	CPlaylistInfoCtrl & PlaylistInfoCtrl()	{return *m_pPlaylistInfoCtrl;}
	CPlaylistCtrl & PlaylistCtrl()			{return *m_pPlaylistCtrl;}
	wxTextCtrl    & TextSimpleQuery()		{return *m_pTextSimpleQuery;}
	void Update( bool bSelFirstItem = true);
private:
	wxBoxSizer			*m_pMainSizer;
	wxBoxSizer			*m_pHorzSizer;
	CPlaylistInfoCtrl	*m_pPlaylistInfoCtrl;
	CPlaylistCtrl		*m_pPlaylistCtrl;
	wxTextCtrl			*m_pTextSimpleQuery;
};

#endif
