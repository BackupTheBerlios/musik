/*
 *  SourcesBox.cpp
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

//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include "SourcesBox.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

//--- frames ---//
#include "../Frames/MusikFrame.h"

//--- file ops ---//
#include <wx/textfile.h>

//-------------------------//
//--- SourcesDropTarget ---//
//-------------------------//
bool SourcesDropTarget::OnDropText( wxCoord x, wxCoord y, const wxString &text )
{
	wxString sFiles = text;
	long n = -1;
	if ( sFiles != wxT( "" ) )
	{
		//--- seperate values. "s\n" for sources, "p\n" for playlist, "a\n" for activity box ---//
		wxString sType = sFiles.Left( 2 );
		sFiles = sFiles.Right( sFiles.Length() - 2 );

		//--- where did we land? ---//
		const wxPoint& pt = wxPoint( x, y );
		int nFlags;
		n = m_SourcesListBox->HitTest( pt, nFlags );

		//--- originated from sources box, so rearrange ---//
		if ( sType == wxT( "s\n" ) )		
		{
			m_SourcesListBox->Freeze();

			
			if ( n == m_SourcesListBox->GetDragIndex() )
				return TRUE;
			else
			{
				g_SourcesList.RemoveAt	( m_SourcesListBox->GetDragIndex() );

			//--- if its -1, we will just push it to the bottom ---//
			if ( n == -1 )
				g_SourcesList.Insert	( sFiles, g_SourcesList.GetCount() );
	
			//--- dragged above old pos, insert, push down... old item is + 1 ---//
			else if ( n < m_SourcesListBox->GetDragIndex() )		
				g_SourcesList.Insert	( sFiles, n );

			//--- dragged below old pos, insert there, then push all others down ---//
			else if ( n > m_SourcesListBox->GetDragIndex() )	
					g_SourcesList.Insert	( sFiles, n );
			}
			
			//--- update ---//
			m_SourcesListBox->Thaw();
		}

		//--- drag originated from playlist or activity box, create new playlist ---//
		else if ( sType == wxT( "p\n" ) || sType == wxT( "a\n" ) )	
		{
			//--- drag not over an object, create new list ---//
			if ( n == -1 )
			{
				wxTextEntryDialog dlg( g_MusikFrame, _( "Enter name for new playlist:" ), MUSIK_VERSION, wxT( "" ) );
				if ( dlg.ShowModal() == wxID_OK )
				{
					wxString sName = dlg.GetValue();
					m_SourcesListBox->NewPlaylist( sName, sFiles, MUSIK_SOURCES_PLAYLIST_STANDARD );		
				}
				n = 0;
			}

			//--- drag over library, can't do that ---//
			else if ( m_SourcesListBox->GetType( n ) == MUSIK_SOURCES_LIBRARY )
			{
				wxMessageBox( _( "Cannot drag songs into the library, they already exist here." ), MUSIK_VERSION, wxOK | wxICON_INFORMATION );
			}
			else if ( m_SourcesListBox->GetType( n ) == MUSIK_SOURCES_NOW_PLAYING )
			{
				wxMessageBox( _( "Cannot drag songs into Now Playing." ), MUSIK_VERSION, wxOK | wxICON_INFORMATION );
			}
			else if ( m_SourcesListBox->GetType( n ) == MUSIK_SOURCES_NETSTREAM )
			{
				wxMessageBox( _( "Cannot drag songs into a net stream." ), MUSIK_VERSION, wxOK | wxICON_INFORMATION );
			}

			//--- drag over an existing item, append ---//
			else if ( n > 0 )
			{
				wxString sName = m_SourcesListBox->GetItemText( n );
				m_SourcesListBox->AppendStdPlaylist( sName, sFiles );
			}
		}
	}

	//--- select nothing, return ---//
	if ( n == -1 )
	{
		n = m_SourcesListBox->GetItemCount()-1;
	}
	else
	{
		wxListCtrlSelNone( m_SourcesListBox );
		m_SourcesListBox->SetItemState( n, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
	}

	return TRUE;
}

wxDragResult SourcesDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult WXUNUSED(def))
{
	const wxPoint& pt = wxPoint( x, y );
	HighlightSel( pt );
	return wxDragMove;
}

void SourcesDropTarget::HighlightSel( wxPoint pPos )
{
	g_DragInProg = true;
	int nFlags;
	long n = m_SourcesListBox->HitTest( pPos, nFlags );
	if ( n != nLastHit )
	{
		wxListCtrlSelNone( m_SourcesListBox );
		m_SourcesListBox->SetItemState( n, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
	}
	nLastHit = n;
}

//-----------------------//
//--- CSourcesListBox ---//
//-----------------------//
BEGIN_EVENT_TABLE(CSourcesListBox, CMusikListCtrl)
	EVT_CONTEXT_MENU			(												CSourcesListBox::ShowMenu				)
	EVT_MENU					(MUSIK_SOURCE_CONTEXT_CREATE_CURRENT_PLAYLIST,	CSourcesListBox::CreateCurPlaylist		)	// Sources Context -> Create -> Create from Current
	EVT_MENU					(MUSIK_SOURCE_CONTEXT_STANDARD_PLAYLIST,		CSourcesListBox::StandardPlaylist		)	// Sources Context -> Create -> Standard Playlist
	EVT_MENU					(MUSIK_SOURCE_CONTEXT_DYNAMIC_PLAYLIST,			CSourcesListBox::DynamicPlaylist		)	// Sources Context -> Create -> Dynamic Playlist
	EVT_MENU					(MUSIK_SOURCE_CONTEXT_CREATE_NETSTREAM,			CSourcesListBox::NetStream				)	// Sources Context -> Create -> Net Stream
	EVT_MENU					(MUSIK_SOURCE_CONTEXT_EDIT_QUERY,				CSourcesListBox::EditQuery				)	// Sources Context -> Edit Query
	EVT_MENU					(MUSIK_SOURCE_CONTEXT_DELETE,					CSourcesListBox::Delete					)	// Sources Context -> Delete
	EVT_MENU					(MUSIK_SOURCE_CONTEXT_RENAME,					CSourcesListBox::Rename					)	// Sources Context -> Rename
	EVT_MENU					(MUSIK_SOURCE_CONTEXT_SHOW_ICONS,				CSourcesListBox::ToggleIconsEvt			)	// Sources Context -> Show Icons
	EVT_MENU					(MUSIK_SOURCE_CONTEXT_COPY_FILES,				CSourcesListBox::CopyFiles				)	// Sources Context -> Copy files
	EVT_LIST_BEGIN_DRAG			(MUSIK_SOURCES,									CSourcesListBox::BeginDrag				)	// user drags files from sources
	EVT_LIST_ITEM_SELECTED		(MUSIK_SOURCES,									CSourcesListBox::OnUpdateSel			)	// sources list sel
	EVT_LIST_BEGIN_LABEL_EDIT	(MUSIK_SOURCES,									CSourcesListBox::BeginEditLabel			)   // user edits a playlist filename
	EVT_LIST_END_LABEL_EDIT		(MUSIK_SOURCES,									CSourcesListBox::EndEditLabel			)   // user edits a playlist filename
	EVT_LIST_KEY_DOWN			(MUSIK_SOURCES,									CSourcesListBox::TranslateKeys			)	// user presses a key in the sources list
	EVT_LIST_ITEM_ACTIVATED		(MUSIK_SOURCES,									CSourcesListBox::OnUpdateSel			)	// user wants to redisplay playlist
	EVT_LIST_COL_BEGIN_DRAG		(MUSIK_SOURCES,									CSourcesListBox::OnSourcesColSize		)
END_EVENT_TABLE()

CSourcesListBox::CSourcesListBox( wxWindow* parent )
	: CMusikListCtrl( parent, MUSIK_SOURCES, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxLC_ALIGN_LEFT |wxLC_EDIT_LABELS | wxLC_SINGLE_SEL | wxNO_BORDER)
{
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ) );

	//--- initialize variables ---//
	m_CurSel = 0;
	m_DragIndex	= -1;

	//--- setup context menus ---//
	sources_context_menu_new = new wxMenu;
	sources_context_menu_new->Append( MUSIK_SOURCE_CONTEXT_CREATE_CURRENT_PLAYLIST, _( "Playlist from Current" ) );
	sources_context_menu_new->AppendSeparator();
	sources_context_menu_new->Append( MUSIK_SOURCE_CONTEXT_STANDARD_PLAYLIST, _( "Standard Playlist" ) );
	sources_context_menu_new->Append( MUSIK_SOURCE_CONTEXT_DYNAMIC_PLAYLIST, _( "Dynamic Playlist" ) );
	sources_context_menu_new->Append( MUSIK_SOURCE_CONTEXT_CREATE_NETSTREAM, _( "Net Stream" ) );

	sources_context_menu = new wxMenu;
	sources_context_menu->Append( MUSIK_SOURCE_CONTEXT_CREATE, _( "&Create New" ), sources_context_menu_new );
	sources_context_menu->Append( MUSIK_SOURCE_CONTEXT_EDIT_QUERY, _( "&Edit Query" ) );
	sources_context_menu->Append( MUSIK_SOURCE_CONTEXT_RENAME, _( "&Rename" ) );
	sources_context_menu->Append( MUSIK_SOURCE_CONTEXT_DELETE, _( "&Delete" ) );
	sources_context_menu->AppendSeparator();
	sources_context_menu->Append( MUSIK_SOURCE_CONTEXT_SHOW_ICONS, _( "&Show Icons" ), wxT( "" ), wxITEM_CHECK );
	sources_context_menu->Check( MUSIK_SOURCE_CONTEXT_SHOW_ICONS, ( bool )g_Prefs.nShowSourcesIcons );
	sources_context_menu->AppendSeparator();
	sources_context_menu->Append( MUSIK_SOURCE_CONTEXT_COPY_FILES, _("Copy files to directory") );
	

	InsertColumn( 0, _( "Sources" ), wxLIST_FORMAT_LEFT );
	SetDropTarget( new SourcesDropTarget( this ) );

	m_Deleting = false;

	ShowIcons();

	Load();
	Update();
}

CSourcesListBox::~CSourcesListBox()
{
	Save();
	delete sources_context_menu;
}

void CSourcesListBox::ShowMenu( wxContextMenuEvent &WXUNUSED(event) )
{
	wxPoint pos = ScreenToClient( wxGetMousePosition() );

	int nSelIndex = GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	
	//---------------------------------------------------------//
	//--- if a dynamic playlist is selected, it can have	---//
	//--- its query edited. other playlists cannot.			---//
	//---------------------------------------------------------//
	sources_context_menu->Enable( MUSIK_SOURCE_CONTEXT_EDIT_QUERY, GetSelType() == MUSIK_SOURCES_PLAYLIST_DYNAMIC );

	//---------------------------------------------------------//
	//--- if a library is selected, it can't be deleted		---//
	//---------------------------------------------------------//
	sources_context_menu->Enable( MUSIK_SOURCE_CONTEXT_DELETE, (GetSelType() != MUSIK_SOURCES_LIBRARY) && (GetSelType() != MUSIK_SOURCES_NOW_PLAYING) );
	
	sources_context_menu->Enable( MUSIK_SOURCE_CONTEXT_RENAME, nSelIndex !=  -1 );

	PopupMenu( sources_context_menu, pos );
}

void CSourcesListBox::CreateCurPlaylist( wxCommandEvent& WXUNUSED(event) )
{
	wxTextEntryDialog dlg( this, _( "Enter name for new playlist:" ), MUSIK_VERSION, wxT( "" ) );
	if ( dlg.ShowModal() == wxID_OK )
	{
		wxString sName = dlg.GetValue();
		NewPlaylist( sName, g_PlaylistBox->PlaylistCtrl().GetAllFiles(), MUSIK_SOURCES_PLAYLIST_STANDARD );
	}
}

void CSourcesListBox::StandardPlaylist( wxCommandEvent& WXUNUSED(event) )
{
	wxTextEntryDialog dlg( this, _( "Enter name for new playlist:" ), MUSIK_VERSION, wxT( "" ) );
	if ( dlg.ShowModal() == wxID_OK )
	{
		wxString sName = dlg.GetValue();
		NewPlaylist( sName, wxT( "" ), MUSIK_SOURCES_PLAYLIST_STANDARD );
	}
}

void CSourcesListBox::DynamicPlaylist( wxCommandEvent& WXUNUSED(event) )
{
	wxTextEntryDialog dlg( this, _( "Enter name for new dynamic playlist:" ), MUSIK_VERSION, wxT( "" ) );
	if ( dlg.ShowModal() == wxID_OK )
	{
		wxString sName = dlg.GetValue();
		NewPlaylist( sName, wxT( "" ), MUSIK_SOURCES_PLAYLIST_DYNAMIC );
	}
}
void CSourcesListBox::NetStream( wxCommandEvent& WXUNUSED(event) )
{
	wxTextEntryDialog dlg( this, _( "Enter name of the net stream:" ), MUSIK_VERSION, wxT( "" ) );
	if ( dlg.ShowModal() == wxID_OK )
	{
		wxString sName = dlg.GetValue();
		NewPlaylist( sName, wxT( "" ), MUSIK_SOURCES_NETSTREAM );
	}
}
void CSourcesListBox::EditQuery( wxCommandEvent& WXUNUSED(event) )
{
	int nSelPos = GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
	UpdateDynPlaylist( nSelPos );
}

void CSourcesListBox::ToggleIconsEvt( wxCommandEvent& WXUNUSED(event) )
{
	ToggleIcons();
}

/*
What could be improved here:
-Option to prepend a numerical value to the destination filename to maintain 
 the same order as the playlist
-Option to create a directory in the destination directory based on playlist name
-A progress dialog
SiW
*/
void CSourcesListBox::CopyFiles( wxCommandEvent& WXUNUSED(event) )
{
	//--------------------------------//
	//--- first choose a directory ---//
	//--------------------------------//
	wxString destdir;
	wxDirDialog dirdlg( this, _("Please choose location to copy songs to:"), _(""), wxDD_NEW_DIR_BUTTON );
	if ( dirdlg.ShowModal() == wxID_OK )
		destdir = dirdlg.GetPath();
	else
		return;

	//-----------------------------------------------------//
	//--- now just loop through the files and copy them ---//
	//-----------------------------------------------------//
	wxArrayString filenames;
	g_PlaylistBox->PlaylistCtrl().GetAllFilesList( filenames );

	wxString sourcebasename, sourceext;
	for ( size_t n = 0; n < filenames.GetCount(); n++ )
	{
		wxString sourcename( filenames.Item( n ) );
		wxFileName::SplitPath( sourcename, NULL, NULL, &sourcebasename, &sourceext );

		//--------------------------------------------------//
		//--- Note that the / will still work on Windows ---//
		//--------------------------------------------------//
		wxString destname( destdir + _("/") + sourcebasename + _(".") + sourceext );
		wxCopyFile( sourcename, destname );
	}

}

