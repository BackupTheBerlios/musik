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

//--- threads ---//
#include "../Threads/PlaylistCtrlThreads.h"

BEGIN_EVENT_TABLE(CPlaylistCtrl, wxListCtrl)
	EVT_LIST_ITEM_ACTIVATED		( MUSIK_PLAYLIST,											CPlaylistCtrl::PlaySel			)	
	EVT_LIST_BEGIN_DRAG			( MUSIK_PLAYLIST,											CPlaylistCtrl::BeginDrag		)
	EVT_LIST_ITEM_SELECTED		( MUSIK_PLAYLIST,											CPlaylistCtrl::UpdateSel		)
	EVT_LIST_COL_END_DRAG		( MUSIK_PLAYLIST,											CPlaylistCtrl::EndDragCol		)
	EVT_MENU					( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FROM_PLAYLIST,		CPlaylistCtrl::OnDelSel			)
	EVT_MENU					( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FILES,				CPlaylistCtrl::OnDelFiles		)
	EVT_MENU					( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FROM_DB,				CPlaylistCtrl::OnDelFilesDB		)
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_RENAME_FILES,						CPlaylistCtrl::OnRenameFiles	)
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_RETAG_FILES,						CPlaylistCtrl::OnRetagFiles		)
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_UNRATED,							CPlaylistCtrl::UnrateSel		) 	
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_RATE1,								CPlaylistCtrl::Rate1Sel			) 	
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_RATE2,								CPlaylistCtrl::Rate2Sel			) 
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_RATE3,								CPlaylistCtrl::Rate3Sel			) 
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_RATE4,								CPlaylistCtrl::Rate4Sel			) 
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_RATE5,								CPlaylistCtrl::Rate5Sel			)
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_TAG_TITLE,							CPlaylistCtrl::ClickTitle		)
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_TAG_TRACKNUM,						CPlaylistCtrl::ClickTrackNum	)
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_TAG_ARTIST,						CPlaylistCtrl::ClickArtist		)
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_TAG_ALBUM,							CPlaylistCtrl::ClickAlbum		)
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_TAG_GENRE,							CPlaylistCtrl::ClickGenre		)
	EVT_MENU					( MUSIK_PLAYLIST_CONTEXT_TAG_YEAR,							CPlaylistCtrl::ClickYear		)
	EVT_MENU					( MUSIK_PLAYLIST_DISPLAY_SMART,								CPlaylistCtrl::OnDisplaySmart	)
	EVT_CONTEXT_MENU			(															CPlaylistCtrl::ShowMenu			)
	EVT_CHAR					(															CPlaylistCtrl::TranslateKeys	)

	//---------------------------------------------------------//
	//--- column on off stuff.								---//
	//---------------------------------------------------------//
	EVT_MENU					( MUSIK_PLAYLIST_DISPLAY_RATING,							CPlaylistCtrl::OnDisplayMenu	)
	EVT_MENU					( MUSIK_PLAYLIST_DISPLAY_TRACK,								CPlaylistCtrl::OnDisplayMenu	)
	EVT_MENU					( MUSIK_PLAYLIST_DISPLAY_TITLE,								CPlaylistCtrl::OnDisplayMenu	)
	EVT_MENU					( MUSIK_PLAYLIST_DISPLAY_ARTIST,							CPlaylistCtrl::OnDisplayMenu	)
	EVT_MENU					( MUSIK_PLAYLIST_DISPLAY_ALBUM,								CPlaylistCtrl::OnDisplayMenu	)
	EVT_MENU					( MUSIK_PLAYLIST_DISPLAY_YEAR,								CPlaylistCtrl::OnDisplayMenu	)
	EVT_MENU					( MUSIK_PLAYLIST_DISPLAY_GENRE,								CPlaylistCtrl::OnDisplayMenu	)
	EVT_MENU					( MUSIK_PLAYLIST_DISPLAY_TIMES_PLAYED,						CPlaylistCtrl::OnDisplayMenu	)
	EVT_MENU					( MUSIK_PLAYLIST_DISPLAY_LAST_PLAYED,						CPlaylistCtrl::OnDisplayMenu	)
	EVT_MENU					( MUSIK_PLAYLIST_DISPLAY_TIME,								CPlaylistCtrl::OnDisplayMenu	)
	EVT_MENU					( MUSIK_PLAYLIST_DISPLAY_BITRATE,							CPlaylistCtrl::OnDisplayMenu	)
	EVT_MENU					( MUSIK_PLAYLIST_DISPLAY_FILENAME,							CPlaylistCtrl::OnDisplayMenu	)

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
		n = g_PlaylistCtrl->HitTest( pt, nFlags );

		//--- originated from sources box, so just populate ---//
		if ( sType == wxT( "s\n" ) ){ 	g_DragInProg = false; }

		//--- drag originated from playlist, rearrange ---//
		else if ( sType == wxT( "p\n" ) )	
		{
			//--- make sure we havn't dragged on a selected item ---//
			for ( size_t i = 0; i < g_PlaylistCtrl->aCurSel.GetCount(); i++ )
			{
				if ( g_PlaylistCtrl->DNDIsSel( n ) )
				{
					g_PlaylistCtrl->aCurSel.Clear();
					g_PlaylistCtrl->aCurSelSongs.Clear();
					g_DragInProg = false;
					return FALSE;
				}
			}

			//--- this is the name of the file we're gonna insert items at ---//
			wxString sFile;
			if ( n == -1 )
				sFile = g_PlaylistCtrl->GetFilename( g_PlaylistCtrl->GetItemCount() - 1 );
			else
				sFile = g_PlaylistCtrl->GetFilename( n );

			//--- first lets make sure user intended to drop files in the playlist ---//
			//--- otherwise we may have songs removed that shouldn't be. that would be bad ---//
			int nLastSelItem = g_PlaylistCtrl->aCurSel.Item( g_PlaylistCtrl->aCurSel.GetCount()-1 );
			if ( n == -1 || n < nLastSelItem || n > nLastSelItem )
			{
				//--- delete old songs ---//
				g_PlaylistCtrl->DNDDelSongs();

				//--- not dragged over anything, push to bottom ---//
				if ( n == -1 )
				{
					g_PlaylistCtrl->DNDInsertItems( sFile, MUSIK_DND_BOTTOM );
					n = g_PlaylistCtrl->DNDGetItemPos( sFile ) + 1;
				}

				//--- else find where we need to drop ---//
				else
				{
					//--- error, couldn't locate our position ---//
					if ( n == -2 )
					{
						wxMessageBox( _( "An internal error has occured.\nCould not find previous item's position.\n\nPlease contact the Musik development team with this error." ), MUSIK_VERSION, wxOK|wxICON_ERROR );
						g_DragInProg = false;
						return FALSE;
					}

					//--- dragged above old pos ---//
					else if ( n < nLastSelItem )		
					{
						g_PlaylistCtrl->DNDInsertItems( sFile, MUSIK_DND_ABOVE );
						n = n;
					}

					//--- dragged below old pos ---//
					else if ( n > nLastSelItem )	
					{
						g_PlaylistCtrl->DNDInsertItems( sFile, MUSIK_DND_BELOW );
						n = g_PlaylistCtrl->DNDGetItemPos( sFile ) + 1;
					}
				}
			}
		}
	}
	g_PlaylistCtrl->DNDDone( n );
	return TRUE;
}

