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

CPlaylistBox::CPlaylistBox( wxWindow *parent )
	: wxPanel( parent, -1, wxPoint( -1, -1 ), wxSize( -1, -1 ),  wxCLIP_CHILDREN|wxSUNKEN_BORDER )
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
    if( g_Prefs.nShowPLInfo )
       m_pPlaylistInfoCtrl->Update();
	m_pHorzSizer->Show( m_pPlaylistInfoCtrl, ( bool )g_Prefs.nShowPLInfo );
	Layout();
}
void CPlaylistBox::Update( bool bSelFirstItem )
{
	m_pPlaylistCtrl->Update(bSelFirstItem);
	if ( g_Prefs.nShowPLInfo )
	{
		m_pPlaylistInfoCtrl->Update();
		m_pPlaylistInfoCtrl->Refresh();//HACK why is this needed?
	}
	Layout();

}
CPlaylistBox::~CPlaylistBox()
{
	
}


BEGIN_EVENT_TABLE(CPlaylistCtrl, CMusikListCtrl)
	EVT_LIST_ITEM_ACTIVATED		( MUSIK_PLAYLIST,														CPlaylistCtrl::PlaySel				)	
	EVT_LIST_BEGIN_DRAG			( MUSIK_PLAYLIST,														CPlaylistCtrl::BeginDrag			)
	EVT_LIST_ITEM_SELECTED		( MUSIK_PLAYLIST,														CPlaylistCtrl::UpdateSel			)
	EVT_LIST_COL_BEGIN_DRAG		( MUSIK_PLAYLIST,														CPlaylistCtrl::BeginDragCol			)
	EVT_LIST_COL_END_DRAG		( MUSIK_PLAYLIST,														CPlaylistCtrl::EndDragCol			)
	EVT_MENU					( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FROM_PLAYLIST,					CPlaylistCtrl::OnDelSel				)
	EVT_MENU					( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FILES,							CPlaylistCtrl::OnDelFiles			)
	EVT_MENU					( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FROM_DB,							CPlaylistCtrl::OnDelFilesDB			)
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_RENAME_FILES,									CPlaylistCtrl::OnRenameFiles		)
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_RETAG_FILES,									CPlaylistCtrl::OnRetagFiles			)
	EVT_MENU_RANGE				( MUSIK_PLAYLIST_CONTEXT_UNRATED, MUSIK_PLAYLIST_CONTEXT_RATE5,			CPlaylistCtrl::OnRateSel			) 	
	EVT_UPDATE_UI_RANGE			( MUSIK_PLAYLIST_CONTEXT_UNRATED, MUSIK_PLAYLIST_CONTEXT_RATE5,			CPlaylistCtrl::OnUpdateUIRateSel	)
	EVT_MENU_RANGE				( MUSIK_PLAYLIST_CONTEXT_TAG_TITLE,	MUSIK_PLAYLIST_CONTEXT_TAG_YEAR,	CPlaylistCtrl::OnClickEditTag		)
	EVT_MENU					( MUSIK_PLAYLIST_DISPLAY_SMART,											CPlaylistCtrl::OnDisplaySmart		)
	EVT_MENU					( MUSIK_PLAYLIST_DISPLAY_FIT,											CPlaylistCtrl::OnDisplayFit			)
	EVT_CONTEXT_MENU			(																		CPlaylistCtrl::ShowMenu				)
	EVT_CHAR					(																		CPlaylistCtrl::TranslateKeys		)
	EVT_LIST_COL_CLICK			( MUSIK_PLAYLIST,														CPlaylistCtrl::OnColumnClick		)

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
bool PlaylistDropTarget::OnDropText( wxCoord x, wxCoord y, const wxString &text )
{
	wxString sFiles = text;
	//--- make sure we have something ---//
	if ( sFiles != wxT( "" ) )
	{
		//--- seperate values. "s\n" for sources, "p\n" for playlist ---//
		wxString sType = sFiles.Left( 2 );
		sFiles = sFiles.Right( sFiles.Length() - 2 );

		//--- where did we land? ---//
		const wxPoint& pt = wxPoint( x, y );
		int nFlags;
		n = m_pPlaylistCtrl->HitTest( pt, nFlags );

		//--- originated from sources box, so just populate ---//
		if ( sType == wxT( "s\n" ) ){ 	g_DragInProg = false; }

		//--- drag originated from playlist, rearrange ---//
		else if ( sType == wxT( "p\n" ) )	
		{
			//--- make sure we havn't dragged on a selected item ---//
			for ( size_t i = 0; i < m_pPlaylistCtrl->aCurSel.GetCount(); i++ )
			{
				if ( m_pPlaylistCtrl->DNDIsSel( n ) )
				{
					m_pPlaylistCtrl->aCurSel.Clear();
					m_pPlaylistCtrl->aCurSelSongs.Clear();
					g_DragInProg = false;
					return FALSE;
				}
			}

			//--- this is the name of the file we're gonna insert items at ---//
			wxString sFile;
			if ( n == -1 )
				sFile = m_pPlaylistCtrl->GetFilename( m_pPlaylistCtrl->GetItemCount() - 1 );
			else
				sFile = m_pPlaylistCtrl->GetFilename( n );

			//--- first lets make sure user intended to drop files in the playlist ---//
			//--- otherwise we may have songs removed that shouldn't be. that would be bad ---//
			int nLastSelItem = m_pPlaylistCtrl->aCurSel.Item( m_pPlaylistCtrl->aCurSel.GetCount()-1 );
			if ( n == -1 || n < nLastSelItem || n > nLastSelItem )
			{
				//--- delete old songs ---//
				m_pPlaylistCtrl->DNDDelSongs();

				//--- not dragged over anything, push to bottom ---//
				if ( n == -1 )
				{
					m_pPlaylistCtrl->DNDInsertItems( sFile, MUSIK_DND_BOTTOM );
					n = m_pPlaylistCtrl->DNDGetItemPos( sFile ) + 1;
				}

				//--- else find where we need to drop ---//
				else
				{
					//--- error, couldn't locate our position ---//
					if ( n == -2 )
					{
						wxMessageBox( _( "An internal error has occured.\nCould not find previous item's position.\n\nPlease contact the "MUSIKAPPNAME" development team with this error." ), MUSIKAPPNAME_VERSION, wxOK|wxICON_ERROR );
						g_DragInProg = false;
						return FALSE;
					}

					//--- dragged above old pos ---//
					else if ( n < nLastSelItem )		
					{
						m_pPlaylistCtrl->DNDInsertItems( sFile, MUSIK_DND_ABOVE );
						n = n;
					}

					//--- dragged below old pos ---//
					else if ( n > nLastSelItem )	
					{
						m_pPlaylistCtrl->DNDInsertItems( sFile, MUSIK_DND_BELOW );
						n = m_pPlaylistCtrl->DNDGetItemPos( sFile ) + 1;
					}
				}
			}
		}
	}
	m_pPlaylistCtrl->DNDDone( n );
	return TRUE;
}