void CSourcesListBox::BeginDrag( wxListEvent &event )
{
	if ( m_CurSel != -1 )
	{
		g_DragInProg = true;

		//-------------------------//
		//--- get selected item	---//
		//-------------------------//
		m_DragIndex		= event.GetIndex();
		wxString sDrop	= wxT( "s\n" ) + g_SourcesList.Item( m_DragIndex );

		//------------------------------------------------------//
		//--- initialize drag and drop                       ---//
		//--- SourcesDropTarget should take care of the rest ---//
		//------------------------------------------------------//
		wxDropSource dragSource( this );
		wxTextDataObject playlist_data( sDrop );
		dragSource.SetData( playlist_data );
		dragSource.DoDragDrop( TRUE );

		Update();
		m_DragIndex		= -1;
		g_DragInProg = false;
	}
}

void CSourcesListBox::OnUpdateSel( wxListEvent& pEvent )
{
	if ( !g_DragInProg && !g_FirstRun && !m_Deleting )
        UpdateSel( pEvent.GetIndex() );	
}

void CSourcesListBox::UpdateSel( size_t index )
{

	///HACK///
	static bool bInFunction = false;
	if(	bInFunction )
		return;
	bInFunction = true;
	if(index == (size_t)-2)
	{	// protect playlists from being accidently changed
		wxListCtrlSelNone( this );
        m_CurSel = FindInSources(wxT( "Musik Library" ),MUSIK_SOURCES_LIBRARY);
		SetItemState( m_CurSel, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
		g_PlaylistChanged = true; 
		bInFunction = false;
		return;
    }
	//--- save std playlist. if deleting, no need to worry ---//
	int nLastSel = m_CurSel;
	if ( !m_Deleting )
	{
		if ( GetType( nLastSel ) == MUSIK_SOURCES_PLAYLIST_STANDARD )
			RewriteStdPlaylist();
	}
	
	m_CurSel = index;
	int nSelType = GetSelType();

	if ( m_CurSel == -1 )
	{
		wxListCtrlSelNone( this );
		SetItemState( nLastSel, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
		m_CurSel = nLastSel;
	}

	//--- library selected ---//
	else if ( nSelType == MUSIK_SOURCES_LIBRARY )
	{
		//--- show old songs ---//
		if ( g_Prefs.nShowAllSongs == 0 )
			g_Playlist = g_LibPlaylist;

		//--- show all songs ---//
		else if ( g_Prefs.nShowAllSongs == 1 && !g_FirstRun )
			g_Library.GetAllSongs( g_Playlist );

		g_LibPlaylist.Clear();
		g_PlaylistBox->Update();
		g_MusikFrame->ShowActivityArea( g_Prefs.nShowActivities );
		
	}

	else 
	{
		//--- backup playlist so we can revert later ---//
		g_LibPlaylist = g_Playlist;

		//--- standard playlist selected ---//
		if (m_CurSel != -1 && nSelType == MUSIK_SOURCES_PLAYLIST_STANDARD )
		{
			wxArrayString aFilelist;
			LoadStdPlaylist( GetItemText( m_CurSel ), aFilelist );
			g_Library.GetStdPlaylistSongs( aFilelist, g_Playlist );
		}

		//--- dynamic playlist selected ---//
		else if (  m_CurSel != -1 && nSelType == MUSIK_SOURCES_PLAYLIST_DYNAMIC )
		{
			wxString sQuery = LoadDynPlaylist( GetItemText( m_CurSel ) );
			 g_Library.QuerySongs( sQuery, g_Playlist );
		}
		else if (  m_CurSel != -1 && nSelType == MUSIK_SOURCES_NOW_PLAYING )
		{
			g_Playlist = g_Player.GetPlaylist();
		}
		else if ( m_CurSel != -1 && nSelType == MUSIK_SOURCES_NETSTREAM )
		{
			CMusikSong song;
			LoadNetStream(GetItemText( m_CurSel ), song);
			g_Playlist.Clear();
			g_Playlist.Add(song);
		}

		//--- update ui with new list ---//
		g_PlaylistBox->Update(true);
		if ( m_CurSel != -1 && nSelType == MUSIK_SOURCES_NOW_PLAYING )
		{
			g_PlaylistBox->PlaylistCtrl().EnsureVisible(g_Player.GetCurIndex());
		}
		g_MusikFrame->ShowActivityArea( false );
		
	}

	g_PlaylistChanged = true; 
	bInFunction = false;
}

void CSourcesListBox::BeginEditLabel( wxListEvent& pEvent )
{
	m_SourcesEditStr	= pEvent.GetText();
	m_SourcesEditIndex	= pEvent.GetIndex();
}

void CSourcesListBox::EndEditLabel( wxListEvent& pEvent )
{
#if wxMINOR_VERSION > 4
	if(pEvent.IsEditCancelled())
#else
	if(pEvent.Cancelled())
#endif
		return;
	int nType = GetType( m_SourcesEditIndex );

	//--- Musik Library entry edited ---//
	if ( nType == MUSIK_SOURCES_LIBRARY )
	{
        wxString sCheck = pEvent.GetText();
		sCheck.Replace( wxT( " " ), wxT( "" ), TRUE );
		if ( sCheck != wxT( "" ) )
			g_SourcesList.Item( m_SourcesEditIndex ) = wxT( "[l] " ) + pEvent.GetText();
	}
	//--- Now Playing entry edited ---//
	if ( nType == MUSIK_SOURCES_NOW_PLAYING )
	{
        wxString sCheck = pEvent.GetText();
		sCheck.Replace( wxT( " " ), wxT( "" ), TRUE );
		if ( sCheck != wxT( "" ) )
			g_SourcesList.Item( m_SourcesEditIndex ) = wxT( "[n] " ) + pEvent.GetText();
	}
	//--- standard or dyanmic playlist renamed ---//
	if ( nType == MUSIK_SOURCES_PLAYLIST_STANDARD || nType == MUSIK_SOURCES_PLAYLIST_DYNAMIC )
	{
		//--- rename file ---//
		wxString sOldFile = m_SourcesEditStr;
		wxString sNewFile = pEvent.GetText();
		
		SourcesToFilename( &sOldFile, nType );
		SourcesToFilename( &sNewFile, nType );

		wxRenameFile( sOldFile, sNewFile );

		//--- get str of type ---//
		wxString sType;
		switch ( nType)
		{
		case MUSIK_SOURCES_PLAYLIST_STANDARD:
			sType = wxT( "[s] " );
		case MUSIK_SOURCES_PLAYLIST_DYNAMIC:
			sType = wxT( "[d] " );
		case MUSIK_SOURCES_NETSTREAM:
			sType = wxT( "[u] " );
		}

		//--- rename in musiksources.dat ---//
		g_SourcesList.Item( m_SourcesEditIndex ) = sType + pEvent.GetText();
	}
}

void CSourcesListBox::TranslateKeys( wxListEvent& event )
{
	if ( event.GetKeyCode() == WXK_DELETE || event.GetKeyCode() == WXK_BACK )
		DelSel();
	else if ( event.GetKeyCode() == WXK_F2 )
		RenameSel();
	else
		event.Skip();
}

void CSourcesListBox::Create()
{
	wxTextFile Out( MUSIK_SOURCES_FILENAME );

	Out.Create();
/*	if ( Out.Open() )
	{
		Out.AddLine( wxT( "[l] Musik Library" ) );
		Out.Write();
		Out.Close();
	}
*/
}

void CSourcesListBox::Load()
{
	if ( !wxFileExists( MUSIK_SOURCES_FILENAME ) )
		Create();

	g_SourcesList = FileToStringArray( MUSIK_SOURCES_FILENAME );

	int i = 0;
	for(i = 0; i < g_SourcesList.GetCount();i++)
	{
		if(g_SourcesList[i].Left(3) == wxT( "[n]" ))
			break;
	}
	if((g_SourcesList.GetCount() == i) )
	{// no [n] found
		g_SourcesList.Insert(wxT( "[n] Now Playing"),0);
	}
	for(i = 0; i < g_SourcesList.GetCount();i++)
	{
		if(g_SourcesList[i].Left(3) == wxT( "[l]" ))
			break;
	}
	if((g_SourcesList.GetCount() == i) )
	{// no [l] found
		g_SourcesList.Insert(wxT( "[l] Musik Library"), 0);
	}
}

void CSourcesListBox::Save()
{
	// SiW - removed this because it was causing the loss of playlists
	// when exiting.. any side effects?
	//if ( GetType( m_CurSel ) == MUSIK_SOURCES_PLAYLIST_STANDARD )
	//		RewriteStdPlaylist();

	if ( wxFileExists( MUSIK_SOURCES_FILENAME ) )
		wxRemoveFile( MUSIK_SOURCES_FILENAME );
	
	wxTextFile Out( MUSIK_SOURCES_FILENAME );
	Out.Create();
	if ( Out.Open() )
	{
		for ( size_t i = 0; i < g_SourcesList.GetCount(); i++ )
			Out.AddLine( g_SourcesList.Item( i ) );
		Out.Write( Out.GuessType() );
		Out.Close();
	}
}

//---------------------------------------------------//
//--- virtual functions. careful with these ones, ---// 
//---        boys. they drive everything          ---//
//---------------------------------------------------//
wxString CSourcesListBox::OnGetItemText(long item, long column) const
{
	wxString sItem = g_SourcesList.Item( item ).Right( g_SourcesList.Item( item ).Length() - 3 );
	sItem.Trim( true ); sItem.Trim( false );

	switch ( column )
	{
	case 0:
		return sItem;
		break;
	}

	return wxT("");
}

wxListItemAttr* CSourcesListBox::OnGetItemAttr(long item) const
{
	wxString type = g_SourcesList.Item( item ).Left( 3 );
	type.MakeLower();

	//--- stripes ---//
	if ( g_Prefs.nSourcesStripes == 1 )
	{
		//--- even ---//
		if ( item % 2 == 0 )
		{
			if ( type == wxT( "[l]" ) )
				return ( wxListItemAttr* )&m_LightBold;
			else
				return ( wxListItemAttr* )&m_Light;
		}
		else
		{
			if ( type == wxT( "[l]" ) )
				return ( wxListItemAttr* )&m_DarkBold;
			else
				return ( wxListItemAttr* )&m_Dark;
		}
	}

	//--- no stripes ---//
	if ( type == wxT( "[l]" ) )
		return ( wxListItemAttr* )&m_LightBold;

	return ( wxListItemAttr* )&m_Light;
}

int	CSourcesListBox::OnGetItemImage	(long item) const
{

	return GetType(item);	
}

void CSourcesListBox::Update()
{
	//----------------------------------------------------------------------------------//
	//---        note that the sources box control is now virtual, so we don't		 ---//
	//---    add items directly to it.  Instead, we have the object pItems a		 ---//
	//---  *wxArrayString, that the virtual listctrl then references via callbacks   ---//
	//----------------------------------------------------------------------------------//

	m_Light		= wxListItemAttr( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ), wxNullFont	);
	m_LightBold	= wxListItemAttr( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT), wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ), g_fntListBold		);
	m_Dark		= wxListItemAttr( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT), StringToColour( g_Prefs.sSourcesStripeColour ), wxNullFont );
	m_DarkBold	= wxListItemAttr( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT), StringToColour( g_Prefs.sSourcesStripeColour ), g_fntListBold );

	SetItemCount( g_SourcesList.GetCount() );
	RescaleColumns();

}