wxDragResult PlaylistDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
	//--- calls HighlightSel() to highlight the item the mouse is over ---//
	g_PlaylistCtrl->SetFocus();
	const wxPoint& pt = wxPoint( x, y );
	HighlightSel( pt );
	return wxDragMove;
}

void PlaylistDropTarget::HighlightSel( wxPoint pPos )
{
	//--- gotta set this so stuff doesn't keep updating ---//
	g_DragInProg	= true;
	int nFlags;
	long n = g_PlaylistCtrl->HitTest( pPos, nFlags );

	//--- highlight what we're over, deselect old ---//
	if ( n != nLastHit && n!= -1 )
	{
		if ( !g_PlaylistCtrl->DNDIsSel( nLastHit ) )
			g_PlaylistCtrl->SetItemState( nLastHit, 0, wxLIST_STATE_SELECTED );

		g_PlaylistCtrl->SetItemState( n, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
	}

	//--- this is a quick way to check if we need to update ---//
	nLastHit = n;
}

//----------------------------//
//--- construct / destruct ---//
//----------------------------//
CPlaylistCtrl::CPlaylistCtrl( wxWindow *parent, const wxWindowID id, const wxPoint& pos, const wxSize& size )
	:	wxListCtrl		( parent, id, pos, size, wxLC_REPORT | wxLC_VIRTUAL | wxSIMPLE_BORDER | wxCLIP_CHILDREN & ~wxHSCROLL )
{
	//--- setup headers ---//
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
	playlist_context_edit_tag_menu->Append( MUSIK_PLAYLIST_CONTEXT_TAG_TITLE,		_( "Title\tF2" ) );
	playlist_context_edit_tag_menu->Append( MUSIK_PLAYLIST_CONTEXT_TAG_TRACKNUM,	_( "Track Number\tF3" ) );
	playlist_context_edit_tag_menu->Append( MUSIK_PLAYLIST_CONTEXT_TAG_ARTIST,		_( "Artist\tF4" ) );
	playlist_context_edit_tag_menu->Append( MUSIK_PLAYLIST_CONTEXT_TAG_ALBUM,		_( "Album\tF5" ) );
	playlist_context_edit_tag_menu->Append( MUSIK_PLAYLIST_CONTEXT_TAG_GENRE,		_( "Genre\tF6" ) );
	playlist_context_edit_tag_menu->Append( MUSIK_PLAYLIST_CONTEXT_TAG_YEAR,		_( "Year\tF7" ) );
	playlist_context_edit_tag_menu->AppendSeparator();
	playlist_context_edit_tag_menu->Append( MUSIK_PLAYLIST_CONTEXT_RENAME_FILES,	_( "&Auto Rename" ) );
	playlist_context_edit_tag_menu->Append( MUSIK_PLAYLIST_CONTEXT_RETAG_FILES,		_( "A&uto Retag" ) );

	//--- delete menu ---//
	playlist_context_delete_menu = new wxMenu;
	playlist_context_delete_menu->Append( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FROM_PLAYLIST,	_( "From Playlist\tDel" ) );
	playlist_context_delete_menu->Append( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FROM_DB,			_( "From Database\tAlt+Del" ) );
	playlist_context_delete_menu->Append( MUSIK_PLAYLIST_DELETE_CONTEXT_DELETE_FILES,			_( "From Computer\tCtrl+Del" ) );

	//--- columns context menu ---//
	playlist_context_display_menu = new wxMenu;
	playlist_context_display_menu->Append( MUSIK_PLAYLIST_DISPLAY_RATING,		_( "Rating" ),			wxT( "" ), wxITEM_CHECK );
	playlist_context_display_menu->Append( MUSIK_PLAYLIST_DISPLAY_TRACK,		_( "Track" ),			wxT( "" ), wxITEM_CHECK );
	playlist_context_display_menu->Append( MUSIK_PLAYLIST_DISPLAY_TITLE,		_( "Title" ),			wxT( "" ), wxITEM_CHECK );
	playlist_context_display_menu->Append( MUSIK_PLAYLIST_DISPLAY_ARTIST,		_( "Artist" ),			wxT( "" ), wxITEM_CHECK );
	playlist_context_display_menu->Append( MUSIK_PLAYLIST_DISPLAY_ALBUM,		_( "Album" ),			wxT( "" ), wxITEM_CHECK );
	playlist_context_display_menu->Append( MUSIK_PLAYLIST_DISPLAY_YEAR,			_( "Year" ),			wxT( "" ), wxITEM_CHECK );
	playlist_context_display_menu->Append( MUSIK_PLAYLIST_DISPLAY_GENRE,		_( "Genre" ),			wxT( "" ), wxITEM_CHECK );
	playlist_context_display_menu->Append( MUSIK_PLAYLIST_DISPLAY_TIMES_PLAYED,	_( "Times Played" ),	wxT( "" ), wxITEM_CHECK );
	playlist_context_display_menu->Append( MUSIK_PLAYLIST_DISPLAY_LAST_PLAYED,	_( "Last Played" ),		wxT( "" ), wxITEM_CHECK );
	playlist_context_display_menu->Append( MUSIK_PLAYLIST_DISPLAY_TIME,			_( "Time" ),			wxT( "" ), wxITEM_CHECK );
	playlist_context_display_menu->Append( MUSIK_PLAYLIST_DISPLAY_BITRATE,		_( "Bitrate" ),			wxT( "" ), wxITEM_CHECK );
	playlist_context_display_menu->Append( MUSIK_PLAYLIST_DISPLAY_FILENAME,		_( "Filename" ),		wxT( "" ), wxITEM_CHECK );
	playlist_context_display_menu->AppendSeparator();
	playlist_context_display_menu->Append( MUSIK_PLAYLIST_DISPLAY_SMART,		_( "Smart Resizing" ),	wxT( "" ), wxITEM_CHECK );

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

	SetActiveThread( NULL );
}

CPlaylistCtrl::~CPlaylistCtrl()
{
	delete playlist_context_menu;
}

void CPlaylistCtrl::SaveColumns()
{
	//---------------------------------------------------------//
	//--- get the total width of all the columns in pixels.	---//
	//--- this value will be used to calculate dynamic		---//
	//--- columns.											---//
	//---------------------------------------------------------//
	wxSize client_size = GetClientSize();

	size_t nCurrCol;
	float f_Pos;
	int n_Pos;
	for ( size_t i = 0; i < m_ColumnOrder.GetCount(); i++ )
	{
		nCurrCol = m_ColumnOrder.Item( i );
		
		//-----------------------------------------//
		//--- if this column is a static type	---//
		//-----------------------------------------//
		if ( g_Prefs.nPlaylistColumnDynamic[nCurrCol] == 0 )
		{
			//-------------------------------------------------//
			//--- overflow pixels get placed in the first	---//
			//--- column. remove them when we save.			---//
			//-------------------------------------------------//
			if ( i == 0 )
			{
				g_Prefs.nPlaylistColumnSize[nCurrCol] = GetColumnWidth( i ) - m_Overflow;
				 m_Overflow = 0;
			}
			else
				g_Prefs.nPlaylistColumnSize[nCurrCol] = GetColumnWidth( i );
		}

		//-----------------------------------------//
		//--- if this column is a dynamic type	---//
		//-----------------------------------------//
		else
		{
			f_Pos = (float)GetColumnWidth( i ) / (float)client_size.GetWidth() * 100.0f;
			n_Pos = (int)f_Pos;

			if ( n_Pos < 1 )
				n_Pos = 1;

			g_Prefs.nPlaylistColumnSize[nCurrCol] = n_Pos;
		}
	}
}

//--------------//
//--- events ---//
//--------------//
void CPlaylistCtrl::ShowMenu( wxCommandEvent& WXUNUSED(event) )
{
	wxPoint pos = ScreenToClient( wxGetMousePosition() );

	bool bItemSel = false;
	if ( GetSelectedItemCount() > 0 )
		bItemSel = true;

	playlist_context_menu->Enable( MUSIK_PLAYLIST_CONTEXT_DELETENODE,	bItemSel );
	playlist_context_menu->Enable( MUSIK_PLAYLIST_CONTEXT_RENAME_FILES, bItemSel );
	playlist_context_menu->Enable( MUSIK_PLAYLIST_CONTEXT_RETAG_FILES,	bItemSel );
	playlist_context_menu->Enable( MUSIK_PLAYLIST_CONTEXT_RATENODE,		bItemSel );
	playlist_context_menu->Enable( MUSIK_PLAYLIST_CONTEXT_TAGNODE,		bItemSel );

	if ( bItemSel )
	{
		//--- uncheck all ratings ---//
		playlist_context_rating_menu->Check( MUSIK_PLAYLIST_CONTEXT_UNRATED, false );
		playlist_context_rating_menu->Check( MUSIK_PLAYLIST_CONTEXT_RATE1, false );
		playlist_context_rating_menu->Check( MUSIK_PLAYLIST_CONTEXT_RATE2, false );
		playlist_context_rating_menu->Check( MUSIK_PLAYLIST_CONTEXT_RATE3, false );
		playlist_context_rating_menu->Check( MUSIK_PLAYLIST_CONTEXT_RATE4, false );
		playlist_context_rating_menu->Check( MUSIK_PLAYLIST_CONTEXT_RATE5, false );

		//--- get rating for first sel ---//
		int nFirst = g_Playlist.Item( GetNextItem( -1, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED ) ).Rating;
		switch ( nFirst )
		{
		case 0:
			playlist_context_rating_menu->Check( MUSIK_PLAYLIST_CONTEXT_UNRATED, true );
			break;
		case 1:
			playlist_context_rating_menu->Check( MUSIK_PLAYLIST_CONTEXT_RATE1, true );
			break;
		case 2:
			playlist_context_rating_menu->Check( MUSIK_PLAYLIST_CONTEXT_RATE2, true );
			break;
		case 3:
			playlist_context_rating_menu->Check( MUSIK_PLAYLIST_CONTEXT_RATE3, true );
			break;
		case 4:
			playlist_context_rating_menu->Check( MUSIK_PLAYLIST_CONTEXT_RATE4, true );
			break;
		case 5:
			playlist_context_rating_menu->Check( MUSIK_PLAYLIST_CONTEXT_RATE5, true );
			break;
		}
		
	}

	//--- check which columns are displayed ---//
	playlist_context_display_menu->Check( MUSIK_PLAYLIST_DISPLAY_RATING,		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_RATING]		);
	playlist_context_display_menu->Check( MUSIK_PLAYLIST_DISPLAY_TRACK,			g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TRACK]			);
	playlist_context_display_menu->Check( MUSIK_PLAYLIST_DISPLAY_TITLE,			g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TITLE]			);
	playlist_context_display_menu->Check( MUSIK_PLAYLIST_DISPLAY_ARTIST,		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_ARTIST]		);
	playlist_context_display_menu->Check( MUSIK_PLAYLIST_DISPLAY_ALBUM,			g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_ALBUM]			);
	playlist_context_display_menu->Check( MUSIK_PLAYLIST_DISPLAY_YEAR,			g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_YEAR]			);
	playlist_context_display_menu->Check( MUSIK_PLAYLIST_DISPLAY_GENRE,			g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_GENRE]			);
	playlist_context_display_menu->Check( MUSIK_PLAYLIST_DISPLAY_TIMES_PLAYED,	g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TIMES_PLAYED]	);
	playlist_context_display_menu->Check( MUSIK_PLAYLIST_DISPLAY_LAST_PLAYED,	g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_LAST_PLAYED]	);
	playlist_context_display_menu->Check( MUSIK_PLAYLIST_DISPLAY_TIME,			g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TIME]			);
	playlist_context_display_menu->Check( MUSIK_PLAYLIST_DISPLAY_BITRATE,		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_BITRATE]		);
	playlist_context_display_menu->Check( MUSIK_PLAYLIST_DISPLAY_FILENAME,		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_FILENAME]		);
	playlist_context_display_menu->Check( MUSIK_PLAYLIST_DISPLAY_SMART,			g_Prefs.nPlaylistSmartColumns								);

	PopupMenu( playlist_context_menu, pos );
}