wxDragResult PlaylistDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult WXUNUSED(def))
{
	//--- calls HighlightSel() to highlight the item the mouse is over ---//
	m_pPlaylistCtrl->SetFocus();
	const wxPoint& pt = wxPoint( x, y );
	HighlightSel( pt );
	return wxDragMove;
}

void PlaylistDropTarget::HighlightSel( wxPoint pPos )
{
	//--- gotta set this so stuff doesn't keep updating ---//
	g_DragInProg	= true;
	int nFlags;
	long n = m_pPlaylistCtrl->HitTest( pPos, nFlags );

	//--- highlight what we're over, deselect old ---//
	if ( n != nLastHit && n!= -1 )
	{
		if ( !m_pPlaylistCtrl->DNDIsSel( nLastHit ) )
			m_pPlaylistCtrl->SetItemState( nLastHit, 0, wxLIST_STATE_SELECTED );

		m_pPlaylistCtrl->SetItemState( n, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
	}

	//--- this is a quick way to check if we need to update ---//
	nLastHit = n;
}

//----------------------------//
//--- construct / destruct ---//
//----------------------------//
CPlaylistCtrl::CPlaylistCtrl( wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size )
	:	CMusikListCtrl		( parent, id, pos, size,wxNO_BORDER)
{
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ) );

	//--- setup headers ---//
	m_ColSaveNeeded = false;
	ResetColumns();

	//--- rating menu ---//
	playlist_context_rating_menu = new wxMenu;
	playlist_context_rating_menu->Append( MUSIK_PLAYLIST_CONTEXT_UNRATED, _( "Unrated" ), wxT( "" ), wxITEM_CHECK );
	playlist_context_rating_menu->Append( MUSIK_PLAYLIST_CONTEXT_RATE1, wxT( "1" ), wxT( "" ), wxITEM_CHECK );
	playlist_context_rating_menu->Append( MUSIK_PLAYLIST_CONTEXT_RATE2, wxT( "2" ), wxT( "" ), wxITEM_CHECK );
	playlist_context_rating_menu->Append( MUSIK_PLAYLIST_CONTEXT_RATE3, wxT( "3" ), wxT( "" ), wxITEM_CHECK );
	playlist_context_rating_menu->Append( MUSIK_PLAYLIST_CONTEXT_RATE4, wxT( "4" ), wxT( "" ), wxITEM_CHECK );
	playlist_context_rating_menu->Append( MUSIK_PLAYLIST_CONTEXT_RATE5, wxT( "5" ), wxT( "" ), wxITEM_CHECK );

	//--- tag edit menu ---//
	playlist_context_edit_tag_menu = new wxMenu;
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
	playlist_context_delete_menu = new wxMenu;
	playlist_context_delete_menu->Append( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FROM_PLAYLIST,	_( "From Playlist\tDel" ) );
	playlist_context_delete_menu->Append( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FROM_DB,			_( "From Database\tAlt+Del" ) );
	playlist_context_delete_menu->Append( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FILES,			_( "From Computer\tCtrl+Del" ) );

	//--- columns context menu ---//
	playlist_context_display_menu = new wxMenu;
	playlist_context_display_menu->AppendCheckItem( MUSIK_PLAYLIST_DISPLAY_RATING,			_( "Rating" ) );
	playlist_context_display_menu->AppendCheckItem( MUSIK_PLAYLIST_DISPLAY_TRACK,			_( "Track" ) );
	playlist_context_display_menu->AppendCheckItem( MUSIK_PLAYLIST_DISPLAY_TITLE,			_( "Title" ) );
	playlist_context_display_menu->AppendCheckItem( MUSIK_PLAYLIST_DISPLAY_ARTIST,			_( "Artist" ));
	playlist_context_display_menu->AppendCheckItem( MUSIK_PLAYLIST_DISPLAY_ALBUM,			_( "Album" ) );
	playlist_context_display_menu->AppendCheckItem( MUSIK_PLAYLIST_DISPLAY_YEAR,			_( "Year" ) );
	playlist_context_display_menu->AppendCheckItem( MUSIK_PLAYLIST_DISPLAY_GENRE,			_( "Genre" ) );
	playlist_context_display_menu->AppendCheckItem( MUSIK_PLAYLIST_DISPLAY_TIMES_PLAYED,	_( "Times Played" ) );
	playlist_context_display_menu->AppendCheckItem( MUSIK_PLAYLIST_DISPLAY_LAST_PLAYED,		_( "Last Played" ));
	playlist_context_display_menu->AppendCheckItem( MUSIK_PLAYLIST_DISPLAY_TIME,			_( "Time" ) );
	playlist_context_display_menu->AppendCheckItem( MUSIK_PLAYLIST_DISPLAY_BITRATE,			_( "Bitrate" ) );
	playlist_context_display_menu->AppendCheckItem( MUSIK_PLAYLIST_DISPLAY_FILENAME,		_( "Filename" ) );
	playlist_context_display_menu->AppendSeparator();
	playlist_context_display_menu->AppendCheckItem( MUSIK_PLAYLIST_DISPLAY_FIT,				_( "Fit Columns" ) );
	playlist_context_display_menu->AppendCheckItem( MUSIK_PLAYLIST_DISPLAY_SMART,			_( "No Horizontal Scroll" ) );

	//--- main context menu ---//
	playlist_context_menu = new wxMenu;
	playlist_context_menu->Append( MUSIK_PLAYLIST_CONTEXT_RATENODE,			_( "&Rating" ),					playlist_context_rating_menu );
	playlist_context_menu->Append( MUSIK_PLAYLIST_CONTEXT_DISPLAYNODE,		_( "Display" ),					playlist_context_display_menu );
	playlist_context_menu->AppendSeparator();
	playlist_context_menu->Append( MUSIK_PLAYLIST_CONTEXT_DELETENODE,		_( "F&ile Operations" ),		playlist_context_delete_menu );
	playlist_context_menu->AppendSeparator();
	playlist_context_menu->Append( MUSIK_PLAYLIST_CONTEXT_TAGNODE,			_( "Edit &Tag" ),				playlist_context_edit_tag_menu );


	//--- setup drop target ---//
	SetDropTarget( new PlaylistDropTarget( this ) );

	//--- not dragging, no selections ---//
	g_DragInProg = false;
	nCurSel = -1;
	m_Overflow = 0;
	m_ActiveThread  = NULL;
	m_bColDragging = false;

}