void CSourcesListBox::DelSel()
{
	//-----------------------------------------------------//
	//--- the EVT_LIST_ITEM_SELECTED will check this	---//
	//--- as there is no need to update the sel until	---//
	//--- we're done									---//
	//-----------------------------------------------------//
	m_Deleting = true;

	//--- where are we selected? ---//
	int nIndex = - 1;
	nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );

	int nType = GetType( nIndex );

	//--- no item selected? bail ---//
	if ( nIndex == - 1 || nType == MUSIK_SOURCES_LIBRARY || nType == MUSIK_SOURCES_NOW_PLAYING )
		return;

	//--- if we have a standard or dynamic playlist, delete the file ---//
	if ( nType == MUSIK_SOURCES_PLAYLIST_STANDARD || nType == MUSIK_SOURCES_PLAYLIST_DYNAMIC || MUSIK_SOURCES_NETSTREAM)
	{
		wxString sFilename = GetItemText( nIndex );
		SourcesToFilename( &sFilename, nType );

		if ( wxFileExists( sFilename ) )
			wxRemoveFile( sFilename );
	}
	
	//--- remove item from list ---//
	g_SourcesList.RemoveAt( nIndex );
	Update();

	//--- reselect item ---//
	wxListCtrlSelNone( this );

	int nNextSel = nIndex;
	if ( nNextSel != 0 )
		nNextSel -= 1;

	SetItemState( nNextSel, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
	UpdateSel( nNextSel );

	m_Deleting = false;
}