void CPlaylistCtrl::OnDisplayMenu( wxCommandEvent& event )
{
	playlist_context_display_menu->Check( event.GetId(), !event.IsChecked() );

	switch ( event.GetId() )
	{
	case MUSIK_PLAYLIST_DISPLAY_RATING:
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_RATING] = !event.IsChecked();
		break;
	case MUSIK_PLAYLIST_DISPLAY_TRACK:
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TRACK] = !event.IsChecked();
		break;
	case MUSIK_PLAYLIST_DISPLAY_TITLE:
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TITLE] = !event.IsChecked();
		break;
	case MUSIK_PLAYLIST_DISPLAY_ARTIST:
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_ARTIST] = !event.IsChecked();
		break;
	case MUSIK_PLAYLIST_DISPLAY_ALBUM:
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_ALBUM] = !event.IsChecked();
		break;
	case MUSIK_PLAYLIST_DISPLAY_YEAR:
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_YEAR] = !event.IsChecked();
		break;
	case MUSIK_PLAYLIST_DISPLAY_GENRE:
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_GENRE] = !event.IsChecked();
		break;
	case MUSIK_PLAYLIST_DISPLAY_TIMES_PLAYED:
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TIMES_PLAYED] = !event.IsChecked();
		break;
	case MUSIK_PLAYLIST_DISPLAY_LAST_PLAYED:
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_LAST_PLAYED] = !event.IsChecked();
		break;
	case MUSIK_PLAYLIST_DISPLAY_TIME:
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_TIME] = !event.IsChecked();
		break;
	case MUSIK_PLAYLIST_DISPLAY_BITRATE:
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_BITRATE] = !event.IsChecked();
		break;
	case MUSIK_PLAYLIST_DISPLAY_FILENAME:
		g_Prefs.nPlaylistColumnEnable[PLAYLISTCOLUMN_FILENAME] = !event.IsChecked();
		break;
	}
	ResetColumns( false, true );
}