CPlaylistCtrl::~CPlaylistCtrl()
{
	delete playlist_context_menu;
}

///////////////////////////////////////////////////////////////////////////////

void CPlaylistCtrl::OnColumnClick( wxListEvent& event )
{
	if(g_SourcesCtrl->GetSelType() != MUSIK_SOURCES_LIBRARY) // only sort in library view, else we will destroy the user playlist
		return;
	int ActualColumn = m_ColumnOrder.Item( event.GetColumn() );
	wxString sortstr = g_PlaylistColumnDBNames[ ActualColumn ];
	m_aColumnSorting.Item( ActualColumn ) = -m_aColumnSorting.Item( ActualColumn );	
	bool desc = ( m_aColumnSorting.Item( ActualColumn ) < 0 );

	g_Library.SortPlaylist( sortstr, desc );
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

		if ( g_Prefs.nPlaylistColumnDynamic[nCurrCol] == 0 )
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

		if ( g_Prefs.nPlaylistColumnDynamic[nCurrCol] == 1 )
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

	bool bItemSel = false;
	if ( GetSelectedItemCount() > 0 )
	{
		int nFirstIndex = GetNextItem( -1, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
		if(g_Playlist.Item ( nFirstIndex ).Format != MUSIK_FORMAT_NETSTREAM)
		{
			bItemSel = true;
		}
	}
	playlist_context_menu->Enable( MUSIK_PLAYLIST_CONTEXT_DELETENODE,	bItemSel );
	playlist_context_menu->Enable( MUSIK_PLAYLIST_CONTEXT_RENAME_FILES, bItemSel );
	playlist_context_menu->Enable( MUSIK_PLAYLIST_CONTEXT_RETAG_FILES,	bItemSel );
	playlist_context_menu->Enable( MUSIK_PLAYLIST_CONTEXT_RATENODE,		bItemSel );
	playlist_context_menu->Enable( MUSIK_PLAYLIST_CONTEXT_TAGNODE,		bItemSel );

	PopupMenu( playlist_context_menu, pos );
}
int CPlaylistCtrl::DisplayEventId2ColumnId( int evid)
{
	int nColumn = -1;
	switch ( evid )
	{
	case MUSIK_PLAYLIST_DISPLAY_RATING:
		nColumn = PLAYLISTCOLUMN_RATING;
		break;
	case MUSIK_PLAYLIST_DISPLAY_TRACK:
		nColumn = PLAYLISTCOLUMN_TRACK;
		break;
	case MUSIK_PLAYLIST_DISPLAY_TITLE:
		nColumn = PLAYLISTCOLUMN_TITLE;
		break;
	case MUSIK_PLAYLIST_DISPLAY_ARTIST:
		nColumn = PLAYLISTCOLUMN_ARTIST;
		break;
	case MUSIK_PLAYLIST_DISPLAY_ALBUM:
		nColumn = PLAYLISTCOLUMN_ALBUM;
		break;
	case MUSIK_PLAYLIST_DISPLAY_YEAR:
		nColumn = PLAYLISTCOLUMN_YEAR;
		break;
	case MUSIK_PLAYLIST_DISPLAY_GENRE:
		nColumn = PLAYLISTCOLUMN_GENRE;
		break;
	case MUSIK_PLAYLIST_DISPLAY_TIMES_PLAYED:
		nColumn = PLAYLISTCOLUMN_TIMES_PLAYED;
		break;
	case MUSIK_PLAYLIST_DISPLAY_LAST_PLAYED:
		nColumn = PLAYLISTCOLUMN_LAST_PLAYED;
		break;
	case MUSIK_PLAYLIST_DISPLAY_TIME:
		nColumn = PLAYLISTCOLUMN_TIME;
		break;
	case MUSIK_PLAYLIST_DISPLAY_BITRATE:
		nColumn = PLAYLISTCOLUMN_BITRATE;
		break;
	case MUSIK_PLAYLIST_DISPLAY_FILENAME:
		nColumn = PLAYLISTCOLUMN_FILENAME;
		break;
	default:
		wxASSERT(0);
	}
	return nColumn;
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
		g_Prefs.nPlaylistColumnEnable[nColumn] = !g_Prefs.nPlaylistColumnEnable[nColumn];
		ResetColumns( false, true );
	}
}
void CPlaylistCtrl::OnDisplayFit( wxCommandEvent& WXUNUSED(event) )
{
	RescaleColumns( true, false, true );
}
void CPlaylistCtrl::OnUpdateUIDisplayMenu ( wxUpdateUIEvent &event)
{
	int nColumn = DisplayEventId2ColumnId(event.GetId());
	if(nColumn > -1)
		event.Check(g_Prefs.nPlaylistColumnEnable[nColumn]);	
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
	g_Prefs.nPlaylistSmartColumns = !g_Prefs.nPlaylistSmartColumns;
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
		wxString sDrop = wxT( "p\n" ) + sValidSelFiles;
		DNDSetCurSel();

		//--- initialize drag and drop... SourcesDropTarget / PlaylistDropTarget should take care of the rest ---//
		wxDropSource dragSource( this );
		wxTextDataObject song_data( sDrop );
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


void CPlaylistCtrl::PlaySel( wxListEvent& WXUNUSED(event) )
{
	g_Player.PlayCurSel();
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
		if ( song.Artist == wxT( "<unknown>" ) )
			return wxT( "-" );
		else 
		{
			return SanitizedString( song.Artist );
		}
		break;

	case PLAYLISTCOLUMN_ALBUM:
		if ( song.Album == _( "<unknown>" ) )
			return wxT( "-" );
		else
			return SanitizedString( song.Album );
		break;

	case PLAYLISTCOLUMN_YEAR:
		if ( song.Year == _( "<unknown>" ) )
			return wxT( "-" );
		else
			return song.Year;
		break;

	case PLAYLISTCOLUMN_GENRE:
		if ( song.Genre == _( "<unknown>" ) )
			return wxT( "-" );
		else
			return SanitizedString( song.Genre );
		break;

	case PLAYLISTCOLUMN_TIMES_PLAYED:
		{
			wxString str;
			if ( song.TimesPlayed > 0 )
				str << song.TimesPlayed;
			return str;
		}
		break;

	case PLAYLISTCOLUMN_LAST_PLAYED:
		if ( song.LastPlayed != wxT( "" ) )
			return song.LastPlayed;
		else
			return wxT( "" );
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
	}

	return wxT( "" );
}