void CSourcesListBox::RenameSel()
{
	int nIndex = -1;
	nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
	if ( nIndex > -1 )
		EditLabel( nIndex );
}

int CSourcesListBox::GetType(long index) const 
{
	if( index < 0)
		return MUSIK_SOURCES_NONE;
	wxString sType = g_SourcesList.Item( index ).Left( 3 );
	
	if ( sType == wxT( "[l]" ) )
		return MUSIK_SOURCES_LIBRARY;
	else if ( sType == wxT( "[s]" ) )
		return MUSIK_SOURCES_PLAYLIST_STANDARD;
	else if ( sType == wxT( "[d]" ) )
		return MUSIK_SOURCES_PLAYLIST_DYNAMIC;
	else if ( sType == wxT( "[c]" ) )
		return MUSIK_SOURCES_CDROM_DRIVE;
	else if ( sType == wxT( "[n]" ) )
		return MUSIK_SOURCES_NOW_PLAYING;
	else if ( sType == wxT( "[u]" ) )
		return MUSIK_SOURCES_NETSTREAM;
	
	return MUSIK_SOURCES_NONE;
}

int CSourcesListBox::GetItemImage( long index )
{
	wxListItem item;
	item.SetId( index );
	item.SetMask( wxLIST_MASK_IMAGE );
	GetItem( item );
	return item.GetImage();
}