void CPlaylistCtrl::OnDisplaySmart( wxCommandEvent& event )
{
	if ( g_Prefs.nPlaylistSmartColumns == 0 )
		g_Prefs.nPlaylistSmartColumns = 1;
	else
		g_Prefs.nPlaylistSmartColumns = 0;

	RescaleColumns();
}

void CPlaylistCtrl::BeginDrag( wxEvent& WXUNUSED(event) )
{
	//--- let program know we're dragging ---//
	g_DragInProg = true;

	//--- pass selected items ---//
	wxString sDrop = wxT( "p\n" ) + GetSelFiles();
	DNDSetCurSel();

	//--- initialize drag and drop... SourcesDropTarget / PlaylistDropTarget should take care of the rest ---//
	wxDropSource dragSource( this );
	wxTextDataObject song_data( sDrop );
	dragSource.SetData( song_data );
	dragSource.DoDragDrop( TRUE );

	//--- tell program we're done dragging ---//
	g_DragInProg = false;
}

void CPlaylistCtrl::EndDragCol( wxListEvent& event )
{
//	wxMessageBox( wxT( "sux0r" ) );
	 
//	RescaleColumns();
//	event.Skip();
	Refresh();
}

void CPlaylistCtrl::PlaySel( wxListEvent& WXUNUSED(event) )
{
	// need to update to clear some repainting issues
	// Refresh( false );
	g_Player.PlayCurSel();
}