void CPlaylistCtrl::FindColumnOrder()
{
	m_ColumnOrder.Clear();
	m_aColumnSorting.Clear();
	for ( int i = 0; i < NPLAYLISTCOLUMNS; i++ )
	{
		if ( g_Prefs.nPlaylistColumnEnable[i] == 1 )
		{
			m_ColumnOrder.Add( i );
		}
		m_aColumnSorting.Add( -1 );
	}
}

int CPlaylistCtrl::OnGetItemImage(long item) const
{
	return g_Playlist.Item ( item ).Rating;
}

wxListItemAttr* CPlaylistCtrl::OnGetItemAttr(long item) const
{
	const CMusikSong & song = g_Playlist.Item ( item );
	if ( g_Player.IsPlaying() && song.Filename == g_Player.GetCurrentFile() )
	{
		g_Player.SetCurSel(item);
		return item % 2 ? (wxListItemAttr *)&m_SelectedDarkAttr : (wxListItemAttr *)&m_SelectedLightAttr;		
	}
	else
	{
		if ( g_Prefs.nPLStripes == 1 )
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
	sResult.Alloc(itemcount * 50); // 50 is just a for the avarege file name length
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

wxArrayInt CPlaylistCtrl::GetSelItems()
{
	wxArrayInt aResult;
	int nIndex = -1;
	for ( int i = 0; i < GetSelectedItemCount(); i++ )
	{
		nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
		if ( nIndex == -1 )
			break;
		aResult.Add( nIndex );
	}
	return aResult;	
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

void CPlaylistCtrl::GetSelSongs(CMusikSongArray & aResult)
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
			CMusikSong *pSong = new CMusikSong();
			g_Library.GetSongFromFilename( GetFilename( nIndex ), pSong );
			aResult.Add( pSong ); // array takes ownership of the pointer
			
		}
	}
	return;
}