void CSourcesListBox::RescaleColumns()
{
	int nWidth, nHeight;
	GetClientSize( &nWidth, &nHeight );
	SetColumnWidth( 0, nWidth );		
}

void CSourcesListBox::RescanPlaylistDir()
{
	wxArrayString playlists;
	GetPlaylistDir( playlists );

	if ( playlists.GetCount() > 0 )
		AddMissing( playlists );

	else
		wxMessageBox( _( "No missing playlists were found in the scan." ), MUSIK_VERSION, wxICON_INFORMATION );
}

void CSourcesListBox::ShowIcons()
{
	if ( g_Prefs.nShowSourcesIcons == 0 )
		SetImageList( g_NullImageList, wxIMAGE_LIST_SMALL );
	else
		SetImageList( g_SourcesImages, wxIMAGE_LIST_SMALL );
}


void CSourcesListBox::ToggleIcons()
{
	if ( g_Prefs.nShowSourcesIcons == 0 )
		g_Prefs.nShowSourcesIcons = 1;
	else
		g_Prefs.nShowSourcesIcons = 0;

	ShowIcons();

	//--- check menu items accordingly ---//
	view_menu->Check( MUSIK_MENU_SOURCES_STATE, ( bool )g_Prefs.nShowSources );
	sources_context_menu->Check( MUSIK_SOURCE_CONTEXT_SHOW_ICONS, ( bool )g_Prefs.nShowSourcesIcons );
}