void CPlaylistCtrl::TranslateKeys( wxKeyEvent& pEvent )
{	 
	int nKeyCode = pEvent.GetKeyCode();
	if ( pEvent.AltDown() == TRUE )
	{
		switch( nKeyCode )
		{
			case WXK_DELETE:
			case WXK_BACK:
				DelSelFilesDB();
				break;
		}
	}
	else if ( pEvent.ControlDown() == TRUE )
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
			DelSelFiles();
			break;
		}
	}
	else if ( pEvent.AltDown() == FALSE )
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

	pEvent.Skip();
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
			return SanitizedString( song.Artist );
		break;

	case PLAYLISTCOLUMN_ALBUM:
		if ( song.Album == wxT( "<unknown>" ) )
			return wxT( "-" );
		else
			return SanitizedString( song.Album );
		break;

	case PLAYLISTCOLUMN_YEAR:
		if ( song.Year == wxT( "<unknown>" ) )
			return wxT( "-" );
		else
			return song.Year;
		break;

	case PLAYLISTCOLUMN_GENRE:
		if ( song.Genre == wxT( "<unknown>" ) )
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
	for ( int i = 0; i < NPLAYLISTCOLUMNS; i++ )
	{
		if ( g_Prefs.nPlaylistColumnEnable[i] == 1 )
			m_ColumnOrder.Add( i );
	}
}

