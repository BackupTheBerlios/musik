/*
 *  SourcesBox.cpp
 *
 *  The "Sources Box" control
 *	This is the control visible on the left side of the main frame.
 *	It's main function is playlist management.
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
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
	long n;
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

wxDragResult SourcesDropTarget::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
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
BEGIN_EVENT_TABLE(CSourcesListBox, wxListCtrl)
	EVT_CONTEXT_MENU			(												CSourcesListBox::ShowMenu				)
	EVT_MENU					(MUSIK_SOURCE_CONTEXT_CREATE_CURRENT_PLAYLIST,	CSourcesListBox::CreateCurPlaylist		)	// Sources Context -> Create -> Create from Current
	EVT_MENU					(MUSIK_SOURCE_CONTEXT_STANDARD_PLAYLIST,		CSourcesListBox::StandardPlaylist		)	// Sources Context -> Create -> Standard Playlist
	EVT_MENU					(MUSIK_SOURCE_CONTEXT_DYNAMIC_PLAYLIST,			CSourcesListBox::DynamicPlaylist		)	// Sources Context -> Create -> Dynamic Playlist
	EVT_MENU					(MUSIK_SOURCE_CONTEXT_EDIT_QUERY,				CSourcesListBox::EditQuery				)	// Sources Context -> Edit Query
	EVT_MENU					(MUSIK_SOURCE_CONTEXT_DELETE,					CSourcesListBox::Delete					)	// Sources Context -> Delete
	EVT_MENU					(MUSIK_SOURCE_CONTEXT_RENAME,					CSourcesListBox::Rename					)	// Sources Context -> Rename
	EVT_MENU					(MUSIK_SOURCE_CONTEXT_SHOW_ICONS,				CSourcesListBox::ToggleIconsEvt			)	// Sources Context -> Show Icons
	EVT_LIST_BEGIN_DRAG			(MUSIK_SOURCES,									CSourcesListBox::BeginDrag				)	// user drags files from sources
	EVT_LIST_ITEM_SELECTED		(MUSIK_SOURCES,									CSourcesListBox::UpdateSel				)	// sources list sel
	EVT_LIST_BEGIN_LABEL_EDIT	(MUSIK_SOURCES,									CSourcesListBox::BeginEditLabel			)   // user edits a playlist filename
	EVT_LIST_END_LABEL_EDIT		(MUSIK_SOURCES,									CSourcesListBox::EndEditLabel			)   // user edits a playlist filename
	EVT_LIST_KEY_DOWN			(MUSIK_SOURCES,									CSourcesListBox::TranslateKeys			)	// user presses a key in the sources list
	EVT_LIST_ITEM_ACTIVATED		(MUSIK_SOURCES,									CSourcesListBox::UpdateSel				)	// user wants to redisplay playlist
END_EVENT_TABLE()

CSourcesListBox::CSourcesListBox( wxPanel* parent, wxWindowID id )
	: wxListCtrl( parent, id, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxLC_NO_HEADER | wxLC_ALIGN_LEFT | wxLC_REPORT | wxLC_VIRTUAL | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL | wxNO_BORDER & ~wxHSCROLL )
{
	//--- initialize variables ---//
	m_CurSel = 0;
	m_DragIndex	= -1;

	//--- setup context menus ---//
	sources_context_menu_new = new wxMenu;
	sources_context_menu_new->Append( MUSIK_SOURCE_CONTEXT_CREATE_CURRENT_PLAYLIST, _( "Playlist from Current" ) );
	sources_context_menu_new->AppendSeparator();
	sources_context_menu_new->Append( MUSIK_SOURCE_CONTEXT_STANDARD_PLAYLIST, _( "Standard Playlist" ) );
	sources_context_menu_new->Append( MUSIK_SOURCE_CONTEXT_DYNAMIC_PLAYLIST, _( "Dynamic Playlist" ) );

	sources_context_menu = new wxMenu;
	sources_context_menu->Append( MUSIK_SOURCE_CONTEXT_CREATE, _( "&Create New" ), sources_context_menu_new );
	sources_context_menu->Append( MUSIK_SOURCE_CONTEXT_EDIT_QUERY, _( "&Edit Query" ) );
	sources_context_menu->Append( MUSIK_SOURCE_CONTEXT_RENAME, _( "&Rename" ) );
	sources_context_menu->Append( MUSIK_SOURCE_CONTEXT_DELETE, _( "&Delete" ) );
	sources_context_menu->AppendSeparator();
	sources_context_menu->Append( MUSIK_SOURCE_CONTEXT_SHOW_ICONS, _( "&Show Icons" ), wxT( "" ), wxITEM_CHECK );
	sources_context_menu->Check( MUSIK_SOURCE_CONTEXT_SHOW_ICONS, ( bool )g_Prefs.nShowSourcesIcons );

	InsertColumn( 0, _( "Sources" ), wxLIST_FORMAT_CENTER );
	SetDropTarget( new SourcesDropTarget( this ) );
	SetItemState( 0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );

	ShowIcons();

	Load();
	Update();
}

CSourcesListBox::~CSourcesListBox()
{
	Save();
	delete sources_context_menu;
}

void CSourcesListBox::ShowMenu( wxCommandEvent &WXUNUSED(event) )
{
	int nSel = GetSelectedItemCount();

	wxPoint pos = ScreenToClient( wxGetMousePosition() );
	if ( nSel == 0 )
	{
		sources_context_menu->Enable( MUSIK_SOURCE_CONTEXT_RENAME, false );
		sources_context_menu->Enable( MUSIK_SOURCE_CONTEXT_DELETE, false );
		sources_context_menu->Enable( MUSIK_SOURCE_CONTEXT_EDIT_QUERY, false );
	}
	else
	{
		sources_context_menu->Enable( MUSIK_SOURCE_CONTEXT_RENAME, true );

		if ( GetType( m_CurSel ) == MUSIK_SOURCES_LIBRARY )
			sources_context_menu->Enable( MUSIK_SOURCE_CONTEXT_DELETE, false );
		else
			sources_context_menu->Enable( MUSIK_SOURCE_CONTEXT_DELETE, true );

		if ( GetType( m_CurSel ) == MUSIK_SOURCES_PLAYLIST_DYNAMIC )
			sources_context_menu->Enable( MUSIK_SOURCE_CONTEXT_EDIT_QUERY, true );
		else
			sources_context_menu->Enable( MUSIK_SOURCE_CONTEXT_EDIT_QUERY, false );

	}
	PopupMenu( sources_context_menu, pos );
}

void CSourcesListBox::CreateCurPlaylist( wxCommandEvent& WXUNUSED(event) )
{
	wxTextEntryDialog dlg( this, _( "Enter name for new playlist:" ), MUSIK_VERSION, wxT( "" ) );
	if ( dlg.ShowModal() == wxID_OK )
	{
		wxString sName = dlg.GetValue();
		NewPlaylist( sName, g_PlaylistCtrl->GetAllFiles(), MUSIK_SOURCES_PLAYLIST_STANDARD );
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
void CSourcesListBox::EditQuery( wxCommandEvent& WXUNUSED(event) )
{
	UpdateDynPlaylist( wxT( "" ), -1 );
}

void CSourcesListBox::ToggleIconsEvt( wxCommandEvent& WXUNUSED(event) )
{
	ToggleIcons();
}

void CSourcesListBox::BeginDrag( wxListEvent &event )
{
	if ( m_CurSel != -1 )
	{
		g_DragInProg = true;

		//-------------------------//
		//--- get selected item ---//
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

void CSourcesListBox::UpdateSel( wxListEvent& pEvent )
{
	if ( !g_DragInProg && !g_FirstRun )
	{
		int nLastSel = m_CurSel;
		m_CurSel = pEvent.GetIndex();

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
				g_Playlist = g_Library.GetAllSongs();

			g_LibPlaylist.Clear();
			g_PlaylistCtrl->Update();
			g_MusikFrame->ShowActivityArea( true );
		}

		else 
		{
			//--- backup playlist so we can revert later ---//
			g_LibPlaylist = g_Playlist;

			//--- standard playlist selected ---//
			if ( m_CurSel != 0 && m_CurSel != -1 && nSelType == MUSIK_SOURCES_PLAYLIST_STANDARD )
			{
				wxArrayString aFilelist = LoadStdPlaylist( GetItemText( m_CurSel ) );
				g_Playlist = g_Library.GetStdPlaylistSongs( &aFilelist );
			}

			//--- dynamic playlist selected ---//
			else if ( m_CurSel != 0 && m_CurSel != -1 && nSelType == MUSIK_SOURCES_PLAYLIST_DYNAMIC )
			{
				wxString sQuery = LoadDynPlaylist( GetItemText( m_CurSel ) );
				g_Playlist = g_Library.QuerySongs( sQuery );
			}

			//--- update ui with new list ---//
			g_MusikFrame->ShowActivityArea( false );
			g_PlaylistCtrl->Update();
		}

		g_PlaylistChanged = true; 
	}
}

void CSourcesListBox::BeginEditLabel( wxListEvent& pEvent )
{
	m_SourcesEditStr	= pEvent.GetText();
	m_SourcesEditIndex	= pEvent.GetIndex();
}

void CSourcesListBox::EndEditLabel( wxListEvent& pEvent )
{

	//--- Musik Library entry edited ---//
	if ( m_SourcesEditIndex == 0 )
	{
        wxString sCheck = pEvent.GetText();
		sCheck.Replace( wxT( " " ), wxT( "" ), TRUE );
		if ( sCheck != wxT( "" ) )
			g_SourcesList.Item( m_SourcesEditIndex ) = wxT( "[l] " ) + pEvent.GetText();
	}

	//--- standard or dyanmic playlist renamed ---//
	int nType = GetType( m_SourcesEditIndex );
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
		if ( nType == MUSIK_SOURCES_PLAYLIST_STANDARD )
			sType = wxT( "[s] " );
		else if ( nType == MUSIK_SOURCES_PLAYLIST_DYNAMIC )
			sType = wxT( "[d] " );

		//--- rename in musiksources.dat ---//
		g_SourcesList.Item( m_SourcesEditIndex ) = sType + pEvent.GetText();
	}
}

void CSourcesListBox::TranslateKeys( wxListEvent& pEvent )
{
	if ( GetSelectedItemCount() == 1 )
	{
		if ( pEvent.GetKeyCode() == WXK_DELETE || pEvent.GetKeyCode() == WXK_BACK )
			DelSel();
		else if ( pEvent.GetKeyCode() == WXK_F2 )
			RenameSel();
	}
}

void CSourcesListBox::Create()
{
	wxTextFile Out( MUSIK_SOURCES_FILENAME );

	Out.Create();
	if ( Out.Open() )
	{
		Out.AddLine( wxT( "[l] Musik Library" ) );
		Out.Write();
		Out.Close();
	}
}

void CSourcesListBox::Load()
{
	if ( !wxFileExists( MUSIK_SOURCES_FILENAME ) )
		Create();

	g_SourcesList = FileToStringArray( MUSIK_SOURCES_FILENAME );
}

void CSourcesListBox::Save()
{
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
	wxString type = g_SourcesList.Item( item ).Left( 3 );

	type.MakeLower();

	if ( type == wxT( "[l]" ) )
		return MUSIK_SOURCES_LIBRARY;
	else if ( type == wxT( "[s]" ) )
		return MUSIK_SOURCES_PLAYLIST_STANDARD;
	else if ( type == wxT( "[d]" ) )
		return MUSIK_SOURCES_PLAYLIST_DYNAMIC;
	else if ( type == wxT( "[a]" ) )
		return MUSIK_SOURCES_ALARM;
	else if ( type == wxT( "[c]" ) )
		return MUSIK_SOURCES_CDROM_DRIVE;
    
	return MUSIK_SOURCES_NONE;	
}

void CSourcesListBox::Update()
{
	//----------------------------------------------------------------------------------//
	//---        note that the sources box control is now virtual, so we don't		 ---//
	//---    add items directly to it.  Instead, we have the object pItems a		 ---//
	//---  *wxArrayString, that the virtual listctrl then references via callbacks   ---//
	//----------------------------------------------------------------------------------//

	m_Light		= wxListItemAttr( *wxBLACK, wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ), wxNullFont	);
	m_LightBold	= wxListItemAttr( *wxBLACK, wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ), g_fntBold		);
	m_Dark		= wxListItemAttr( *wxBLACK, StringToColour( g_Prefs.sSourcesStripeColour ), wxNullFont );
	m_DarkBold	= wxListItemAttr( *wxBLACK, StringToColour( g_Prefs.sSourcesStripeColour ), g_fntBold );

	Freeze();
	SetItemCount( g_SourcesList.GetCount() );
	Thaw();
	Refresh( FALSE );
	RescaleColumns();
}


void CSourcesListBox::DelSel()
{
	//--- where are we selected? ---//
	int nIndex = - 1;
	nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );

	int nType = GetType( nIndex );

	//--- no item selected? bail ---//
	if ( nIndex == - 1 || nType == MUSIK_SOURCES_LIBRARY )
		return;

	//--- if we have a standard or dynamic playlist, delete the file ---//
	if ( nType == MUSIK_SOURCES_PLAYLIST_STANDARD || nType == MUSIK_SOURCES_PLAYLIST_DYNAMIC )
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
	SetItemState( nIndex - 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
}

void CSourcesListBox::RenameSel()
{
	int nIndex = -1;
	nIndex = GetNextItem( nIndex, wxLIST_NEXT_ALL , wxLIST_STATE_SELECTED );
	if ( nIndex > -1 )
		EditLabel( nIndex );
}

int CSourcesListBox::GetType(long index)
{
	wxListItem item;
	item.SetId( index );
	item.SetMask( wxLIST_MASK_IMAGE );
	GetItem( item );
	return item.GetImage();
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
	wxArrayString playlists = GetPlaylistDir();

	if ( playlists.GetCount() > 0 )
	{
		for ( size_t i = 0; i < playlists.GetCount(); i++ )
		{
			//--- check, then add it if needed ---//
			if ( !PlaylistVisible( playlists.Item( i ) ) )
				AddMissing( playlists.Item( i ) );			
		}
	}
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

//--- playlist routines ---//
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

    //--- create standard playlist ---//
	if ( nType == MUSIK_SOURCES_PLAYLIST_STANDARD )
		CreateStdPlaylist( sName, sVal );

	//--- create dynamic playlist
	else if ( nType == MUSIK_SOURCES_PLAYLIST_DYNAMIC )
		CreateDynPlaylist( sName );
}

bool CSourcesListBox::PlaylistExists( wxString sName )
{
	SourcesToFilename( &sName );
	return wxFileExists( sName );
}

bool CSourcesListBox::PlaylistVisible( wxString sFilename )
{
	sFilename = sFilename.Left( sFilename.Length() - 4 );	//--- remove the .mpl ---//
	sFilename.Replace( wxT( "_" ), wxT( " " ), TRUE );
	for ( int i = 0; i < GetItemCount(); i++ )
	{
		if ( GetItemText( i ) == sFilename )
			return true;
	}
	return false;
}

void CSourcesListBox::AddMissing( wxString sName )
{
	sName = sName.Left( sName.Length() - 4 );	//--- remove extension ---//
	sName.Replace( wxT( "_" ), wxT( " " ), true );

	g_SourcesList.Add( wxT( "[s] " ) + sName );
	Update();
}

//--- standard playlist ---//
void CSourcesListBox::RewriteStdPlaylist()
{
	//--- read what we're gonna write ---//
	wxString sFilename = GetItemText( m_CurSel );	
	wxString sItemText = sFilename;
	
	//--- remove the old file ---//
	SourcesToFilename( &sFilename );
	wxRemoveFile( sFilename );
	
	//--- write the new file ---//
	wxString sFiles = g_PlaylistCtrl->GetAllFiles();
	PlaylistToFile( sItemText, &sFiles, MUSIK_SOURCES_PLAYLIST_STANDARD );
}

bool CSourcesListBox::CreateStdPlaylist( wxString sName, wxString sSongs )
{
	//-------------------------------------------------------------//
	//--- see if the playlist exists, if it does check to see	---//
	//--- if its not visible. if its not then ask if it should	---//
	//--- be. will also prompt to append if already exists		---//
	//-------------------------------------------------------------//
	if ( PlaylistExists( sName ) )
	{
		wxMessageDialog MsgAppend( this, _( "This playlist already exists. Append existing?" ), MUSIK_VERSION, wxYES_NO | wxICON_QUESTION );
		if ( MsgAppend.ShowModal() == wxID_YES )
			AppendStdPlaylist( sName, sSongs );

		//-----------------------------------------------------//
		//--- search the list to see if the item is visible ---//
		//-----------------------------------------------------//
		wxString sMatch = wxT( "[s] " ) + sName;
		sMatch.MakeLower();
		bool bFound = false;
		for ( size_t i = 0; i < g_SourcesList.GetCount(); i++ )
		{
			if ( sMatch == g_SourcesList.Item( i ).Lower() )
			{
				bFound = true;
				break;
			}
		}
		if ( !bFound )
		{
			wxMessageDialog MsgAddExisting( this, _( "Playlist \"" ) + sName + _( "\" does not appear to be visible in the panel. Show?" ), MUSIK_VERSION, wxYES_NO | wxICON_QUESTION ); 
			if ( MsgAddExisting.ShowModal() == wxID_YES )
				AddMissing( sName );
		}

		return true;
	}
	
	//--- save to file ---//
	if ( PlaylistToFile( sName, &sSongs, MUSIK_SOURCES_PLAYLIST_STANDARD ) )
	{    
		//--- add to sources list ---//
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
	int nIndex = FindItem( 0, sName );
	if ( nIndex == -1 || nIndex == 0 )
	{
		wxTextEntryDialog dlg( this, _( "Examples:\ntitle like '%funky%'    (all titles containing funky)\nbitrate < 128, vbr = 0    (all low quality, non-VBR)\ntimesplayed > 10 order by artist    (your popular tracks)" ), MUSIK_VERSION, wxT( "" ) );
		if ( dlg.ShowModal() == wxID_OK )
		{
			//--- get query ---//
			wxString sQuery = dlg.GetValue();

			//-- show and save item ---//
			PlaylistToFile( sName, &sQuery, MUSIK_SOURCES_PLAYLIST_DYNAMIC );
			g_SourcesList.Add( wxT( "[d] " ) + sName );
			Update();
		}		
	}
	else
	{
		wxMessageDialog MsgPlaylistExists( this, _( "This playlist already exists. Edit current?" ), MUSIK_VERSION, wxYES_NO | wxICON_QUESTION );
		if ( MsgPlaylistExists.ShowModal() == wxID_YES )
			UpdateDynPlaylist( sName, nIndex );
		return false;
	}
	return true;	
}

void CSourcesListBox::UpdateDynPlaylist( wxString sName, int nIndex )
{
	if ( sName == wxT( "" ) )
		sName = GetItemText( m_CurSel );

	if ( nIndex == -1 )
		nIndex = FindItem( 0, sName );

	wxString sOldQuery = g_SourcesList.Item( nIndex ).Right( g_SourcesList.Item( nIndex ).Length() - 3 );
	wxTextEntryDialog dlg( this, _( "Examples:\ntitle like '%funky%'    (all titles containing funky)\nbitrate < 128, vbr = 0    (all low quality, non-VBR)\ntimesplayed > 10 order by artist    (your popular tracks)" ), MUSIK_VERSION, sOldQuery );
	if ( dlg.ShowModal() == wxID_OK )
	{
		//--- get query ---//
		wxString sNewQuery = dlg.GetValue();	

		//--- remove old, add new ---//
		g_SourcesList.Item( nIndex ) = wxT( "[d] " ) + sName;
		PlaylistToFile( sName, &sNewQuery, MUSIK_SOURCES_PLAYLIST_DYNAMIC );

		//--- populate list ---//
		g_Playlist = g_Library.QuerySongs( sNewQuery );
		g_PlaylistCtrl->Update( );
	}
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

wxArrayString CSourcesListBox::LoadStdPlaylist( wxString sName )
{
	wxArrayString aReturn;
	
	SourcesToFilename( &sName );

	if ( !wxFileExists( sName ) )
		return aReturn;

	wxTextFile In( sName );
	In.Open();
	if ( !In.IsOpened() )
	{
		wxArrayString aNULL;
		return aNULL;
	}
	
	for ( size_t i = 0; i < In.GetLineCount(); i++ )
	{
		wxString sCheck = In.GetLine( i );
		sCheck.Replace( wxT( " " ), wxT( "" ), TRUE );
		if ( sCheck != wxT( "" ) )
			aReturn.Add( In.GetLine( i ) );
	}
	In.Close();

	return aReturn;
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

void CSourcesListBox::SourcesToFilename( wxString* sSources, int nType )
{
	wxString sName = *sSources;
	sName.Replace( wxT( " " ), wxT( "_" ), TRUE );
	sName = sName.Lower();	

	wxString sExt;
	if ( nType == MUSIK_SOURCES_PLAYLIST_STANDARD )
		sExt = wxT( ".mpl" );
	else if ( nType == MUSIK_SOURCES_PLAYLIST_DYNAMIC )
		sExt = wxT( ".mpd" );
	
	*sSources = MUSIK_PLAYLIST_DIR + sName + sExt;
}

void CSourcesListBox::ShowIconsChecked( bool bCheck )
{
	sources_context_menu->Check( MUSIK_SOURCE_CONTEXT_SHOW_ICONS, bCheck );
}

//-------------------//
//--- CSourcesBox ---//
//-------------------//
CSourcesBox::CSourcesBox( wxWindow *parent, wxWindowID id )
	: wxPanel( parent, -1, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxSIMPLE_BORDER | wxCLIP_CHILDREN )
{
	//--- CListHeader / CSourcesListBox ---//
	pHeader		= new CListHeader( this, _( "Sources" ) );
	pListBox	= new CSourcesListBox( this, id );
	
	//--- top sizer ---//
	pSizer = new wxBoxSizer( wxVERTICAL );
	pSizer->Add( pHeader, 0, wxEXPAND | wxBOTTOM, 0 );
	pSizer->Add( pListBox, 1, wxEXPAND | wxTOP, 0 );
	SetSizerAndFit( pSizer );

	Layout();
}

CSourcesBox::~CSourcesBox()
{
	delete pHeader;
	delete pListBox;
}