void CSourcesListBox::NewPlaylist( wxString sName, wxString sVal, int nType )
{
	//--- check validity ---//
	wxString sCheck = sName;
	sCheck.Replace( wxT( " " ), wxT( "" ) );
	if ( sCheck == wxT( "" ) )
	{
		wxMessageBox( _( "Invalid playlist name." ), MUSIK_VERSION, wxOK | wxICON_INFORMATION );
		return;
	}

	//--- backup old playlist ---//
	if ( GetSelType() == MUSIK_SOURCES_LIBRARY )
		g_LibPlaylist = g_Playlist;

	switch ( nType )
	{
	case MUSIK_SOURCES_PLAYLIST_STANDARD:
    //--- create standard playlist ---//
		CreateStdPlaylist( sName, sVal );
		break;
	//--- create dynamic playlist
	case MUSIK_SOURCES_PLAYLIST_DYNAMIC:
		CreateDynPlaylist( sName );
		break;
	case MUSIK_SOURCES_NETSTREAM:
		CreateNetStream( sName );
		break;
	default:
		wxASSERT(false);
	}
}	

bool CSourcesListBox::PlaylistExists( wxString sName, int type )
{
	SourcesToFilename( &sName, type );
	return wxFileExists( sName );
}

void CSourcesListBox::RewriteStdPlaylist()
{
	//--- read what we're gonna write ---//
	wxString sFilename = GetItemText( m_CurSel );	
	wxString sItemText = sFilename;
	
	//--- remove the old file ---//
	SourcesToFilename( &sFilename );
	wxRemoveFile( sFilename );
	
	//--- write the new file ---//
	wxString sFiles = g_PlaylistBox->PlaylistCtrl().GetAllFiles();
	PlaylistToFile( sItemText, &sFiles, MUSIK_SOURCES_PLAYLIST_STANDARD );
}

bool CSourcesListBox::CreateStdPlaylist( wxString sName, wxString sSongs )
{
	//-------------------------------------------------------------//
	//--- see if the playlist exists, if it does check to see	---//
	//--- if its not visible. if its not then ask if it should	---//
	//--- be. will also prompt to append if already exists		---//
	//-------------------------------------------------------------//
	if ( PlaylistExists( sName, MUSIK_SOURCES_PLAYLIST_STANDARD ) )
	{
		//-----------------------------------------------------//
		//--- search the list to see if the item is visible ---//
		//-----------------------------------------------------//
		if ( FindInSources( sName, MUSIK_SOURCES_PLAYLIST_STANDARD ) == -1 )
		{
			if ( wxMessageBox( _( "Standard playlist \"" ) + sName + _( "\" already exists, but does not appear to be visible in the panel.\n\nWould you like to show it?" ), MUSIK_VERSION, wxYES_NO | wxICON_QUESTION ) == wxYES )
			{		
				g_SourcesList.Add( wxT( "[s] " ) + sName );
				Update();
			}
		}

		//-----------------------------------------------------//
		//--- if we don't have songs to add, don't bother	---//
		//--- with asking user to append the playlist. this	---//
		//--- will happen when the context menu is used	to	---//
		//--- create the list.								---//
		//-----------------------------------------------------//
		if ( sSongs != wxT( "" ) )
		{
			if ( wxMessageBox( _( "A playlist with this name already exists, would you like to append the existing one?" ), MUSIK_VERSION, wxYES_NO | wxICON_QUESTION ) == wxYES )
				AppendStdPlaylist( sName, sSongs );
		}

		return true;
	}
	
	//-------------------------------------------------------------//
	//--- otherwise the playlist does not already exist, so	we	---//
	//--- need to add it to the sources list and update the		---//
	//--- control.												---//
	//-------------------------------------------------------------//
	if ( PlaylistToFile( sName, &sSongs, MUSIK_SOURCES_PLAYLIST_STANDARD ) )
	{    
		g_SourcesList.Add( wxT( "[s] " ) + sName );
		Update();
	}
	
	return true;
}

bool CSourcesListBox::PlaylistToFile( wxString sName, wxString* sSongs, int nType, bool bDelOld )
{
	SourcesToFilename( &sName, nType );
	if ( bDelOld )
	{
		if ( wxFileExists( sName ) )
			wxRemoveFile( sName );
	}

	wxTextFile Out;	
	Out.Create( sName );
	Out.Open();

	if ( !Out.IsOpened() )
		return false;

	Out.AddLine( *sSongs );
	Out.Write( Out.GuessType() );
	Out.Close();
    
	return true;
}