int CPlaylistCtrl::OnGetItemImage(long item) const
{
	return g_Playlist.Item ( item ).Rating;
}

wxListItemAttr* CPlaylistCtrl::OnGetItemAttr(long item) const
{
	const CMusikSong & song = g_Playlist.Item ( item );
	if ( song.Filename == g_Player.GetCurrentFile() )
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
	sResult.Alloc( GetItemCount() * 30 ); // optimization ( the 30 is a wild guess)
	for ( int i = 0; i < GetItemCount(); i++ )
	{
		sResult +=  GetFilename( i );
		sResult += '\n';
	}
	//--- we don't need the last \n ---//
	return sResult.Left( sResult.Length() - 1 );	
}

wxString CPlaylistCtrl::GetSelFiles()
{
	wxString sResult;
	int nIndex = -1;
	for ( int i = 0; i < GetSelectedItemCount(); i++ )
	{
		nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
		if ( nIndex > -1 )
			sResult = sResult + GetFilename( nIndex ) + wxT( "\n" );
	}

	//--- we don't need the last \n ---//
	return sResult.Left( sResult.Length() - 1 );	
}

wxArrayInt CPlaylistCtrl::GetSelItems()
{
	wxArrayInt aResult;
	int nIndex = -1;
	for ( int i = 0; i < GetSelectedItemCount(); i++ )
	{
		nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
		if ( nIndex > -1 )
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
		if ( nIndex > -1 )
			aResult.Add( GetFilename( nIndex ) );
	}

	return;
}

void CPlaylistCtrl::GetAllFilesList(wxArrayString & aResult )
{
	aResult.Clear();
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
			if ( nIndex > -1 )
			{
				CMusikSong *pSong = new CMusikSong();
				g_Library.GetSongFromFilename( GetFilename( nIndex ), pSong );
				aResult.Add( pSong ); // array takes ownership of the pointer
			}
		}
	}
	return;
}