double CPlaylistCtrl::GetTotalFilesize()
{
	// just leaving this here until we're sure the new way is working,
	// then we can get rid of it, and the CNiceFilesize class
	/*CNiceFilesize filesize;

	for ( int i = 0; i < GetItemCount(); i++ )
	{
		CMusikSong song = g_Playlist.Item( i );
		filesize.AddB( song.Filesize );
	}

	return filesize.GetFormatted();*/
	return g_Library.GetTotalPlaylistSize();
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

wxString CPlaylistCtrl::GetFilename( int nItem )
{
	if ( nItem > -1 )
	{
		const CMusikSong & song = g_Playlist.Item ( nItem );
		return song.Filename;
	}

	//--- not found? return a null string ---//
	return wxT( "" );
}



//----------------------------------------//
//--- various other functions we need. ---//
//----------------------------------------//
void CPlaylistCtrl::ShowIcons()
{
	if ( g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_RATING] == 0 )
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
		wxString sCurrFile = g_Playlist.Item( item ).Filename;
		g_Library.GetSongFromFilename( sCurrFile, &g_Playlist.Item( item ) );
		
		if ( lastitem > -1 && lastitem != item )
		{
			wxString sCurrFile = g_Playlist.Item( lastitem ).Filename;
			g_Library.GetSongFromFilename( sCurrFile, &g_Playlist.Item(lastitem ) );		
		}			
	}

	RefreshItem( item );

	if ( lastitem > -1 && lastitem != item )
		RefreshItem( lastitem );	
}
void CPlaylistCtrl::ResynchItem( int item, const CMusikSong & song)
{
	wxASSERT(g_Playlist.GetCount() &&(item < g_Playlist.GetCount()));
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
		if ( g_Prefs.nPlaylistColumnDynamic[nCurrItem] == 0 )
			nStaticWidth += g_Prefs.nPlaylistColumnSize[nCurrItem];
	}

	//-------------------------------------------------//
	//--- if using smart columns, we need to find	---//
	//--- what percentages mean.					---//
	//-------------------------------------------------//
	if ( g_Prefs.nPlaylistSmartColumns == 1 || bAutoFit )
	{
		for ( size_t i = 0; i < m_ColumnOrder.GetCount(); i++ )
		{
			nCurrItem = m_ColumnOrder.Item( i );
			if ( g_Prefs.nPlaylistColumnDynamic[nCurrItem] == 1 )
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
		if ( g_Prefs.nPlaylistColumnDynamic[nCurrItem] == 0 )
			SetColumnWidth( i, g_Prefs.nPlaylistColumnSize[nCurrItem] );

		//-------------------------//
		//--- set dynamic size	---//
		//-------------------------//
		else
		{
			n_LastDyn = i;

			if ( g_Prefs.nPlaylistSmartColumns == 1 || bAutoFit )
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
	if ( ( g_Prefs.nPlaylistSmartColumns == 1 || bAutoFit ) && nTotalPercent && nDynamicWidth )
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
		g_Library.SetRating( GetFilename( nIndex ), nVal );
		g_Playlist.Item( nIndex ).Rating = nVal;
		RefreshItem( nIndex );
		
	}
	g_Library.EndTransaction();
}

void CPlaylistCtrl::EditTag( int i )
{
	int nSelCount = GetSelectedItemCount();
	CMusikSongArray songs;
	int nEditType = 0;
	int nIndex = -1;

	//--- if there are less than 2 songs selected, setup for single edit mode ---//
	if ( nSelCount < 2 )
	{
		songs = g_Playlist;
		nEditType = MUSIK_TAG_SINGLE;
		nIndex = GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	}

	//--- more than 1 song, setup for batch edit mode ---//
	else if ( nSelCount >= 2 )
	{
		GetSelSongs( songs );
		nEditType = MUSIK_TAG_MULTIPLE;
	}

	//--- call the tag dialog, disable this window. ---//
	//---  tag dialog will re-enable when complete  ---//
	if ( nEditType != 0 )
	{
		MusikTagFrame* pMusikTagFrame = new MusikTagFrame( g_MusikFrame, songs, i, nEditType, nIndex );
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
			continue;// net streams cannot be deleted from the playlist ( they are deleted in the sources box)
  		
	   //--- if its valid, delete ---//
		if( bDeleteFromDB )
		{
	    	sFile = g_Playlist.Item( nIndex - nDeletedSongs).Filename; // get the filename before song is deleted from the array
			if( bDeleteFromComputer )
			{
				if ( !wxRemoveFile( sFile ) )
					sError +=  sFile + wxT( "\n" );
			}
			g_Library.RemoveSong( sFile );
		}
		g_Playlist.RemoveAt( nIndex - nDeletedSongs, 1 );
		nDeletedSongs ++;
	}
	g_PlaylistChanged = (nDeletedSongs > 0);  

	//--- if certain files couldn't be deleted ---//
	if ( !sError.IsEmpty() )
		wxMessageBox( _( "Failed to delete the following files from your computer:\n\n " ) + sError, MUSIKAPPNAME_VERSION, wxICON_STOP );

	Update( false );

	//--- select the last known item ---//
	if ( nFirstSel > ( GetItemCount() - 1 ) )
		nFirstSel = GetItemCount() - 1;
	SetItemState( nFirstSel, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );

	Thaw();

	g_ActivityAreaCtrl->ResetAllContents();

	
}

void CPlaylistCtrl::RenameSelFiles()
{
	if ( GetActiveThread() == 0 )
	{
		CMusikSongArray songs;
		GetSelSongs( songs );
		pRenameThread = new MusikPlaylistRenameThread(this, songs );
		pRenameThread->Create();
		SetActiveThread(pRenameThread);
		pRenameThread->Run();
	}
	else
		wxMessageBox( _( "An internal error has occured.\nPrevious thread not terminated correctly.\n\nPlease contact the "MUSIKAPPNAME" development team with this error." ), MUSIKAPPNAME_VERSION, wxICON_STOP );

}

void CPlaylistCtrl::RetagSelFiles()
{
	if ( GetActiveThread() == 0 )
	{
		CMusikAutoTaggerFrame dlg(this);
		dlg.SetConvertUnderscoresToSpaces((bool)g_Prefs.nAutoTagConvertUnderscoresToSpaces);
		if(dlg.ShowModal()==wxID_CANCEL)
			return;
		g_Prefs.nAutoTagConvertUnderscoresToSpaces = dlg.GetConvertUnderscoresToSpaces() ?1:0;
		CMusikSongArray songs;
		GetSelSongs( songs );
		pRetagThread = new MusikPlaylistRetagThread(this, dlg.GetMask(), songs );
		pRetagThread->Create();
		SetActiveThread	( pRetagThread );
		pRetagThread->Run();
	}
	else
		wxMessageBox( _( "An internal error has occured.\nPrevious thread not terminated correctly.\n\nPlease contact the "MUSIKAPPNAME" development team with this error." ), MUSIKAPPNAME_VERSION, wxICON_STOP );
}

bool CPlaylistCtrl::ViewDirtyTags()
{
	CMusikSongArray dirty;
	g_Library.QuerySongs( wxT( "dirty = 1" ), dirty );
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
	aCurSelSongs.Clear();
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
				aCurSelSongs.Add( g_Playlist.Item( nIndex ) );
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

void CPlaylistCtrl::DNDDelSongs()
{
	//--- delete songs we're moving around ---//
	for ( size_t i = 0; i < aCurSel.GetCount(); i++ )
		g_Playlist.RemoveAt( aCurSel.Item( i ) - i, 1 );
}

void CPlaylistCtrl::DNDInsertItems( wxString sFile, int nType )
{
	CMusikSongArray *pPlaylist = &g_Playlist;
	if ( nType == MUSIK_DND_BOTTOM )
	{
		//--- just push to back of stack ---//
		for ( size_t i = 0; i < aCurSelSongs.GetCount(); i++ )
			pPlaylist->Add( aCurSelSongs.Item( i ) );
	}

	else
	{ 
		//--- this is where the final drop is going to     ---//
		//--- take place. a crucial step.				   ---//
		int nIndex = DNDGetItemPos( sFile );

		for ( size_t i = 0; i < aCurSelSongs.GetCount(); i++ )
		{
			//--- push items to nIndex + 1, will force them to appear below the target drop ---//
			if ( nType == MUSIK_DND_BELOW )
				pPlaylist->Insert( aCurSelSongs.Item( i ), nIndex + 1 );

			//--- push items to nIndex, will force them to appear above the target drop ---//
			else if ( nType == MUSIK_DND_ABOVE )
				pPlaylist->Insert( aCurSelSongs.Item( i ), nIndex );

			nIndex++;  
		}
	}
}

int CPlaylistCtrl::DNDGetItemPos( wxString sFile )
{
	//--- get the position of a specified file in the playlist ---//
	CMusikSongArray *pPlaylist = &g_Playlist;
	for ( size_t i = 0; i < pPlaylist->GetCount(); i++ )
	{
		if ( pPlaylist->Item( i ).Filename == sFile )
			return i;
	}
	return -2;
}

void CPlaylistCtrl::DNDDone( int nNewPos )
{
	//--- finalize dragging, set selection ---//
	Update();
	g_DragInProg = false;
	
	//--- select new songs ---//
	wxListCtrlSelNone( this );
	for ( size_t i = 0; i < aCurSelSongs.GetCount(); i++ )
		SetItemState( nNewPos + i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );

	//--- clean up ---//
	aCurSel.Clear();
	aCurSelSongs.Clear();
}

//----------------------------------------//
//--- MusikPlaylistRenameThread events ---//
//----------------------------------------//
void CPlaylistCtrl::OnThreadStart( wxCommandEvent& event )
{
	SetProgressType(event.GetExtraLong());
	SetProgress(0);
	//--- setup thread to begin in g_MusikFrame ---//
	g_MusikFrame->SetActiveThread	( GetActiveThread() );
	g_MusikFrame->SetProgressType	( GetProgressType() );
	g_MusikFrame->SetProgress		( GetProgress() );

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
	if( GetProgressType() == MUSIK_PLAYLIST_RETAG_THREAD )
	{
		//g_Playlist = pRetagThread->GetReTaggedSongs();
  		g_ActivityAreaCtrl->ResetAllContents();
		g_SourcesCtrl->UpdateCurrent();
	}
	SetActiveThread	( NULL );
	Update();

	//--- update locally ---//
	SetProgressType	( 0 );
	SetProgress	( 0 );

    //--- relay thread end message to g_MusikFrame ---//
	wxCommandEvent MusikEndProgEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_FRAME_THREAD_END );
	wxPostEvent( g_MusikFrame, MusikEndProgEvt );
}
void CPlaylistCtrl::SetActiveThread( wxThread* newactivethread)
{
	wxThread * pCurrThread = GetActiveThread();
	if(newactivethread == NULL)
	{
		
		wxASSERT(pCurrThread);
		pCurrThread->Wait();// wait until thread has completed
		delete pCurrThread;
		m_ActiveThread = NULL;
	}
	else
	{
		wxASSERT(pCurrThread == NULL); // ATTENTION!!! there is an active thread. someone forgot to call SetActiveThread(NULL)
	}
	m_ActiveThread = newactivethread;
}