bool CSourcesListBox::CreateDynPlaylist( wxString sName )
{
	//-----------------------------------------------------//
	//--- see if the playlist with this name already	---//
	//--- exists on the user's hard drive. otherwise	---//
	//--- create it.									---//
	//-----------------------------------------------------//
	if ( PlaylistExists( sName, MUSIK_SOURCES_PLAYLIST_DYNAMIC ) )
	{
		//-----------------------------------------------------//
		//--- search the list to see if the item is visible ---//
		//-----------------------------------------------------//
		int nItemPos = FindInSources( sName, MUSIK_SOURCES_PLAYLIST_DYNAMIC );
		if ( nItemPos == -1 )
		{
			if ( wxMessageBox( _( "Dynamic playlist \"" ) + sName + _( "\" already exists, but does not appear to be visible in the panel.\n\nWould you like to show it?" ), MUSIK_VERSION, wxYES_NO | wxICON_QUESTION ) == wxYES )
			{		
				nItemPos = g_SourcesList.GetCount();
				g_SourcesList.Add( wxT( "[d] " ) + sName );
				Update();
			}
		}

		//-----------------------------------------------------//
		//--- ask the user if he wants to edit the current	---//
		//--- query since it already exists					---//
		//-----------------------------------------------------//
		int nAnswer = wxMessageBox( _( "A dynamic playlist with this name already exists, would you like to edit the existing query?" ), MUSIK_VERSION, wxYES_NO | wxICON_QUESTION );
		if ( nAnswer == wxYES )
			UpdateDynPlaylist( nItemPos );

		return false;
	}

	//-----------------------------------------------------//
	//--- ask user for a query. if the user doesn't		---//
	//--- give a blank one back, save this to file, and	---//
	//--- return.										---//
	//-----------------------------------------------------//
	else
	{
		wxString sQuery = PromptDynamicPlaylist( wxT( "" ) );

		if ( sQuery != wxT( "" ) )
		{
			PlaylistToFile( sName, &sQuery, MUSIK_SOURCES_PLAYLIST_DYNAMIC );
			g_SourcesList.Add( wxT( "[d] " ) + sName );
			Update();
			
			return true;
		}
		else
			return false;
	}

	return false;
}

void CSourcesListBox::UpdateDynPlaylist( int nIndex )
{
	wxString sName	= GetPlaylistName( nIndex );
	wxString sQuery = LoadDynPlaylist( sName );

	sQuery = PromptDynamicPlaylist( sQuery );

	if ( sQuery != wxT( "" ) )
	{
		g_SourcesList.Item( nIndex ) = wxT( "[d] " ) + sName;
		PlaylistToFile( sName, &sQuery, MUSIK_SOURCES_PLAYLIST_DYNAMIC );

		g_Library.QuerySongs( sQuery, g_Playlist );
		g_PlaylistBox->Update();
	}
}

wxString CSourcesListBox::PromptDynamicPlaylist( wxString sQuery )
{
	wxTextEntryDialog dlg( this, _( "Examples:\ntitle like '%funky%'    (all titles containing funky)\nbitrate < 128, vbr = 0    (all low quality, non-VBR)\ntimesplayed > 10 order by artist    (your popular tracks)" ), MUSIK_VERSION, sQuery );

	if ( dlg.ShowModal() == wxID_OK )
		return dlg.GetValue();	

	return wxT( "" );
}

bool CSourcesListBox::CreateNetStream( wxString sName)
{
	//-------------------------------------------------------------//
	//--- see if the net stream exists, if it does check to see	---//
	//--- if its not visible. if its not then ask if it should	---//
	//--- be. will also prompt to append if already exists		---//
	//-------------------------------------------------------------//
	if ( PlaylistExists( sName, MUSIK_SOURCES_NETSTREAM ) )
	{
		//-----------------------------------------------------//
		//--- search the list to see if the item is visible ---//
		//-----------------------------------------------------//
		if ( FindInSources( sName, MUSIK_SOURCES_NETSTREAM ) == -1 )
		{
			if ( wxMessageBox( _( "Net Stream \"" ) + sName + _( "\" already exists, but does not appear to be visible in the panel.\n\nWould you like to show it?" ), MUSIK_VERSION, wxYES_NO | wxICON_QUESTION ) == wxYES )
			{		
				g_SourcesList.Add( wxT( "[u] " ) + sName );
				Update();
			}
		}
		return true;
	}
	
	//---------------------------------------------------------------//
	//--- otherwise the net stream does not already exist, so we  ---//
	//--- need to add it to the sources list and update the		  ---//
	//--- control.												  ---//
	//---------------------------------------------------------------//
	wxString sAddress = PromptNetStreamAddress(wxT(""));
	if ((sAddress.IsEmpty() == false) && PlaylistToFile( sName, &sAddress, MUSIK_SOURCES_NETSTREAM ) )
	{    
		g_SourcesList.Add( wxT( "[u] " ) + sName );
		Update();
		return true;
	}
	return false;
	
}
wxString CSourcesListBox::PromptNetStreamAddress( wxString sAddress )
{
	wxTextEntryDialog dlg( this, _( "Enter net stream address(URL):" ), MUSIK_VERSION, sAddress );

	if ( dlg.ShowModal() == wxID_OK )
		return dlg.GetValue();	

	return wxT( "" );
}
wxString CSourcesListBox::GetPlaylistName( int nIndex )
{
	wxString sRet = g_SourcesList.Item( nIndex );
	sRet = sRet.Right( sRet.Length() - 4 );
	return sRet;
}

void CSourcesListBox::AppendStdPlaylist( wxString sName, wxString sSongs )
{
	wxString origname( sName );

	SourcesToFilename( &sName );
	wxTextFile In( sName );

	In.Open();
	if ( !In.IsOpened() )
			return;

	sSongs = wxT( "\n" ) + sSongs;
	In.AddLine( sSongs );
	In.Write();
	In.Close();
}