wxString CPlaylistCtrl::GetTotalFilesize()
{
	CNiceFilesize filesize;

	for ( int i = 0; i < GetItemCount(); i++ )
	{
		CMusikSong song = g_Playlist.Item( i );
		filesize.AddB( song.Filesize );
	}

	return filesize.GetFormatted();
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

//----------------------------------------------//
//--- functions to set playlist information. ---//
//----------------------------------------------//
void CPlaylistCtrl::SetSelFirst()
{
	// set selection of playlist to first item
	wxListCtrlSelNone( this );
	SetItemState( g_Player.GetCurIndex(), wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
	SetFocus();
	EnsureVisible( g_Player.GetCurIndex() );
	Refresh();
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

void CPlaylistCtrl::Update( bool bSelFirst, bool  bRescaleColumns)
{
	//----------------------------------------------------------------------------------//
	//---         note that the playlist control is now virtual, so we don't         ---//
	//---    add items directly to it.  Instead, we have MusikFrame::g_Playlist, a   ---//
	//---  CMusikSongArray, that the virtual listctrl then references via callbacks  ---//
	//----------------------------------------------------------------------------------//
	Freeze();

	//--- setup listbox colours from prefs	---//
	m_LightAttr			= wxListItemAttr( *wxBLACK, wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ), wxNullFont );
	m_SelectedLightAttr	= wxListItemAttr( *wxBLACK, wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ), g_fntListBold );
	m_DarkAttr			= wxListItemAttr( *wxBLACK, StringToColour( g_Prefs.sPLStripeColour ), wxNullFont );
	m_SelectedDarkAttr	= wxListItemAttr( *wxBLACK, StringToColour( g_Prefs.sPLStripeColour ), g_fntListBold );

	//--- SetItemCount() kinda tells the virtual list control to udpate ---//
	SetItemCount( ( long )g_Playlist.GetCount() );
	wxListCtrlSelNone( this );

	//--- sel first item, if we're supposed to ---//
	if ( bSelFirst )
		SetItemState( 0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );	
	Thaw();
	Refresh();
	if( bRescaleColumns )
		RescaleColumns();
	if ( g_Prefs.nShowPLInfo == 1 )
		g_PlaylistInfoCtrl->Update();
}

void CPlaylistCtrl::RescaleColumns( bool bFreeze )
{
	if ( g_DisablePlacement )
		return;
	if ( bFreeze )
		Freeze();

	SaveColumns();

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
	if ( g_Prefs.nPlaylistSmartColumns == 1 )
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

			if ( g_Prefs.nPlaylistSmartColumns == 1 )
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
	if ( g_Prefs.nPlaylistSmartColumns == 1 && nTotalPercent && nDynamicWidth )
	{
		m_Overflow = client_size.GetWidth() - ( nStaticWidth + nDynamicWidth );
		size_t nLastSize = GetColumnWidth( n_LastDyn ) + m_Overflow;
		SetColumnWidth( n_LastDyn, nLastSize );
	}

	//-------------------------------------------------//
	//--- make sure window is properly refreshed.	---//
	//-------------------------------------------------//
	if ( bFreeze )
		Thaw();

	Refresh( false );
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
	for ( int i = 0; i < GetSelectedItemCount(); i++ )
	{
		nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
		if ( nIndex > -1 )
		{
			//--- set db entry, then resync item(s) ---//
			g_Library.SetRating( GetFilename( nIndex ), nVal );
			ResynchItem( nIndex, -1, false );
		}
	}
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
		g_MusikTagFrame = new MusikTagFrame( g_MusikFrame, songs, i, nEditType, nIndex );
		g_MusikFrame->Enable( FALSE );
		g_MusikTagFrame->Show();
	}
}

void CPlaylistCtrl::DelSelSongs()
{
	Freeze();

	int nIndex = -1;
	int nSelCount = GetSelectedItemCount();

	//--- find which songs are selected, delete as we go along ---//
	int nFirstSel = 0;
	int n = 0;
	for ( int i = 0; i < nSelCount; i++ )
	{
		nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
		if ( nIndex > -1 )
		{
			if ( i == 0 )
				nFirstSel = nIndex;

			g_Playlist.RemoveAt( nIndex - n, 1 );
			g_PlaylistChanged = true;

			n++;
		}
	}

	Update( false );

	//--- select the last known item ---//
	if ( nFirstSel > ( GetItemCount() - 1 ) )
		nFirstSel = GetItemCount() - 1;
	SetItemState( nFirstSel, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );

	Thaw();

	g_ActivityAreaCtrl->ResetAllContents();
}

void CPlaylistCtrl::DelSelFilesDB()
{
	wxMessageDialog confirm( this, _( "Delete the selected songs from Musik's internal database?" ), MUSIK_VERSION, wxYES_NO | wxICON_QUESTION );
	if ( confirm.ShowModal() == wxID_NO )
		return;
	else
	{
		Freeze();
		int nIndex = -1;
		int nSelCount = GetSelectedItemCount();

		//--- find which songs are selected, delete as we go along ---//
		wxString sFile;
		nIndex = -1;
		int n = 0;
		int nFirstSel = 0;
		for ( int i = 0; i < nSelCount; i++ )
		{
			//--- find next item to delete ---//
			nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );

			//--- if its valid, delete ---//
			if ( nIndex > -1 )
			{
				sFile = g_Playlist.Item( nIndex - n ).Filename;
				g_Library.RemoveSong( sFile );
				g_Playlist.RemoveAt( nIndex - n, 1 );
				g_PlaylistChanged = true;

				if ( i == 0 )
					nFirstSel = nIndex;

				n++;
			}
		}

		Update( false );

		//--- select the last known item ---//
		if ( nFirstSel > ( GetItemCount() - 1 ) )
			nFirstSel = GetItemCount() - 1;
		SetItemState( nFirstSel, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );

		Thaw();

		g_ActivityAreaCtrl->ResetAllContents();
	}
}

