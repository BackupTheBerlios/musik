/*
 *  PlaylistCtrl.cpp
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

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "wx/file.h"

#include "PlaylistCtrl.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

//--- frames ---//
#include "../Frames/MusikFrame.h"
#include "../Frames/MusikTagFrame.h"
#include "../Frames/MusikAutoTaggerFrame.h"
//--- threads ---//
#include "../Threads/PlaylistCtrlThreads.h"

// other
#include "../DataObjectCompositeEx.h"
#include "../DNDHelper.h"


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
		dobj->Add(m_pSonglistDObj = new CMusikSonglistDataObject(),true);
		dobj->Add(m_pFileDObj = new wxFileDataObject());
		SetDataObject(dobj);
	}
	virtual wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult def);
	//-------------------------//
	//--- virtual functions ---//
	//-------------------------//
	virtual bool		 OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);

	virtual bool		 OnDropSonglist(wxCoord x, wxCoord y, const wxString& text);
	virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);

	//-------------------------//
	//--- utility functions ---//
	//-------------------------//
	bool  HighlightSel( const  wxPoint &pPos );
private:
	wxTextDataObject * m_pSonglistDObj;
	wxFileDataObject * m_pFileDObj;

	CPlaylistCtrl *m_pPlaylistCtrl;	//--- pointer to the playlist ---//
	int nLastHit;					//--- last item hit           ---//
	long n;							//--- new pos                 ---//
};

CPlaylistBox::CPlaylistBox( wxWindow *parent )
	: wxPanel( parent, -1, wxPoint( -1, -1 ), wxSize( -1, -1 ),  wxTAB_TRAVERSAL|wxCLIP_CHILDREN|wxSUNKEN_BORDER )
{
	SetBackgroundColour( *wxTheColourDatabase->FindColour(wxT("LIGHT STEEL BLUE")));

	//--- CSourcesListBox ---//
	m_pPlaylistCtrl	= new CPlaylistCtrl( this, MUSIK_PLAYLIST, wxPoint( -1, -1 ), wxSize( -1, -1 ) );
	m_pPlaylistInfoCtrl = new CPlaylistInfoCtrl( this ,  m_pPlaylistCtrl);
	//--------------------//
	//--- simple query ---//
	//--------------------//
	wxBoxSizer *pSimpleQuerySizer = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText *stSearch	= new wxStaticText( this, -1, _( "Search:" ),wxPoint( -1, -1 ), wxSize( -1, -1 ), wxALIGN_RIGHT | wxTRANSPARENT_WINDOW );
	m_pTextSimpleQuery		= new wxTextCtrl( this, MUSIK_SIMPLEQUERY, wxT( "" ), wxPoint( -1, -1 ), wxSize( -1, -1 ), wxSIMPLE_BORDER );
	pSimpleQuerySizer->Add( stSearch,	0, wxRIGHT | wxADJUST_MINSIZE | wxALIGN_CENTER_VERTICAL, 2 );
	pSimpleQuerySizer->Add( m_pTextSimpleQuery, 1, wxADJUST_MINSIZE );

	m_pHorzSizer = new wxBoxSizer( wxHORIZONTAL );
	m_pHorzSizer->Add( m_pPlaylistInfoCtrl, 4, wxADJUST_MINSIZE|wxALIGN_CENTER_VERTICAL  );
	m_pHorzSizer->Add( 5,-1 );

	m_pHorzSizer->Add( pSimpleQuerySizer, 1, wxADJUST_MINSIZE|wxALIGN_CENTER_VERTICAL );
	//--- top sizer ---//
	m_pMainSizer = new wxBoxSizer( wxVERTICAL );
	m_pMainSizer->Add( m_pHorzSizer, 0, wxADJUST_MINSIZE|wxEXPAND|wxRIGHT|wxLEFT|wxBOTTOM|wxTOP  , 2 );
	m_pMainSizer->Add(  m_pPlaylistCtrl, 1, wxEXPAND|wxRIGHT|wxLEFT|wxBOTTOM,5);
	SetSizerAndFit( m_pMainSizer );

	Layout();
}
void CPlaylistBox::ShowPlaylistInfo()
{
    if( g_Prefs.bShowPLInfo )
       m_pPlaylistInfoCtrl->Update();
	m_pHorzSizer->Show( m_pPlaylistInfoCtrl, ( bool )g_Prefs.bShowPLInfo );
	Layout();
}
void CPlaylistBox::Update( bool bSelFirstItem )
{
	m_pPlaylistCtrl->Update(bSelFirstItem);
	if ( g_Prefs.bShowPLInfo )
	{
		m_pPlaylistInfoCtrl->Update();
	}
	Layout();

}
CPlaylistBox::~CPlaylistBox()
{
	
}


BEGIN_EVENT_TABLE(CPlaylistCtrl, CMusikListCtrl)
	EVT_LIST_ITEM_ACTIVATED		( -1,														CPlaylistCtrl::PlaySel				)	
	EVT_LIST_BEGIN_DRAG			( -1,														CPlaylistCtrl::BeginDrag			)
	EVT_LIST_ITEM_SELECTED		( -1,														CPlaylistCtrl::UpdateSel			)
	EVT_LIST_COL_BEGIN_DRAG		( -1,														CPlaylistCtrl::BeginDragCol			)
	EVT_LIST_COL_END_DRAG		( -1,														CPlaylistCtrl::EndDragCol			)
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_PLAY_INSTANTLY,								CPlaylistCtrl::OnPlayInstantly		)
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_PLAY_ASNEXT,									CPlaylistCtrl::OnPlayAsNext			)
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_PLAY_ENQUEUED,									CPlaylistCtrl::OnPlayEnqueued		)
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_PLAY_REPLACE_PLAYERLIST,						CPlaylistCtrl::OnPlayReplace		)
	EVT_MENU					( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FROM_PLAYLIST,					CPlaylistCtrl::OnDelSel				)
	EVT_MENU					( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FILES,							CPlaylistCtrl::OnDelFiles			)
	EVT_MENU					( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FROM_DB,							CPlaylistCtrl::OnDelFilesDB			)
	EVT_UPDATE_UI_RANGE			( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FROM_PLAYLIST, MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FROM_DB,	CPlaylistCtrl::OnUpdateUIDelete	)
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_RENAME_FILES,									CPlaylistCtrl::OnRenameFiles		)
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_RETAG_FILES,									CPlaylistCtrl::OnRetagFiles			)
	EVT_MENU_RANGE				( MUSIK_PLAYLIST_CONTEXT_UNRATED, MUSIK_PLAYLIST_CONTEXT_RATE5,			CPlaylistCtrl::OnRateSel			) 	
	EVT_UPDATE_UI_RANGE			( MUSIK_PLAYLIST_CONTEXT_UNRATED, MUSIK_PLAYLIST_CONTEXT_RATE5,			CPlaylistCtrl::OnUpdateUIRateSel	)
	EVT_MENU_RANGE				( MUSIK_PLAYLIST_CONTEXT_TAG_TITLE,	MUSIK_PLAYLIST_CONTEXT_TAG_YEAR,	CPlaylistCtrl::OnClickEditTag		)
	EVT_MENU					( MUSIK_PLAYLIST_DISPLAY_SMART,											CPlaylistCtrl::OnDisplaySmart		)
	EVT_MENU					( MUSIK_PLAYLIST_DISPLAY_FIT,											CPlaylistCtrl::OnDisplayFit			)
	EVT_MENU					( MUSIK_PLAYLIST_CLEARPLAYERLIST,										CPlaylistCtrl::OnClearPlayerlist	)
	EVT_CONTEXT_MENU			(																		CPlaylistCtrl::ShowMenu				)
	EVT_CHAR					(																		CPlaylistCtrl::TranslateKeys		)
	EVT_LIST_COL_CLICK			( -1,																	CPlaylistCtrl::OnColumnClick		)

	//---------------------------------------------------------//
	//--- column on off stuff.								---//
	//-------------  		--------------------------------------------//
	EVT_MENU_RANGE				( MUSIK_PLAYLIST_DISPLAY_FIRST, MUSIK_PLAYLIST_DISPLAY_LAST,	CPlaylistCtrl::OnDisplayMenu				)
	EVT_UPDATE_UI_RANGE		( MUSIK_PLAYLIST_DISPLAY_FIRST, MUSIK_PLAYLIST_DISPLAY_LAST,		CPlaylistCtrl::OnUpdateUIDisplayMenu		)
	//---------------------------------------------------------//
	//--- threading events.. we use EVT_MENU becuase its	---//
	//--- nice and simple, and gets the job done. this may	---//
	//--- become a little prettier later, but it works.		---//
	//---------------------------------------------------------//
    EVT_MENU			( MUSIK_PLAYLIST_THREAD_START,	CPlaylistCtrl::OnThreadStart)
	EVT_MENU			( MUSIK_PLAYLIST_THREAD_END,	CPlaylistCtrl::OnThreadEnd	)
	EVT_MENU			( MUSIK_PLAYLIST_THREAD_PROG,	CPlaylistCtrl::OnThreadProg	)

END_EVENT_TABLE()

//-----------------//
//--- dnd stuff ---//
//-----------------//

wxDragResult PlaylistDropTarget::OnData(wxCoord x, wxCoord y, wxDragResult def)
{
	bool bRes = false;
	if (GetData() )
	{
			
		wxDataObjectSimple *dobj = ((wxDataObjectCompositeEx *)GetDataObject())->GetActualDataObject();
		
		if( dobj == (wxDataObjectSimple *)m_pSonglistDObj )
			bRes = OnDropSonglist(x, y, m_pSonglistDObj->GetText());
		else if( dobj == (wxDataObjectSimple *)m_pFileDObj )
			bRes = OnDropFiles(x, y, m_pFileDObj->GetFilenames());
	}
	g_DragInProg = false;
	return bRes ? def : wxDragNone;
}

bool PlaylistDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
	return m_pPlaylistCtrl->OnDropFiles( x,  y,  filenames);
}
bool PlaylistDropTarget::OnDropSonglist( wxCoord x, wxCoord y, const wxString &sFiles )
{
	//--- make sure we have something ---//
	if ( sFiles != wxT( "" ) )
	{

		//--- where did we land? ---//
		const wxPoint& pt = wxPoint( x, y );
		int nFlags;
		n = m_pPlaylistCtrl->HitTest( pt, nFlags );


		//--- make sure we havn't dragged on a selected item ---//
		if ( m_pPlaylistCtrl->DNDIsSel( n ) )
		{
			m_pPlaylistCtrl->aCurSel.Clear();
			g_DragInProg = false;
			return false;
		}

		//--- first lets make sure user intended to drop files in the playlist ---//
		//--- otherwise we may have songs removed that shouldn't be. that would be bad ---//
		int nLastSelItem = m_pPlaylistCtrl->aCurSel.Last();
		if ( n == -1 || n < nLastSelItem || n > nLastSelItem )
		{
			//--- not dragged over anything, push to bottom ---//
			if ( n == -1 )
			{
				n = m_pPlaylistCtrl->GetItemCount() - 1;
			}
			//--- find where we need to drop ---//
			if ( n == -2 )
			{	//--- error, couldn't locate our position ---//
				wxMessageBox( _( "An internal error has occured.\nCould not find previous item's position.\n\nPlease contact the "MUSIKAPPNAME" development team with this error." ), MUSIKAPPNAME_VERSION, wxOK|wxICON_ERROR );
				g_DragInProg = false;
				return false;
			}
			if(n >= nLastSelItem)
				n++;
			m_pPlaylistCtrl->MovePlaylistEntrys( n ,m_pPlaylistCtrl->aCurSel );
			
		}
	}
	m_pPlaylistCtrl->DNDDone();
	return true;
}

wxDragResult PlaylistDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
	if(def == wxDragNone)
		return wxDragNone;

	//--- calls HighlightSel() to highlight the item the mouse is over ---//
	//m_pPlaylistCtrl->SetFocus();
	const wxPoint& pt = wxPoint( x, y );
	return HighlightSel( pt ) ? def : wxDragNone;
}

bool PlaylistDropTarget::HighlightSel( const  wxPoint & pPos )
{
	//--- gotta set this so stuff doesn't keep updating ---//
	g_DragInProg	= true;
	int nFlags;
	long n = m_pPlaylistCtrl->HitTest( pPos, nFlags );
	long topitem = m_pPlaylistCtrl->GetTopItem();
	long countperpage = m_pPlaylistCtrl->GetCountPerPage();
	if( n == topitem && n > 0)
		m_pPlaylistCtrl->EnsureVisible(n - 1);
	else if((n == topitem + countperpage - 1) &&  (n < m_pPlaylistCtrl->GetItemCount() - 1))
		m_pPlaylistCtrl->EnsureVisible(n + 1);
	//--- highlight what we're over, deselect old ---//
	if ( n != nLastHit && n!= -1 )
	{
		if ( !m_pPlaylistCtrl->DNDIsSel( nLastHit ) )
			m_pPlaylistCtrl->SetItemState( nLastHit, 0, wxLIST_STATE_SELECTED );

		m_pPlaylistCtrl->SetItemState( n, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
	}

	//--- this is a quick way to check if we need to update ---//
	nLastHit = n;
	g_DragInProg	= false;
	return true;
}

//----------------------------//
//--- construct / destruct ---//
//----------------------------//
CPlaylistCtrl::CPlaylistCtrl( CPlaylistBox *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size )
	:	CMusikListCtrl		( parent, id, pos, size,wxNO_BORDER)
	,m_pParent(parent)
{
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ) );

	//--- setup headers ---//
	m_ColSaveNeeded = false;
	ResetColumns();


	//--- setup drop target ---//
	SetDropTarget( new PlaylistDropTarget( this ) );

	//--- not dragging, no selections ---//
	g_DragInProg = false;
	nCurSel = -1;
	m_Overflow = 0;
	m_bColDragging = false;

}

CPlaylistCtrl::~CPlaylistCtrl()
{
}

///////////////////////////////////////////////////////////////////////////////
wxMenu * CPlaylistCtrl::CreateContextMenu()
{

	//--- rating menu ---//
	wxMenu *playlist_context_rating_menu = new wxMenu;
	playlist_context_rating_menu->Append( MUSIK_PLAYLIST_CONTEXT_UNRATED, _( "Unrated" ), wxT( "" ), wxITEM_CHECK );
	playlist_context_rating_menu->Append( MUSIK_PLAYLIST_CONTEXT_RATE1, wxT( "1" ), wxT( "" ), wxITEM_CHECK );
	playlist_context_rating_menu->Append( MUSIK_PLAYLIST_CONTEXT_RATE2, wxT( "2" ), wxT( "" ), wxITEM_CHECK );
	playlist_context_rating_menu->Append( MUSIK_PLAYLIST_CONTEXT_RATE3, wxT( "3" ), wxT( "" ), wxITEM_CHECK );
	playlist_context_rating_menu->Append( MUSIK_PLAYLIST_CONTEXT_RATE4, wxT( "4" ), wxT( "" ), wxITEM_CHECK );
	playlist_context_rating_menu->Append( MUSIK_PLAYLIST_CONTEXT_RATE5, wxT( "5" ), wxT( "" ), wxITEM_CHECK );

	//--- tag edit menu ---//
	wxMenu *playlist_context_edit_tag_menu = new wxMenu;
	playlist_context_edit_tag_menu->Append( MUSIK_PLAYLIST_CONTEXT_TAG_TITLE,		_( "Edit Title\tF2" ) );
	playlist_context_edit_tag_menu->Append( MUSIK_PLAYLIST_CONTEXT_TAG_TRACKNUM,	_( "Edit Track Number\tF3" ) );
	playlist_context_edit_tag_menu->Append( MUSIK_PLAYLIST_CONTEXT_TAG_ARTIST,		_( "Edit Artist\tF4" ) );
	playlist_context_edit_tag_menu->Append( MUSIK_PLAYLIST_CONTEXT_TAG_ALBUM,		_( "Edit Album\tF5" ) );
	playlist_context_edit_tag_menu->Append( MUSIK_PLAYLIST_CONTEXT_TAG_GENRE,		_( "Edit Genre\tF6" ) );
	playlist_context_edit_tag_menu->Append( MUSIK_PLAYLIST_CONTEXT_TAG_YEAR,		_( "Edit Year\tF7" ) );
	playlist_context_edit_tag_menu->AppendSeparator();
	playlist_context_edit_tag_menu->Append( MUSIK_PLAYLIST_CONTEXT_RENAME_FILES,	_( "&Auto Rename" ) );
	playlist_context_edit_tag_menu->Append( MUSIK_PLAYLIST_CONTEXT_RETAG_FILES,		_( "A&uto Retag..." ) );

	//--- delete menu ---//
	wxMenu *playlist_context_delete_menu = new wxMenu;
	playlist_context_delete_menu->Append( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FROM_PLAYLIST,	_( "From Playlist\tDel" ) );
	playlist_context_delete_menu->Append( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FROM_DB,			_( "From Database\tAlt+Del" ) );
	playlist_context_delete_menu->Append( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FILES,			_( "From Computer\tCtrl+Del" ) );

	//--- columns context menu ---//
	wxMenu *playlist_context_display_menu = new wxMenu;
	for(size_t i = 0 ; i < NPLAYLISTCOLUMNS;i ++)
	{
		playlist_context_display_menu->AppendCheckItem( MUSIK_PLAYLIST_DISPLAY_FIRST + i,g_PlaylistColumnLabels[i] );
	}
	playlist_context_display_menu->AppendSeparator();
	playlist_context_display_menu->AppendCheckItem( MUSIK_PLAYLIST_DISPLAY_FIT,				_( "Fit Columns" ) );
	playlist_context_display_menu->AppendCheckItem( MUSIK_PLAYLIST_DISPLAY_SMART,			_( "No Horizontal Scroll" ) );

	bool bNetStreamSel = false;
	if ( GetSelectedItemCount() > 0 )
	{
		int nFirstIndex = GetNextItem( -1, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
		bNetStreamSel  = (g_Playlist.Item ( nFirstIndex ).Format == MUSIK_FORMAT_NETSTREAM);
	}
	bool bIsNowPlayingSelected = (g_SourcesCtrl->GetSelType() == MUSIK_SOURCES_NOW_PLAYING);

	//--- main context menu ---//
	wxMenu *playlist_context_menu = new wxMenu;
	if(!bIsNowPlayingSelected)
	{
		//Play menu
		wxMenu * playlist_context_play_menu = new wxMenu;
		playlist_context_play_menu->Append( MUSIK_PLAYLIST_CONTEXT_PLAY_ASNEXT , _( "Next" ), wxT( "" ) );
		playlist_context_play_menu->Append( MUSIK_PLAYLIST_CONTEXT_PLAY_ENQUEUED, _( "Enqueue" ), wxT( "" ) );
		playlist_context_play_menu->Append( MUSIK_PLAYLIST_CONTEXT_PLAY_INSTANTLY , _( "Instantly" ), wxT( "" ));
		playlist_context_play_menu->Append( MUSIK_PLAYLIST_CONTEXT_PLAY_REPLACE_PLAYERLIST, _( "Replace current playlist" ), wxT( "" ) );

		playlist_context_menu->Append( MUSIK_PLAYLIST_CONTEXT_PLAYNODE,			_( "&Play" ),					playlist_context_play_menu );
	}
	else
		playlist_context_menu->Append( MUSIK_PLAYLIST_CLEARPLAYERLIST,			_( "&Clear List" ),					wxT("") );

	playlist_context_menu->Append( MUSIK_PLAYLIST_CONTEXT_RATENODE,			_( "&Rating" ),					playlist_context_rating_menu );
	playlist_context_menu->Append( MUSIK_PLAYLIST_CONTEXT_DISPLAYNODE,		_( "Display" ),					playlist_context_display_menu );
	playlist_context_menu->AppendSeparator();
	playlist_context_menu->Append( MUSIK_PLAYLIST_CONTEXT_DELETENODE,		_( "F&ile Operations" ),		playlist_context_delete_menu );
	playlist_context_menu->AppendSeparator();
	playlist_context_menu->Append( MUSIK_PLAYLIST_CONTEXT_TAGNODE,			_( "Edit &Tag" ),				playlist_context_edit_tag_menu );

	if(!bIsNowPlayingSelected)
		playlist_context_menu->Enable( MUSIK_PLAYLIST_CONTEXT_PLAYNODE,		!bNetStreamSel );
	//	playlist_context_menu->Enable( MUSIK_PLAYLIST_CONTEXT_DELETENODE,	!bNetStreamSel );
	playlist_context_menu->Enable( MUSIK_PLAYLIST_CONTEXT_RENAME_FILES, !bNetStreamSel );
	playlist_context_menu->Enable( MUSIK_PLAYLIST_CONTEXT_RETAG_FILES,	!bNetStreamSel );
	playlist_context_menu->Enable( MUSIK_PLAYLIST_CONTEXT_RATENODE,		!bNetStreamSel );
	playlist_context_menu->Enable( MUSIK_PLAYLIST_CONTEXT_TAGNODE,		!bNetStreamSel );


	return playlist_context_menu;
}
void CPlaylistCtrl::OnColumnClick( wxListEvent& event )
{
	if(g_SourcesCtrl->GetSelType() != MUSIK_SOURCES_LIBRARY) // only sort in library view, else we will destroy the user playlist
		return;
	int ActualColumn = m_ColumnOrder.Item( event.GetColumn() );
	
	int currentsortorder = m_aColumnSorting.Item( ActualColumn );
	for(int i = 0; i < m_aColumnSorting.GetCount();i++)
    	m_aColumnSorting.Item( i ) = 0;	   // set all columns to unsorted

	m_aColumnSorting.Item( ActualColumn ) = currentsortorder > 0 ? -1 : 1;   // toggle sort order

	bool desc = ( m_aColumnSorting.Item( ActualColumn ) < 0 );

	g_Library.SetSortOrderField( ActualColumn, desc );
	g_Library.RedoLastQuerySongsWhere( g_Playlist ,true);//sorted
	g_PlaylistChanged = true;
	Update();
}

///////////////////////////////////////////////////////////////////////////////

void CPlaylistCtrl::SaveColumns()
{
	//---------------------------------------------------------//
	//--- get the total width of all the columns in pixels.	---//
	//--- this value will be used to calculate dynamic		---//
	//--- columns.											---//
	//---------------------------------------------------------//
	m_ColSaveNeeded = false;
	wxSize client_size = GetClientSize();

	size_t nCurrCol;
	float f_Pos;
	int n_Pos;

	//---------------------------------------------------------//
	//--- sum up all the columns with a static size. the	---//
	//--- remaining size will be used to calculate dynamic	---//
	//--- column percentages.								---//
	//---------------------------------------------------------//
	size_t nStaticWidth = 0;
	for ( size_t i = 0; i < m_ColumnOrder.GetCount(); i++ )
	{
		nCurrCol = m_ColumnOrder.Item( i );

		if ( g_Prefs.bPlaylistColumnDynamic[nCurrCol] == 0 )
		{
			g_Prefs.nPlaylistColumnSize[nCurrCol] = GetColumnWidth( i );
			nStaticWidth += GetColumnWidth( i );
		}
	}

	//---------------------------------------------------------//
	//--- calculate the dynamic size columns. these are		---//
	//--- these are the percent of the window the current	---//
	//--- column takes up.									---//
	//---------------------------------------------------------//
	size_t nRemaining = client_size.GetWidth() - nStaticWidth;
	for ( size_t i = 0; i < m_ColumnOrder.GetCount(); i++ )
	{
		nCurrCol = m_ColumnOrder.Item( i );

		if ( g_Prefs.bPlaylistColumnDynamic[nCurrCol] == 1 )
		{
			f_Pos = (float)GetColumnWidth( i ) / (float)nRemaining * 100.0f;
			n_Pos = MusikRound( f_Pos );

			if ( n_Pos < 1 )
				n_Pos = 1;

			g_Prefs.nPlaylistColumnSize[nCurrCol] = n_Pos;
		}
	}
}

//--------------//
//--- events ---//
//--------------//
void CPlaylistCtrl::ShowMenu( wxContextMenuEvent& WXUNUSED(event) )
{
	wxPoint pos = ScreenToClient( wxGetMousePosition() );

	wxMenu *playlist_context_menu = CreateContextMenu();

	PopupMenu( playlist_context_menu, pos );
	delete playlist_context_menu;
}
int CPlaylistCtrl::DisplayEventId2ColumnId( int evid)
{
	return  evid - MUSIK_PLAYLIST_DISPLAY_FIRST;

}
void CPlaylistCtrl::OnRateSel( wxCommandEvent& event )
{
	RateSel( event.GetId() - MUSIK_PLAYLIST_CONTEXT_UNRATED );
}
void CPlaylistCtrl::OnClickEditTag( wxCommandEvent& event )
{
	EditTag( event.GetId() - MUSIK_PLAYLIST_CONTEXT_TAG_TITLE );
}
void CPlaylistCtrl::OnDisplayMenu( wxCommandEvent& event )
{
	int nColumn = -1;
	nColumn = DisplayEventId2ColumnId(event.GetId());
	if( nColumn > -1)
	{
		g_Prefs.bPlaylistColumnEnable[nColumn] = !g_Prefs.bPlaylistColumnEnable[nColumn];
		ResetColumns( false, true );
	}
}
void CPlaylistCtrl::OnDisplayFit( wxCommandEvent& WXUNUSED(event) )
{
	RescaleColumns( true, false, true );
}
void CPlaylistCtrl::OnClearPlayerlist( wxCommandEvent& WXUNUSED(event) )
{

	g_Playlist.Clear();
	g_Player.SetPlaylist(g_Playlist);
	g_Player.Stop();
	Update(false);
}
void CPlaylistCtrl::OnUpdateUIDisplayMenu ( wxUpdateUIEvent &event)
{
	int nColumn = DisplayEventId2ColumnId(event.GetId());
	if(nColumn > -1)
		event.Check(g_Prefs.bPlaylistColumnEnable[nColumn]);	
}

void CPlaylistCtrl::OnUpdateUIDelete ( wxUpdateUIEvent &event)
{
	bool bEnable = true;
			
	int nFirstIndex = GetNextItem( -1, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
	if(nFirstIndex >= 0 && g_Playlist.Item ( nFirstIndex ).Format == MUSIK_FORMAT_NETSTREAM)
	{
		if((event.GetId() == MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FROM_PLAYLIST) 
			&&(g_SourcesCtrl->GetSelType() == MUSIK_SOURCES_NOW_PLAYING))
			bEnable = true;
		else
			bEnable = false;

	}
	event.Enable(bEnable);
}

void CPlaylistCtrl::OnUpdateUIRateSel ( wxUpdateUIEvent &event)
{
	//--- get rating for first sel ---//
	int item = GetNextItem( -1, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
	
	if(item > -1)
	{
		int nRating = g_Playlist.Item( item ).Rating;
		wxASSERT(nRating >= 0 && nRating <= 5);
		event.Check( event.GetId() == MUSIK_PLAYLIST_CONTEXT_UNRATED + nRating);
	}
}

void CPlaylistCtrl::OnDisplaySmart( wxCommandEvent& WXUNUSED(event) )
{
	g_Prefs.bPlaylistSmartColumns = !g_Prefs.bPlaylistSmartColumns;
	RescaleColumns();
}

void CPlaylistCtrl::BeginDrag( wxListEvent& WXUNUSED(event) )
{
	//--- let program know we're dragging ---//
	g_DragInProg = true;

	//--- pass selected items ---//
	wxString sValidSelFiles = GetSelFiles();
	if(!sValidSelFiles.IsEmpty())
	{
		wxString sDrop (sValidSelFiles);
		DNDSetCurSel();
		  
		//--- initialize drag and drop... SourcesDropTarget / PlaylistDropTarget should take care of the rest ---//
		wxDropSource dragSource( this );
		CMusikSonglistDataObject song_data( sDrop );
		dragSource.SetData( song_data );
		dragSource.DoDragDrop( TRUE );
	}
	//--- tell program we're done dragging ---//
	g_DragInProg = false;
}
void CPlaylistCtrl::BeginDragCol(wxListEvent& WXUNUSED(event))
{
	m_bColDragging = true;
}
void CPlaylistCtrl::EndDragCol( wxListEvent& WXUNUSED(event) )
{
	m_bColDragging = false;
	m_ColSaveNeeded = true;
}


void CPlaylistCtrl::PlaySel( wxListEvent& (event) )
{
	if(g_SourcesCtrl->GetSelType() == MUSIK_SOURCES_NOW_PLAYING)
		g_Player.Play(event.GetIndex());
	else
	OnPlayInstantly(event);
}

void CPlaylistCtrl::TranslateKeys( wxKeyEvent& event )
{	 
	int nKeyCode = event.GetKeyCode();
	if ( event.AltDown() == TRUE )
	{
		switch( nKeyCode )
		{
			case WXK_DELETE:
			case WXK_BACK:
				DelSelSongs(true);// delete songs from db
				break;
			default:
				event.Skip();
				return;
		}
	}
	else if ( event.ControlDown() == TRUE )
	{
		switch( nKeyCode )
		{
		case 1:	//--- a / A ---//
			wxListCtrlSelAll( this );
			break;
		case 4:	//--- d / D ---//
			wxListCtrlSelNone( this );
			break;
		case WXK_DELETE:
		case WXK_BACK:
			DelSelSongs(true,true);// delete from db and computer
			break;
		default:
			event.Skip();
			return;
		}
	}
	else if ( event.AltDown() == FALSE )
	{
		if( GetSelectedItemCount() > 0 )
		{			
			switch( nKeyCode )
			{
				case WXK_F2:
					EditTag( 0 );
					break;
				case WXK_F3:
					EditTag( 1 );
					break;
				case WXK_F4:
					EditTag( 2 );
					break;
				case WXK_F5:
					EditTag( 3 );
					break;
				case WXK_F6:
					EditTag( 4 );
					break;
				case WXK_F7:
					EditTag( 5 );
					break;
				case WXK_DELETE:
				case WXK_BACK:
					DelSelSongs();
					break;
				default:
					event.Skip();
					return;
			}
		}
	}
	else
		event.Skip();   // we did not handle the key, so propagate it further
}

//---------------------------------------------------//
//--- virtual functions. careful with these ones, ---// 
//---        boys. they drive everything          ---//
//---------------------------------------------------//
wxString CPlaylistCtrl::OnGetItemText(long item, long column) const
{
	//-----------------------------------------------------//
	//--- FindColumnOrder() should already have fired	---//
	//--- so go ahead and add the items.				---//
	//-----------------------------------------------------//

	size_t nCurrType = m_ColumnOrder.Item( column );

    const CMusikSong & song = g_Playlist.Item ( item );
	switch ( nCurrType )
	{
	case PLAYLISTCOLUMN_RATING:
		break;

	case PLAYLISTCOLUMN_TRACK:
		if ( song.TrackNum > 0 )
			return wxString::Format( wxT( "%d" ), song.TrackNum );
		else
			return wxT( "-" );
		break;

	case PLAYLISTCOLUMN_TITLE:
		return SanitizedString( song.Title );		
		break;

	case PLAYLISTCOLUMN_ARTIST:
		if ( song.Artist.IsEmpty() )
			return _( "<unknown>" );
		else 
		{
			return SanitizedString( song.Artist );
		}
		break;

	case PLAYLISTCOLUMN_ALBUM:
		if ( song.Album.IsEmpty() )
			return _( "<unknown>" );
		else
			return SanitizedString( song.Album );
		break;

	case PLAYLISTCOLUMN_YEAR:
		if ( song.Year.IsEmpty() )
			return _( "<unknown>" );
		else
			return song.Year;
		break;

	case PLAYLISTCOLUMN_GENRE:
		if ( song.Genre.IsEmpty() )
			return _( "<unknown>" );
		else
			return SanitizedString( song.Genre );
		break;

	case PLAYLISTCOLUMN_TIMES_PLAYED:
		{
			
			return (song.TimesPlayed > 0) ? IntToString(song.TimesPlayed) : wxT("-");
		}
		break;

	case PLAYLISTCOLUMN_LAST_PLAYED:
		if ( song.LastPlayed != 0.0 )
		{
			wxDateTime dt(song.LastPlayed);
			return dt.Format(wxT("%x %X"));
		}
		else
			return _("Never");
		break;

	case PLAYLISTCOLUMN_TIME:
		return MStoStr( song.Duration );
		break;

	case PLAYLISTCOLUMN_BITRATE:
		return IntTowxString( song.Bitrate );
		break;

	case PLAYLISTCOLUMN_FILENAME:
		return song.Filename;
		break;
	case PLAYLISTCOLUMN_NOTES:
		return song.Notes;
	case PLAYLISTCOLUMN_TIMEADDED:
		wxDateTime dt(song.TimeAdded);
		return dt.Format(wxT("%x %X"));
		break;
	}

	return wxT( "" );
}

void CPlaylistCtrl::FindColumnOrder()
{
	m_ColumnOrder.Clear();
	m_aColumnSorting.Clear();
	for ( int i = 0; i < NPLAYLISTCOLUMNS; i++ )
	{
		if ( g_Prefs.bPlaylistColumnEnable[i] == 1 )
		{
			m_ColumnOrder.Add( i );
		}
		m_aColumnSorting.Add( 0 );
	}
}

int CPlaylistCtrl::OnGetItemImage(long item) const
{
	return g_Playlist.Item ( item ).Rating;
}

wxListItemAttr* CPlaylistCtrl::OnGetItemAttr(long item) const
{
	const CMusikSong & song = g_Playlist.Item ( item );
	if(g_Player.IsPlaying() && (g_SourcesCtrl->GetSelType() == MUSIK_SOURCES_NOW_PLAYING) 
		&& (g_Player.GetCurIndex() == item ) && (song.songid == g_Player.GetCurrentSongid()))
	{
		if ( g_Prefs.bPLStripes == 1 )
			return item % 2 ? (wxListItemAttr *)&m_SelectedDarkAttr : (wxListItemAttr *)&m_SelectedLightAttr;
		else
			return (wxListItemAttr *)&m_SelectedLightAttr;
	}
	else if ( g_Player.IsPlaying() && (g_SourcesCtrl->GetSelType() != MUSIK_SOURCES_NOW_PLAYING) 
		&& song.songid == g_Player.GetCurrentSongid() )
	{
		if ( g_Prefs.bPLStripes == 1 )
			return item % 2 ? (wxListItemAttr *)&m_SelectedDarkAttr : (wxListItemAttr *)&m_SelectedLightAttr;
		else
			return (wxListItemAttr *)&m_SelectedLightAttr;
	}
	else
	{
		if ( g_Prefs.bPLStripes == 1 )
			return item % 2 ? (wxListItemAttr *)&m_DarkAttr : (wxListItemAttr *)&m_LightAttr;
		else
			return (wxListItemAttr *)&m_LightAttr;
	}
}

//--------------------------------------------------//
//--- functions to get information from the list ---//
//--------------------------------------------------//
wxString CPlaylistCtrl::GetSubitemText( int nItem, int nSubitem )
{
	wxListItem iInfo;
	iInfo.SetMask		( wxLIST_MASK_TEXT );
	iInfo.SetId			( nItem );
	iInfo.SetColumn		( nSubitem );
	GetItem				( iInfo );
	return				( iInfo.GetText() );
}

wxString CPlaylistCtrl::GetAllFiles()
{
	wxString sResult;
	sResult.Alloc( GetItemCount() * 50 ); // optimization ( the 50 is a wild guess)
	for ( int i = 0; i < GetItemCount(); i++ )
	{
		sResult +=  GetFilename( i );
		sResult += '\n';
	}
	//--- we don't need the last \n ---//
	return sResult.Truncate( sResult.Length() - 1 );	
}

wxString CPlaylistCtrl::GetSelFiles()
{
	wxString sResult;
	int nIndex = -1;
	int itemcount = GetSelectedItemCount();
	sResult.Alloc(itemcount * 50); // 50 is just a for the average file name length
	for ( int i = 0; i < itemcount; i++ )
	{
		nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
		if( i == -1)
			break;
		if(g_Playlist.Item( nIndex ).Format != MUSIK_FORMAT_NETSTREAM)
		{
			sResult += GetFilename( nIndex );
			sResult+= wxT( "\n" ); // only add \n if it is not the last name
		}
	}

	//--- we don't need the last \n ---//
	return sResult.IsEmpty() ? sResult : sResult.Truncate( sResult.Length() - 1 );	
}

void  CPlaylistCtrl::GetSelItems(wxArrayInt & aResult)
{
	aResult.Clear();
	int nIndex = -1;
	for ( int i = 0; i < GetSelectedItemCount(); i++ )
	{
		nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
		if ( nIndex == -1 )
			break;
		aResult.Add( nIndex );
	}
	return;	
}

void CPlaylistCtrl::GetSelFilesList( wxArrayString & aResult )
{
	aResult.Clear();
	int nIndex = -1;
	for ( int i = 0; i < GetSelectedItemCount(); i++ )
	{
		nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
		if ( nIndex == -1 )
			break;
		aResult.Add( GetFilename( nIndex ) );
	}

	return;
}

void CPlaylistCtrl::GetAllFilesList(wxArrayString & aResult )
{
	aResult.Clear();
	aResult.Alloc(GetItemCount());
	for ( int i = 0; i < GetItemCount(); i++ )
	{
		aResult.Add( GetFilename( i ) );
	}

	return;
}

void CPlaylistCtrl::GetSelectedSongs(CMusikSongArray & aResult)
{
	aResult.Clear();
	int nIndex = -1;
	
	if( GetSelectedItemCount() > 0 )
	{
		for( int i = 0; i < GetSelectedItemCount(); i++ )
		{
			nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
			if ( nIndex == -1 )
				break;
			aResult.Add( g_Playlist.Item ( nIndex ) ); 
			
		}
	}
	return;
}

double CPlaylistCtrl::GetTotalFilesize()
{
	double filesize = 0.0;
	for ( int i = 0; i < g_Playlist.GetCount(); i++ )
	{
		const CMusikSong &song = g_Playlist.Item( i );
		filesize += song.Filesize;
	}

	return filesize;
}

int CPlaylistCtrl::GetTotalPlayingTimeInSeconds()
{
	int Duration = 0;
	for ( int i = 0; i < GetItemCount(); i++ )
	{
		const CMusikSong & song = g_Playlist.Item ( i );
		Duration += song.Duration/1000;
	}
	return Duration;
}

wxString  CPlaylistCtrl::GetFilename( int nItem )
{
	if ( nItem > -1 )
	{
		const CMusikSong & song = g_Playlist.Item ( nItem );
		return song.Filename;
	}

	//--- not found? return a null string ---//
	return wxString();
}



//----------------------------------------//
//--- various other functions we need. ---//
//----------------------------------------//
void CPlaylistCtrl::ShowIcons()
{
	if ( g_Prefs.bPlaylistColumnEnable[PLAYLISTCOLUMN_RATING] == 0 )
		SetImageList( g_NullImageList, wxIMAGE_LIST_SMALL );
	else
		SetImageList( g_RatingImages, wxIMAGE_LIST_SMALL );
}

void CPlaylistCtrl::UpdateSel( wxListEvent& event )
{
	if ( !g_DragInProg ) 
		nCurSel = event.GetIndex();
}

void CPlaylistCtrl::ResynchItem( int item, int lastitem, bool refreshonly )
{
	if ( !refreshonly )
	{
		int songid = g_Playlist.Item( item ).songid;
		g_Library.GetSongFromSongid( songid, &g_Playlist.Item( item ) );
		
		if ( lastitem > -1 && lastitem != item )
		{
			int songid = g_Playlist.Item( lastitem ).songid;
			g_Library.GetSongFromSongid( songid, &g_Playlist.Item(lastitem ) );		
		}			
	}

	if(lastitem == -1)
		 RefreshItem( item );
	else
		Refresh();
}
void CPlaylistCtrl::ResynchItem( int item, const CMusikSong & song)
{
	if(g_Playlist.GetCount() && (item < g_Playlist.GetCount()) && (g_SourcesCtrl->GetSelType() == MUSIK_SOURCES_NOW_PLAYING))
	{
		g_Playlist.Item( item ) = song;
		RefreshItem( item );
	}
}

void CPlaylistCtrl::Update( bool bSelFirst)
{
	//----------------------------------------------------------------------------------//
	//---         note that the playlist control is now virtual, so we don't         ---//
	//---    add items directly to it.  Instead, we have MusikFrame::g_Playlist, a   ---//
	//---  CMusikSongArray, that the virtual listctrl then references via callbacks  ---//
	//----------------------------------------------------------------------------------//

	//--- setup listbox colours from prefs	---//
	m_LightAttr			= wxListItemAttr( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ), wxNullFont );
	m_SelectedLightAttr	= wxListItemAttr( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ), g_fntListBold );
	m_DarkAttr			= wxListItemAttr( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT), StringToColour( g_Prefs.sPLStripeColour ), wxNullFont );
	m_SelectedDarkAttr	= wxListItemAttr( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT), StringToColour( g_Prefs.sPLStripeColour ), g_fntListBold );

	//--- SetItemCount() kinda tells the virtual list control to udpate ---//
	// no Freeze() here , because RescaleColumns(); will not work correctly then
	SetItemCount( ( long )g_Playlist.GetCount() );
	RescaleColumns();

	wxListCtrlSelNone( this );

	//--- sel first item, if we're supposed to ---//
	if ( bSelFirst && GetItemCount() )
		SetItemState( 0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );	
	
}

void CPlaylistCtrl::RescaleColumns( bool bFreeze, bool WXUNUSED(bSave), bool bAutoFit )
{
	if ( g_DisablePlacement || m_bColDragging)
		return;

	static bool bInRescaleColumns = false;
	if(bInRescaleColumns)
		return;
	bInRescaleColumns = true;
	if ( m_ColSaveNeeded && !bAutoFit )
		SaveColumns();

	if ( bFreeze )
		Freeze();

	//-------------------------------------------------//
	//--- size of the client area.					---//
	//-------------------------------------------------//
	wxSize client_size		= GetClientSize();
	size_t nTotalPercent	= 0;
	size_t nRemainingWidth	= 0;
	size_t nStaticWidth		= 0;
	size_t nDynamicWidth	= 0;
	size_t nCurrItem;
	
	//-------------------------------------------------//
	//--- if gtk, add width of scrollbar to static	---//
	//-------------------------------------------------//
	#ifdef __WXGTK__
		nStaticWidth += wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y);	
	#endif 	

	//-------------------------------------------------//
    //--- find the size of all the static members	---//
	//-------------------------------------------------//
	for ( size_t i = 0; i < m_ColumnOrder.GetCount(); i++ )
	{
		nCurrItem = m_ColumnOrder.Item( i );
		if ( g_Prefs.bPlaylistColumnDynamic[nCurrItem] == 0 )
			nStaticWidth += g_Prefs.nPlaylistColumnSize[nCurrItem];
	}

	//-------------------------------------------------//
	//--- if using smart columns, we need to find	---//
	//--- what percentages mean.					---//
	//-------------------------------------------------//
	if ( g_Prefs.bPlaylistSmartColumns == 1 || bAutoFit )
	{
		for ( size_t i = 0; i < m_ColumnOrder.GetCount(); i++ )
		{
			nCurrItem = m_ColumnOrder.Item( i );
			if ( g_Prefs.bPlaylistColumnDynamic[nCurrItem] == 1 )
				nTotalPercent += g_Prefs.nPlaylistColumnSize[nCurrItem];
		}

		if ( nTotalPercent == 0 )
			nTotalPercent = 1;
	}

	//-------------------------------------------------//
	//--- remaining width = window width - static	---//
	//--- width. (this is width for dynamic items)	---//		
	//-------------------------------------------------//
	nRemainingWidth = client_size.GetWidth() - nStaticWidth;

	//-------------------------------------------------//
	//--- go in and set all the column widths, both	---//
	//--- static and dynamic.						---//
	//-------------------------------------------------//
	float f_Per;
	int n_Per;
	int n_LastDyn = -1;
	for ( size_t i = 0; i < m_ColumnOrder.GetCount(); i++ )
	{
		nCurrItem = m_ColumnOrder.Item( i );

		//-------------------------//
		//--- set static size	---//
		//-------------------------//
		if ( g_Prefs.bPlaylistColumnDynamic[nCurrItem] == 0 )
			SetColumnWidth( i, g_Prefs.nPlaylistColumnSize[nCurrItem] );

		//-------------------------//
		//--- set dynamic size	---//
		//-------------------------//
		else
		{
			n_LastDyn = i;

			if ( g_Prefs.bPlaylistSmartColumns == 1 || bAutoFit )
			{
				f_Per = ( (float)g_Prefs.nPlaylistColumnSize[nCurrItem] / (float)nTotalPercent ) * nRemainingWidth;
				n_Per = (int)f_Per;
			}

			else
			{
				f_Per = ( (float)g_Prefs.nPlaylistColumnSize[nCurrItem] / 100.0f ) * nRemainingWidth;
				n_Per = (int)f_Per;		
			}

			nDynamicWidth += (int)n_Per;
			SetColumnWidth( i, n_Per );
		}
	}
	
	//-------------------------------------------------//
	//--- remaining pixels, that may have been		---//
	//--- lost by integer division.					---//
	//-------------------------------------------------//
	if ( ( g_Prefs.bPlaylistSmartColumns == 1 || bAutoFit ) && nTotalPercent && nDynamicWidth )
	{
		m_Overflow = client_size.GetWidth() - ( nStaticWidth + nDynamicWidth );
		size_t nLastSize = GetColumnWidth( n_LastDyn ) + m_Overflow;
		SetColumnWidth( n_LastDyn, nLastSize );
	}

	//-------------------------------------------------//
	//--- make sure window is properly refreshed.	---//
	//-------------------------------------------------//
	if ( bFreeze )
	{
		Thaw();
		wxWindow::Update(); // instantly update window content
	}

	if ( bAutoFit )
		SaveColumns();
	bInRescaleColumns = false;
}

void CPlaylistCtrl::ResetColumns( bool update, bool rescale )
{
	//-------------------------------------------------//
	//--- clear all columns							---//
	//-------------------------------------------------//
	size_t nColumnCount = GetColumnCount();
	for ( size_t i = 0; i < nColumnCount; i++ )
		DeleteColumn( 0 );

	FindColumnOrder();

	//-------------------------------------------------//
	//--- construct all columns, and set any static	---//
	//--- values. RescaleColumns() will be called	---//
	//--- afterwords to setup any dynamic columns.	---//
	//-------------------------------------------------//
	size_t nCurrType;
	for ( size_t i = 0; i < m_ColumnOrder.GetCount(); i++ )
	{
		nCurrType = m_ColumnOrder.Item( i );
		InsertColumn( i, g_PlaylistColumnLabels[nCurrType], g_PlaylistColumnAlign[nCurrType], 50 );
	}

	ShowIcons();

	if ( rescale )
		RescaleColumns();

	if ( update )
		Update();
}

void CPlaylistCtrl::RateSel( int nVal )
{
	int nIndex = -1;
	//--- yeah, we can rate multiple items. find whats selected ---//
	g_Library.BeginTransaction();
	for ( int i = 0; i < GetSelectedItemCount(); i++ )
	{
		nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
		if ( nIndex == -1 )
			break;
		//--- set db entry, then resync item(s) ---//
		g_Library.SetRating( g_Playlist.Item ( nIndex ).songid, nVal );
		g_Playlist.Item( nIndex ).Rating = nVal;
		RefreshItem( nIndex );
		
	}
	g_Library.EndTransaction();
}

void CPlaylistCtrl::EditTag( int i )
{
	int nSelCount = GetSelectedItemCount();
	g_Playlist;
	int nEditType = 0;
	int nIndex = -1;

	//--- if there are less than 2 songs selected, setup for single edit mode ---//
	if ( nSelCount < 2 )
	{
		nEditType = MUSIK_TAG_SINGLE;
		nIndex = GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	}

	//--- more than 1 song, setup for batch edit mode ---//
	else if ( nSelCount > 1)
	{
		nEditType = MUSIK_TAG_MULTIPLE;
	}

	//--- call the tag dialog, disable this window. ---//
	//---  tag dialog will re-enable when complete  ---//
	if ( nEditType != 0 )
	{
		MusikTagFrame* pMusikTagFrame = new MusikTagFrame( g_MusikFrame, this, i, nEditType, nIndex );
		g_MusikFrame->Enable( FALSE );
		pMusikTagFrame->Show();
	}
}




void CPlaylistCtrl::DelSelSongs(bool bDeleteFromDB, bool bDeleteFromComputer)
{
	int nSelCount = GetSelectedItemCount();
	if( nSelCount == 0)
		return;
	if( bDeleteFromComputer )
	{
		wxMessageDialog confirm( this, _( "Delete the selected songs from your computer?" ), MUSIKAPPNAME_VERSION, wxYES_NO | wxICON_STOP );
		if ( confirm.ShowModal() == wxID_NO )
			return;
	}
	else if( bDeleteFromDB )
	{
		wxMessageDialog confirm( this, _( "Delete the selected songs from Musik's internal database?" ), MUSIKAPPNAME_VERSION, wxYES_NO | wxICON_QUESTION );
		if ( confirm.ShowModal() == wxID_NO )
			return;
	}
	else if( MUSIK_SOURCES_LIBRARY == g_SourcesCtrl->GetSelType() )
	{// we do not want to delete entry from the library playlist ( it would have no effect if library is reselected)
		return;// ignore
	}

	wxString sError;
	Freeze();
	int nIndex = -1;
	

	//--- find which songs are selected, delete as we go along ---//
	wxString sFile;
	nIndex = -1;
	int nFirstSel = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
	int nDeletedSongs = 0;
	bool bSourceNowPlayingSelected = (g_SourcesCtrl->GetSelType() == MUSIK_SOURCES_NOW_PLAYING);
	for ( int i = 0; i < nSelCount; i++ )
	{
		//--- find next item to delete ---//
		nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
		if ( nIndex == -1 )
			break;
		
		// correct nIndex by nIndex - nDeletedSongs, substract the number of entry,
		// which have been already deleted from the array
		// because GetNextItem() still returns the old index values
		if(g_Playlist.Item( nIndex - nDeletedSongs).Format == MUSIK_FORMAT_NETSTREAM)
		{
			if( MUSIK_SOURCES_NOW_PLAYING != g_SourcesCtrl->GetSelType() )
				continue;// net streams cannot be deleted from the playlist, except from now playing list  ( they are deleted in the sources box)
		}
	   //--- if its valid, delete ---//
		if( bDeleteFromDB )
		{
	    	const CMusikSong &song = g_Playlist.Item( nIndex - nDeletedSongs); // get the filename before song is deleted from the array
			if( bDeleteFromComputer )
			{
				if ( !wxRemoveFile( song.Filename ) )
					sError +=  song.Filename + wxT( "\n" );
			}
			g_Library.RemoveSong( song.songid );
		}
		g_Playlist.RemoveAt( nIndex - nDeletedSongs, 1 );
		if(bSourceNowPlayingSelected)
		{
			// active playlist is "now playing" list => delete songs from players playlist too
			g_Player.RemovePlaylistEntry(nIndex - nDeletedSongs);
		}
		nDeletedSongs ++;
	}
	g_PlaylistChanged = (nDeletedSongs > 0);  

	//--- if certain files couldn't be deleted ---//
	if ( !sError.IsEmpty() )
		wxMessageBox( _( "Failed to delete the following files from your computer:\n\n " ) + sError, MUSIKAPPNAME_VERSION, wxICON_STOP );

	m_pParent->Update( false );

	//--- select the last known item ---//
	if ( nFirstSel > ( GetItemCount() - 1 ) )
		nFirstSel = GetItemCount() - 1;
	SetItemState( nFirstSel, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );

	Thaw();
	if(bDeleteFromDB)
		g_ActivityAreaCtrl->ResetAllContents();

	
}

void CPlaylistCtrl::RenameSelFiles()
{
	if ( m_ActiveThreadController.IsAlive() == false )
    {
		CMusikSongArray songs;
		GetSelectedSongs( songs );
		m_ActiveThreadController.AttachAndRun( new MusikPlaylistRenameThread(this, songs ) );
    }
	else
		wxMessageBox( _( "An internal error has occured.\nPrevious thread not terminated correctly.\n\nPlease contact the "MUSIKAPPNAME" development team with this error." ), MUSIKAPPNAME_VERSION, wxICON_STOP );

}

void CPlaylistCtrl::RetagSelFiles()
{
	if ( m_ActiveThreadController.IsAlive() == false )
	{
		CMusikAutoTaggerFrame dlg(this);
		dlg.SetConvertUnderscoresToSpaces((bool)g_Prefs.bAutoTagConvertUnderscoresToSpaces);
		if(dlg.ShowModal()==wxID_CANCEL)
			return;
		g_Prefs.bAutoTagConvertUnderscoresToSpaces = dlg.GetConvertUnderscoresToSpaces() ?1:0;
		CMusikSongArray songs;
		GetSelectedSongs( songs );
		m_ActiveThreadController.AttachAndRun( new MusikPlaylistRetagThread(this, dlg.GetMask(), songs ) );
	}
	else
		wxMessageBox( _( "An internal error has occured.\nPrevious thread not terminated correctly.\n\nPlease contact the "MUSIKAPPNAME" development team with this error." ), MUSIKAPPNAME_VERSION, wxICON_STOP );
}

bool CPlaylistCtrl::ViewDirtyTags()
{
	CMusikSongArray dirty;
	g_Library.QuerySongsWhere( wxT( "dirty = 1" ), dirty );
	if ( dirty.GetCount() > 0 )
	{

		g_Playlist = dirty;
		g_SourcesCtrl->SelectLibrary();
		Update();
		return true;
	}
	else
		wxMessageBox( _( "There are no pending tags available to display." ), MUSIKAPPNAME_VERSION, wxICON_INFORMATION );

	return false;
}

//--------------------------------------//
//--- dnd related playlist functions ---//	//--- please don't call these functions manually, just let dnd do it ---//
//--------------------------------------//
void CPlaylistCtrl::DNDSetCurSel()
{
	//--- find whats currently selected for dnd ---//
	aCurSel.Clear();
	int nIndex = -1;
	if( GetSelectedItemCount() > 0 )
	{
		for( int i = 0; i < GetSelectedItemCount(); i++ )
		{
			nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
			if ( nIndex == -1 )
				break;
			if(g_Playlist.Item( nIndex ).Format != MUSIK_FORMAT_NETSTREAM)
			{
				aCurSel.Add( nIndex );
			}
		}
	}
}

bool CPlaylistCtrl::DNDIsSel( int nVal )
{
	for ( size_t i = 0; i < aCurSel.GetCount(); i++ )
	{
		if ( nVal == aCurSel.Item( i ) )
			return true;
	}
	return false;
}

size_t CPlaylistCtrl::DNDDelSongs()
{
	//--- delete songs we're moving around ---//
	for ( size_t i = 0; i < aCurSel.GetCount(); i++ )
		g_Playlist.RemoveAt( aCurSel.Item( i ) - i, 1 );
	return aCurSel.GetCount();
}
void  CPlaylistCtrl::MovePlaylistEntrys(int nMoveTo ,const wxArrayInt &arrToMove,bool bSelectItems)
{
	// assumes that arrToMove is sorted in ascending order

	wxASSERT(nMoveTo >= 0 && nMoveTo <= g_Playlist.GetCount()); 
	int i = arrToMove.GetCount() - 1;
	// first move all entrys which are behind nMoveTo position
	for(;i >= 0 ; i--)
	{
		if(nMoveTo > arrToMove[i])
			break;
		g_Playlist.Insert(g_Playlist.Detach(arrToMove[i] + ( arrToMove.GetCount() - 1 - i)),nMoveTo);
	}
	// now move all entry which are before
	for(int j = i; j >= 0; j--)
	{
		g_Playlist.Insert(g_Playlist.Detach(arrToMove[j]),nMoveTo - (i - j)-1);
	}
	if(g_SourcesCtrl->GetSelType() == MUSIK_SOURCES_NOW_PLAYING)
	{
		g_Player.MovePlaylistEntrys(	nMoveTo , arrToMove );
	}
	if(bSelectItems)
	{
		//--- select new songs ---//
		wxListCtrlSelNone( this );
		for ( size_t i = 0; i < arrToMove.GetCount(); i++ )
			SetItemState( nMoveTo + i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );

	}

}



void CPlaylistCtrl::DNDDone()
{
	//--- finalize dragging ---//
	Update(false);
	g_DragInProg = false;
	

	//--- clean up ---//
	aCurSel.Clear();
}


bool CPlaylistCtrl::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
   g_MusikFrame->AutoUpdate(filenames,true);

   return true;
}

//----------------------------------------//
//--- MusikPlaylistRenameThread events ---//
//----------------------------------------//
void CPlaylistCtrl::OnThreadStart( wxCommandEvent& event )
{
	SetProgressType(event.GetExtraLong());
	SetProgress(0);

	//--- post the event. we're up and running now! ---//
	wxCommandEvent MusikStartProgEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_FRAME_THREAD_START );
	wxPostEvent( g_MusikFrame, MusikStartProgEvt );
}

void CPlaylistCtrl::OnThreadProg( wxCommandEvent& event )
{
	SetProgress(event.GetExtraLong());
	//--- relay thread progress message to g_MusikFrame ---//
	g_MusikFrame->SetProgress( GetProgress() );

	wxCommandEvent MusikEndProgEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_FRAME_THREAD_PROG );
	wxPostEvent( g_MusikFrame, MusikEndProgEvt );
}

void CPlaylistCtrl::OnThreadEnd( wxCommandEvent& WXUNUSED(event) )
{
	m_ActiveThreadController.Join();

	if( GetProgressType() == MUSIK_PLAYLIST_RETAG_THREAD )
	{
  		g_ActivityAreaCtrl->ResetAllContents();
	}
	g_SourcesCtrl->UpdateCurrent();
	Update();

	//--- update locally ---//
	SetProgressType	( 0 );
	SetProgress	( 0 );

    //--- relay thread end message to g_MusikFrame ---//
	wxCommandEvent MusikEndProgEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_FRAME_THREAD_END );
	wxPostEvent( g_MusikFrame, MusikEndProgEvt );
}

CMusikSongArray * CPlaylistCtrl::GetPlaylist()
{
	return &g_Playlist;
}

void CPlaylistCtrl::OnPlayInstantly( wxCommandEvent& WXUNUSED(event) )
{
	CMusikSongArray aResult;
	GetSelectedSongs(aResult);
	g_Player.InsertToPlaylist(aResult);

}
void CPlaylistCtrl::OnPlayAsNext ( wxCommandEvent& WXUNUSED(event) )
{
	CMusikSongArray aResult;
	GetSelectedSongs(aResult);
	g_Player.InsertToPlaylist(aResult,g_Player.IsPlaying() ? false : true);

}
void CPlaylistCtrl::OnPlayEnqueued	( wxCommandEvent& WXUNUSED(event) )
{
	CMusikSongArray aResult;
	GetSelectedSongs(aResult);
	g_Player.AddToPlaylist(aResult,g_Player.IsPlaying() ? false : true);
}
void CPlaylistCtrl::OnPlayReplace	( wxCommandEvent& WXUNUSED(event) )
{	
	
	int nCurSel = g_PlaylistBox->PlaylistCtrl().GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	if ( nCurSel > -1 )
	{
		g_Player.PlayReplaceList(nCurSel,g_Playlist);
	}
}