void CSourcesListBox::LoadStdPlaylist(wxString sName, wxArrayString & aReturn )
{
	aReturn.Clear();

	SourcesToFilename( &sName );

	if ( !wxFileExists( sName ) )
	{
		return ;
	}
	wxTextFile In( sName );
	In.Open();
	if ( !In.IsOpened() )
	{
		return;
	}
	
	for ( size_t i = 0; i < In.GetLineCount(); i++ )
	{
		wxString sCheck = In.GetLine( i );
		sCheck.Replace( wxT( " " ), wxT( "" ), TRUE );
		if ( sCheck != wxT( "" ) )
			aReturn.Add( In.GetLine( i ) );
	}
	In.Close();

	return;
}

wxString CSourcesListBox::LoadDynPlaylist( wxString sName )
{
	wxString sReturn;

	SourcesToFilename( &sName, MUSIK_SOURCES_PLAYLIST_DYNAMIC );

	if ( !wxFileExists( sName ) )
		return wxT( "" );

	wxTextFile In( sName );
	In.Open();
	if ( !In.IsOpened() )
		return wxT( "" );

	sReturn = In.GetLine( 0 );

	return sReturn;
}
void CSourcesListBox::LoadNetStream(wxString sName, CMusikSong & song )
{
	wxString sFilename = sName;
	SourcesToFilename( &sFilename ,MUSIK_SOURCES_NETSTREAM);

	if ( !wxFileExists( sFilename ) )
	{
		return ;
	}
	wxTextFile In( sFilename );
	In.Open();
	if ( !In.IsOpened() )
	{
		return;
	}
	if(In.GetLineCount() >= 1)
	{
		song.Title = sName;
		song.Artist = _("Net Stream");
		song.Filename = In.GetLine(0);
		song.Format = MUSIK_FORMAT_NETSTREAM;

	}
	In.Close();

	return;}
void CSourcesListBox::AddMissing( const wxArrayString & playlists )
{
	wxString sExt, sName, sAdd;
	int nType;

	for ( size_t i = 0; i < playlists.GetCount(); i++ )
	{
		sName = playlists.Item( i );

		sExt = sName.Right( 3 );
		sExt.MakeLower();

		sName = sName.Left( sName.Length() - 4 );
		sName.Replace( wxT( "_" ), wxT( " " ), true );

		if ( sExt == wxT( "mpl" ) )
		{
			sAdd = wxT( "[s] " ) + sName;
			nType = MUSIK_SOURCES_PLAYLIST_STANDARD;
		}
		else if ( sExt == wxT( "mpd" ) )
		{
			sAdd = wxT( "[d] " ) + sName;
			nType = MUSIK_SOURCES_PLAYLIST_DYNAMIC;
		}
		else
			return;

		if ( FindInSources( sName, nType ) == -1 )
			g_SourcesList.Add( sAdd );
	}

	Update();
}

int CSourcesListBox::FindInSources( wxString sName, int nType )
{
	wxString sFind;
	switch( nType )
	{
	case MUSIK_SOURCES_LIBRARY:
		sFind = wxT( "[l] " );
		break;
	case MUSIK_SOURCES_PLAYLIST_STANDARD:
		sFind = wxT( "[s] " );
		break;
	case MUSIK_SOURCES_PLAYLIST_DYNAMIC:
		sFind = wxT( "[d] " );
		break;
	case MUSIK_SOURCES_CDROM_DRIVE:
		sFind = wxT( "[c] " );
		break;
	case MUSIK_SOURCES_NOW_PLAYING:
		sFind = wxT( "[n] " );
		break;
	case MUSIK_SOURCES_NETSTREAM:
		sFind = wxT( "[u] " );
		break;
	default:
		return -1;
	}
	sFind += sName;
	sFind.MakeLower();

	for ( size_t i = 0; i < g_SourcesList.GetCount(); i++ )
	{
		if ( sFind == g_SourcesList.Item( i ).Lower() )
			return i;
	}

	return -1;
}

void CSourcesListBox::SourcesToFilename( wxString* sSources, int nType )
{
	wxString sName = *sSources;

	sName.Replace( wxT( " " ), wxT( "_" ), TRUE );
	sName = sName.Lower();	

	wxString sExt;
	switch ( nType )
	{
	case MUSIK_SOURCES_PLAYLIST_STANDARD:
		sExt = wxT( ".mpl" );
		break;
	case MUSIK_SOURCES_PLAYLIST_DYNAMIC:
		sExt = wxT( ".mpd" );
		break;
	case MUSIK_SOURCES_NETSTREAM:
		sExt = wxT( ".mpu" );
		break;
	default:
		wxASSERT(false);
	}	
	*sSources = MUSIK_PLAYLIST_DIR + sName + sExt;
}

void CSourcesListBox::ShowIconsChecked( bool bCheck )
{
	sources_context_menu->Check( MUSIK_SOURCE_CONTEXT_SHOW_ICONS, bCheck );
}

BEGIN_EVENT_TABLE(CSourcesBox, wxSashLayoutWindow)
	EVT_SASH_DRAGGED			( MUSIK_SOURCES,				CSourcesBox::OnSashDragged			)	
END_EVENT_TABLE()

CSourcesBox::CSourcesBox( wxWindow *parent )
	: wxSashLayoutWindow( parent, MUSIK_SOURCES, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxNO_BORDER | wxCLIP_CHILDREN |wxSW_3D )
{
	//--- CSourcesListBox ---//
	pListBox	= new CSourcesListBox( this );

/*	
	//--- top sizer ---//
	pSizer = new wxBoxSizer( wxVERTICAL );
	pSizer->Add( pListBox, 0, wxEXPAND | wxTOP, 0 );
	SetSizerAndFit( pSizer );
*/
	Layout();
}

void CSourcesBox::OnSashDragged	(wxSashEvent & ev)
{
	g_Prefs.nSourceBoxWidth = ev.GetDragRect().width;
	SetDefaultSize(wxSize(g_Prefs.nSourceBoxWidth, 1000));
	ev.Skip();
}

CSourcesBox::~CSourcesBox()
{
	
}