void CPlaylistCtrl::DelSelFiles()
{
	wxMessageDialog confirm( this, _( "Delete the selected songs from your computer?" ), MUSIK_VERSION, wxYES_NO | wxICON_STOP );
	if ( confirm.ShowModal() == wxID_NO )
		return;
	else
	{
		wxString sError = wxT( "" );
		Freeze();
		int nIndex = -1;
		int nSelCount = GetSelectedItemCount();

		//--- find which songs are selected, delete as we go along ---//
		wxString sFile;
		nIndex = -1;
		int n = 0;
		int nFirstSel = 0;
		for ( int i = 0; i < nSelCount; i++ )
		{
			//--- find next item to delete ---//
			nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );

			//--- if its valid, delete ---//
			if ( nIndex > -1 )
			{
				sFile = g_Playlist.Item( nIndex - n ).Filename;
				g_Playlist.RemoveAt( nIndex - n, 1 );
				g_PlaylistChanged = true;

				if ( wxRemoveFile( sFile ) )
					g_Library.RemoveSong( sFile );
				else
					sError = sError + sFile + wxT( "\n" );

				if ( i == 0 )
					nFirstSel = nIndex;

				n++;
			}

		}

		//--- if certain files couldn't be deleted ---//
		if ( sError != wxT( "" ) )
			wxMessageBox( _( "Failed to delete the following files:\n\n " ) + sError, MUSIK_VERSION, wxICON_STOP );

		Update( false );

		//--- select the last known item ---//
		if ( nFirstSel > ( GetItemCount() - 1 ) )
			nFirstSel = GetItemCount() - 1;
		SetItemState( nFirstSel, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );

		g_ActivityAreaCtrl->ResetAllContents();

		Thaw();
	}
}


void CPlaylistCtrl::RenameSelFiles()
{
	if ( GetActiveThread() == 0 )
	{
		CMusikSongArray songs;
		GetSelSongs( songs );
		pRenameThread = new MusikPlaylistRenameThread( songs );
		pRenameThread->Create();
		pRenameThread->Run();
	}
	else
		wxMessageBox( _( "An internal error has occured.\nPrevious thread not terminated correctly.\n\nPlease contact the Musik development team with this error." ), MUSIK_VERSION, wxICON_STOP );

}

void CPlaylistCtrl::RetagSelFiles()
{
	if ( GetActiveThread() == 0 )
	{
		CMusikSongArray songs;
		GetSelSongs( songs );
		pRetagThread = new MusikPlaylistRetagThread( songs );
		pRetagThread->Create();
		pRetagThread->Run();
	}
	else
		wxMessageBox( _( "An internal error has occured.\nPrevious thread not terminated correctly.\n\nPlease contact the Musik development team with this error." ), MUSIK_VERSION, wxICON_STOP );
}

bool CPlaylistCtrl::ViewDirtyTags()
{
	CMusikSongArray dirty;
	g_Library.QuerySongs( wxT( "dirty = 1" ), dirty );
	if ( dirty.GetCount() > 0 )
	{
		g_LibPlaylist = g_Playlist;
		g_Playlist = dirty;
		Update();
		return true;
	}
	else
		wxMessageBox( _( "There are no pending tags available to display." ), MUSIK_VERSION, wxICON_INFORMATION );

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
			if ( nIndex > -1 )
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
void CPlaylistCtrl::OnThreadStart( wxCommandEvent& WXUNUSED(event) )
{
	//--- setup thread to begin in g_MusikFrame ---//
	g_MusikFrame->SetActiveThread	( GetActiveThread() );
	g_MusikFrame->SetProgressType	( GetProgressType() );
	g_MusikFrame->SetProgress		( GetProgress() );

	//--- post the event. we're up and running now! ---//
	wxCommandEvent MusikStartProgEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_FRAME_THREAD_START );
	wxPostEvent( g_MusikFrame, MusikStartProgEvt );
}

void CPlaylistCtrl::OnThreadProg( wxCommandEvent& WXUNUSED(event) )
{
	//--- relay thread progress message to g_MusikFrame ---//
	g_MusikFrame->SetProgress( GetProgress() );

	wxCommandEvent MusikEndProgEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_FRAME_THREAD_PROG );
	wxPostEvent( g_MusikFrame, MusikEndProgEvt );
}

void CPlaylistCtrl::OnThreadEnd( wxCommandEvent& WXUNUSED(event) )
{
	if( GetProgressType() == MUSIK_PLAYLIST_RETAG_THREAD )
		g_ActivityAreaCtrl->ResetAllContents();

	g_PlaylistCtrl->Update();

	//--- update locally ---//
	SetActiveThread	( NULL );
	SetProgressType	( 0 );
	SetProgress	( 0 );

    //--- relay thread end message to g_MusikFrame ---//
	wxCommandEvent MusikEndProgEvt( wxEVT_COMMAND_MENU_SELECTED, MUSIK_FRAME_THREAD_END );
	wxPostEvent( g_MusikFrame, MusikEndProgEvt );
}
