///////////////////////////////////////////////////

#include "stdafx.h"

#include "../musikCore/include/musikPlayer.h"

#include "MainFrm.h"

#include "musik.h"
#include "musikSourcesCtrl.h"
#include "musikSourcesDropTarget.h"

///////////////////////////////////////////////////

#pragma warning (disable : 4312)	// conversion from int to void* of greater size

///////////////////////////////////////////////////

// CmusikSourcesBar

///////////////////////////////////////////////////

CmusikSourcesBar::CmusikSourcesBar( CFrameWnd* parent, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs, UINT dropid, UINT pldropid_r, UINT sbdropid_r )
	: CmusikDockBar( prefs )
{
	m_wndChild = new CmusikSourcesCtrl( parent, library, player, prefs, dropid, pldropid_r, sbdropid_r );
	m_Parent = parent;
}

///////////////////////////////////////////////////

CmusikSourcesBar::~CmusikSourcesBar()
{
	delete m_wndChild;
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikSourcesBar, baseCmusikSourcesBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY( PTN_SELCHANGE, IDC_SOURCES, OnItemChanged )
	ON_COMMAND(ID_SOURCES_RENAME, OnSourcesRename)
	ON_COMMAND(ID_SOURCES_DELETE, OnSourcesDelete)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CmusikSourcesBar::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( baseCmusikSourcesBar::OnCreate(lpCreateStruct) == -1 )
		return -1;

	// dock bar
    SetBarStyle( GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
	EnableDocking( CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT );

	// child
	if ( !m_wndChild->Create( WS_CHILD | WS_VISIBLE | PTS_NOTIFY, CRect( 0, 0, 0, 0 ), this, IDC_SOURCES ) )
		return -1;

	if ( !m_Font.CreateStockObject( DEFAULT_GUI_FONT ) )
		return -1;

	m_wndChild->SetFont( &m_Font );

	return 0;
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize(nType, cx, cy);

	CRect client_size;
	GetClientRect( &client_size );

	m_wndChild->MoveWindow( 0, 0, client_size.Width(), client_size.Height() );
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnItemChanged( NMHDR* pNotifyStruct, LRESULT* plResult )
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;
  	*plResult = 0;  

	if ( pNMPropTree->pItem )
	{
		// we don't care about root level items,
		// they just act as headers.
		if ( pNMPropTree->pItem->IsRootLevel() || pNMPropTree->pItem->IsActivated() )
			return;

		int playlist_type = pNMPropTree->pItem->GetPlaylistType();

		// library selected
		if ( playlist_type == MUSIK_SOURCES_TYPE_LIBRARY )
		{
			int WM_SOURCESLIBRARY = RegisterWindowMessage( "SOURCESLIBRARY" );
			m_Parent->SendMessage( WM_SOURCESLIBRARY, NULL );

			return;
		}

		// quick search
		else if ( playlist_type == MUSIK_SOURCES_TYPE_QUICKSEARCH )
		{
			GetCtrl()->QuickSearch();
			return;
		}

		// create new std playlist
		else if ( playlist_type == MUSIK_SOURCES_TYPE_NEWSTDPLAYLIST )
		{
			GetCtrl()->CreateNewStdPlaylist();
			return;
		}

		// now playing selected
		else if ( playlist_type == MUSIK_SOURCES_TYPE_NOWPLAYING )
		{
			int WM_SOURCESNOWPLAYING = RegisterWindowMessage( "SOURCESNOWPLAYING" );
			m_Parent->SendMessage( WM_SOURCESNOWPLAYING, NULL );

			return;
		}

		// standard playlist selected
		else if ( playlist_type == MUSIK_PLAYLIST_TYPE_STANDARD )
		{
			int WM_SOURCESSTDPLAYLIST = RegisterWindowMessage( "SOURCESSTDPLAYLIST" );
			m_Parent->SendMessage( WM_SOURCESSTDPLAYLIST, NULL );

			return;
		}

		// dynamic playlist selected
		else if ( playlist_type == MUSIK_PLAYLIST_TYPE_DYNAMIC )
		{
			int WM_SOURCESDYNPLAYLIST = RegisterWindowMessage( "SOURCESDYNDPLAYLIST" );		
			m_Parent->SendMessage( WM_SOURCESDYNPLAYLIST, NULL );

			return;
		}
	}
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnOptions()
{
	ShowMenu( true );
}

///////////////////////////////////////////////////

void CmusikSourcesBar::ShowMenu( bool force_show )
{
	if ( GetCtrl()->m_EditInPlace.IsVisible() )
		GetCtrl()->m_EditInPlace.Cancel();
	CmusikPropTreeItem* pItem = GetCtrl()->GetFocusedItem();

	if ( pItem || ( !pItem && force_show ) )
	{
		CPoint pos;
		::GetCursorPos( &pos );

		CMenu main_menu;
		CMenu* popup_menu;

		main_menu.LoadMenu( IDR_SOURCES_MENU );
		popup_menu = main_menu.GetSubMenu( 0 );

		// update ui doesn't work...
		int type = pItem->GetPlaylistType();

		if ( type == MUSIK_PLAYLIST_TYPE_STANDARD || type == MUSIK_PLAYLIST_TYPE_DYNAMIC )
		{
			popup_menu->EnableMenuItem( ID_SOURCES_RENAME, MF_ENABLED );
			popup_menu->EnableMenuItem( ID_SOURCES_DELETE, MF_ENABLED );

			if ( type == MUSIK_PLAYLIST_TYPE_DYNAMIC )
				popup_menu->EnableMenuItem( ID_SOURCES_EDITQUERY, MF_ENABLED );
			else
				popup_menu->EnableMenuItem( ID_SOURCES_EDITQUERY, MF_GRAYED | MF_DISABLED );
		}
		else
		{
			popup_menu->EnableMenuItem( ID_SOURCES_RENAME, MF_GRAYED | MF_DISABLED );
			popup_menu->EnableMenuItem( ID_SOURCES_DELETE, MF_GRAYED | MF_DISABLED );
			popup_menu->EnableMenuItem( ID_SOURCES_EDITQUERY, MF_GRAYED | MF_DISABLED );
		}

		popup_menu->TrackPopupMenu( 0, pos.x, pos.y, this );
	}
}


///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesRename()
{
	GetCtrl()->RenameItem();
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesDelete()
{
	GetCtrl()->DeleteSel();
}

///////////////////////////////////////////////////

// CmusikSourcesCtrl

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CmusikSourcesCtrl, CmusikPropTree )

///////////////////////////////////////////////////

int WM_SOURCES_EDIT_COMMIT = RegisterWindowMessage( "MUSIKEDITCOMMIT" );
int WM_SOURCES_EDIT_CANCEL = RegisterWindowMessage( "MUSIKEDITCANCEL" );
int WM_SOURCES_EDIT_CHANGE = RegisterWindowMessage( "MUSIKEDITCHANGE" );

///////////////////////////////////////////////////

CmusikSourcesCtrl::CmusikSourcesCtrl( CFrameWnd* parent, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs, UINT dropid, UINT pldropid_r, UINT sbdropid_r )
	: CmusikPropTree( prefs, library, dropid )
{
	m_DropTarget		= new CmusikSourcesDropTarget( this, dropid, pldropid_r, sbdropid_r );
	m_Parent			= parent;
	m_LibRoot			= NULL;
	m_SrcRoot			= NULL;
	m_StdPlaylistRoot	= NULL;
	m_DynPlaylistRoot	= NULL;
	m_DropArrange		= false;
	m_Startup			= true;
	m_Player			= player;
	m_MouseTrack		= false;
}

///////////////////////////////////////////////////

CmusikSourcesCtrl::~CmusikSourcesCtrl()
{
	if ( m_DropTarget )
	{
		m_DropTarget->Revoke();
		delete m_DropTarget;
	}
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP( CmusikSourcesCtrl, CmusikPropTree )
	// mfc message maps
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_KEYDOWN()
	ON_WM_CONTEXTMENU()

	// custom message maps
	ON_REGISTERED_MESSAGE(WM_SOURCES_EDIT_COMMIT, OnEditCommit)
	ON_REGISTERED_MESSAGE(WM_SOURCES_EDIT_CANCEL, OnEditCancel)
	ON_REGISTERED_MESSAGE(WM_SOURCES_EDIT_CHANGE, OnEditChange)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikSourcesCtrl::InitItems()
{
	CmusikPlaylistInfo info;
	
	// "Music"
	info.Set( "Music", -1, -1 );
	m_LibRoot = InsertItem( new CmusikPropTreeItem() );
	m_LibRoot->SetPlaylistInfo( info );
	m_LibRoot->Expand( true );
	
	LibLoad();
	
	// "Search"
	info.Set( "Search", -1, -1 );
	m_SrcRoot = InsertItem( new CmusikPropTreeItem() );
	m_SrcRoot->SetPlaylistInfo( info );
	m_SrcRoot->Expand( 1 );

	SrcLoad();

	// standard playlists
	info.Set( "Standard Playlists", -1, -1 );
	m_StdPlaylistRoot = InsertItem( new CmusikPropTreeItem() );
	m_StdPlaylistRoot->SetPlaylistInfo( info );
	m_StdPlaylistRoot->Expand( true );

	StdPlaylistsLoad();

	// dynamic playlists
	info.Set( "Dynamic Playlists", -1, -1 );
	m_DynPlaylistRoot = InsertItem( new CmusikPropTreeItem() );
	m_DynPlaylistRoot->SetPlaylistInfo( info );
	m_DynPlaylistRoot->Expand( true );
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::CleanItems()
{
	ClearVisibleList();
	DeleteAllItems();
	
	m_LibRoot = NULL;
	m_SrcRoot = NULL;
	m_StdPlaylistRoot = NULL;
	m_DynPlaylistRoot = NULL;

	m_Lib.clear();
	m_Src.clear();
	m_StdPlaylists.clear();
	m_DynPlaylists.clear();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::SrcLoad()
{
	CmusikPlaylistInfo info;
	CmusikPropTreeItem* pSrc;

	info.Set( "Enter Search...", MUSIK_SOURCES_TYPE_QUICKSEARCH, -1 ); 
	pSrc = InsertItem( new CmusikPropTreeItem(), m_SrcRoot );
	pSrc->SetPlaylistInfo( info );

	m_Src.push_back( pSrc );
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FocusLibrary()
{
	KillFocus( false );

	m_Lib.at( 0 )->Select( TRUE );
	SetFocusedItem( m_Lib.at( 0 ) );
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FocusQuickSearch()
{
	KillFocus( false );

	m_Src.at( 0 )->Select( TRUE );
	SetFocusedItem( m_Src.at( 0 ) );
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FocusNowPlaying()
{
	KillFocus( false );

	m_Lib.at( 1 )->Select( TRUE );
	SetFocusedItem( m_Lib.at( 1 ) );
}

///////////////////////////////////////////////////

int CmusikSourcesCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CmusikPropTree::OnCreate(lpCreateStruct) == -1 )
		return -1;

	if ( m_DropTarget )
		m_DropTarget->Register( this );

	// quick search
	m_EditInPlace.Create( WS_CHILD | WS_CLIPCHILDREN, CRect( 0, 0, 0, 0 ), this, IDC_EDITINPLACE );
	m_EditInPlace.EnableWindow( FALSE );

	CFont font;
	font.CreateStockObject( DEFAULT_GUI_FONT );

	m_EditInPlace.SetFont( &font );

	InitItems();

	return 0;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::LibLoad()
{
	CmusikPlaylistInfo info;
	CmusikPropTreeItem *lib, *nowplaying;

	// library
	info.Set( "Library", MUSIK_SOURCES_TYPE_LIBRARY, NULL );
	lib = InsertItem( new CmusikPropTreeItem(), m_LibRoot );
	lib->SetPlaylistInfo( info );

	m_Lib.push_back( lib );

	// now playing
	info.Set( "Now Playing", MUSIK_SOURCES_TYPE_NOWPLAYING, NULL );
	nowplaying = InsertItem( new CmusikPropTreeItem(), m_LibRoot );
	nowplaying->SetPlaylistInfo( info );

	m_Lib.push_back( nowplaying );
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::StdPlaylistsLoad()
{
	CmusikPropTreeItem* temp;

	// load the playlists...
	for ( size_t i = 0; i < m_StdPlaylists.size(); i++ )
		DeleteItem( m_StdPlaylists.at( i ) );

	m_StdPlaylists.clear();

	CmusikPlaylistInfoArray items;
	m_Library->GetAllStdPlaylists( &items );

	CString sTemp;
	for ( size_t i = 0; i < items.size(); i++ )
	{
		temp = InsertItem( new CmusikPropTreeItem(), m_StdPlaylistRoot );
		temp->SetPlaylistInfo( items.at( i ) );

		m_StdPlaylists.push_back( temp );
	}

	// load the "Create..."
	CmusikPlaylistInfo info;
	info.Set( "Create...", MUSIK_SOURCES_TYPE_NEWSTDPLAYLIST, -1 ); 
	temp = InsertItem( new CmusikPropTreeItem(), m_StdPlaylistRoot );
	temp->SetPlaylistInfo( info );

	m_StdPlaylists.push_back( temp );
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::LoadDynPlaylists()
{

}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnDropFiles( HDROP hDropInfo, bool right_button )
{
	// set cursor back to hour glass
	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	// see if the drag landed on an existing
	// playlist, if it did, we'll append
	CPoint pos;
	::GetCursorPos( &pos );
	ScreenToClient( &pos );

	CmusikPropTreeItem* pItem = FindItem( pos );

	// drag originated from ourselves
	if ( m_DropArrange )
	{
		if ( pItem )
		{
			CmusikPropTreeItem* pSel = GetFocusedItem();

			if ( pSel != pItem && pSel->GetPlaylistType() == pItem->GetPlaylistType() )
			{
				MessageBox( "Successful drag" );
			}
		}
				
		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
		m_DropArrange = false;
		return;
	}

	if ( right_button )
	{
		if ( pItem && !pItem->IsRootLevel() )
		{
			KillFocus();
			pItem->Select( TRUE );
			SetFocusedItem( pItem );	
		}
	}

	// dnd stuff
	size_t nNumFiles;
	TCHAR szNextFile [MAX_PATH];
	SHFILEINFO  rFileInfo;
	
	nNumFiles = DragQueryFile ( hDropInfo, -1, NULL, 0 );
	CmusikStringArray files;

	CmusikString sTemp;
	for ( size_t i = 0; i < nNumFiles; i++ )
	{
		if ( DragQueryFile( hDropInfo, i, szNextFile, MAX_PATH ) > 0 )
		{
			// get the filetype. if its a directory
			// that was dropped, we'll want to 
			// recurse it and add all the supported 
			// media files...
			SHGetFileInfo( szNextFile, 0, &rFileInfo, sizeof( rFileInfo ), SHGFI_ATTRIBUTES );
			if ( rFileInfo.dwAttributes & SFGAO_FOLDER )
			{
				sTemp = szNextFile;
				sTemp += "\\*.*";

				m_Dir.m_Dir = sTemp;
				m_Dir.m_Target = &files;

				m_Dir.Run();
			}		

			// otherwise it was just a file... add it...
			else
				files.push_back( szNextFile );
		}

	}

	DragFinish( hDropInfo );

	// did we actually hit an item?
	if ( pItem && pItem != m_StdPlaylistRoot )
	{
		// standard playlist
		if ( pItem->GetPlaylistType() == MUSIK_PLAYLIST_TYPE_STANDARD )
			m_Library->AppendStdPlaylist( pItem->GetPlaylistID(), files );

		// hit now playing?
		else if ( pItem->GetPlaylistType() == MUSIK_SOURCES_TYPE_NOWPLAYING )
		{
			bool begin_trans = false;

			m_Library->BeginTransaction();
			CmusikSong song;
			for ( size_t i = 0; i < files.size(); i++ )
			{
				// add song (if necessary)
				m_Library->AddSong( files.at( i ) );

				if ( m_Library->GetSongFromFilename( files.at( i ), song ) )
					m_Player->GetPlaylist()->Add( song );
			}
			m_Library->EndTransaction();
		}

		// hit create?
		else if ( pItem->GetPlaylistType() == MUSIK_SOURCES_TYPE_NEWSTDPLAYLIST )
		{
			CreateNewStdPlaylist( &files );
			return;
		}
	}

	// didn't hit an item, a new 
	// playlist was created, so it
	// was pushed to the back of the list
	if ( !pItem || pItem == m_StdPlaylistRoot )
	{
		CreateNewStdPlaylist( &files );
		return;
	}

	if ( right_button && pItem )
		SendNotify( PTN_SELCHANGE, pItem );
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikSourcesCtrl::CreateNewStdPlaylist( CmusikStringArray* files )
{
	bool no_files = false;
	if ( !files )
	{
		files = new CmusikStringArray();
		no_files = true;
	}

	// create the playlist
	CmusikString playlist_str = _T( "New Playlist" );	
	int nID = m_Library->CreateStdPlaylist( playlist_str.c_str(), *files );

	// reload std playlists
	StdPlaylistsLoad();
	OnPaint();

	// rename new playlist
	CmusikPropTreeItem* pItem = NULL;
	if ( m_StdPlaylists.size() > 1 )
	{
		pItem = m_StdPlaylists.at( m_StdPlaylists.size() - 2 );
		RenameItem( pItem, MUSIK_SOURCES_EDITINPLACE_NEWSTDPLAYLIST );
	}

	if ( no_files )
		delete files;

	return pItem;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::KillFocus( bool redraw )
{
	CmusikPropTreeItem* pItem = GetFocusedItem();
	if ( pItem )
	{
		pItem->Select( FALSE );
		SetFocusedItem( NULL );

		if ( redraw )
			Invalidate();
	}
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::DoDrag( CmusikPropTreeItem* pItem )
{
	if ( !pItem )
		return;

	COleDataSource datasrc;
	HGLOBAL        hgDrop;
	DROPFILES*     pDrop;
	CStringList    lsDraggedFiles;
	POSITION       pos;
	CString        sFile;
	UINT           uBuffSize = 0;
	TCHAR*         pszBuff;
	FORMATETC      etc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	// get a list of filenames with the currently
	// selected items...
	CmusikStringArray files;

	int nMode = pItem->GetPlaylistType();
	
	// standard playlist dragged
	if ( nMode == MUSIK_PLAYLIST_TYPE_STANDARD )
		m_Library->GetStdPlaylistFns( pItem->GetPlaylistID(), files, false );
	
	// now playing dragged..
	else if ( nMode == MUSIK_SOURCES_TYPE_NOWPLAYING )
	{
		if ( m_Player->GetPlaylist() )
		{
			m_Library->BeginTransaction();
			for ( size_t i = 0; i < m_Player->GetPlaylist()->GetCount(); i++ )
				files.push_back( m_Player->GetPlaylist()->GetField( i, MUSIK_LIBRARY_TYPE_FILENAME ) );
			m_Library->EndTransaction();
		}
	}

	// library playlist dragged
	else if ( nMode == MUSIK_SOURCES_TYPE_LIBRARY )
	{
		CMainFrame* pMain = (CMainFrame*)m_Parent;
		if ( pMain->m_LibPlaylist )
		{
			m_Library->BeginTransaction();
			for ( size_t i = 0; i < pMain->m_LibPlaylist->GetCount(); i++ )
				files.push_back( pMain->m_LibPlaylist->GetField( i, MUSIK_LIBRARY_TYPE_FILENAME ) );
			m_Library->EndTransaction();
		}
	}

	else if ( nMode == MUSIK_PLAYLIST_TYPE_DYNAMIC )
		MessageBox( "This operation is not supported yet.", "musik", MB_ICONINFORMATION | MB_OK );


	if ( !files.size() )
		return;

	// CStringList containing files
	for ( size_t i = 0; i < files.size(); i++ )
	{
		lsDraggedFiles.AddTail( files.at( i ) );
		uBuffSize += files.at( i ).GetLength() + 1;
	}

	files.clear();

	// Add 1 extra for the final null char, and the size of the DROPFILES struct.
	uBuffSize = sizeof(DROPFILES) + sizeof(TCHAR) * (uBuffSize + 1);

	// Allocate memory from the heap for the DROPFILES struct.
	hgDrop = GlobalAlloc ( GHND | GMEM_SHARE, uBuffSize );

	if ( !hgDrop )
		return;

	pDrop = (DROPFILES*) GlobalLock ( hgDrop );

	if ( !pDrop )
	{
		GlobalFree ( hgDrop );
		return;
	}

	// Fill in the DROPFILES struct.
	pDrop->pFiles = sizeof(DROPFILES);

	// If we're compiling for Unicode, set the Unicode flag in the struct to
	// indicate it contains Unicode strings.
	#ifdef _UNICODE
		pDrop->fWide = TRUE;
	#endif;

	// Copy all the filenames into memory after the end of the DROPFILES struct.
	pos = lsDraggedFiles.GetHeadPosition();
	pszBuff = (TCHAR*) (LPBYTE(pDrop) + sizeof(DROPFILES));

	while ( pos )
	{
		lstrcpy ( pszBuff, (LPCTSTR) lsDraggedFiles.GetNext ( pos ) );
		pszBuff = 1 + _tcschr ( pszBuff, '\0' );
	}

	GlobalUnlock ( hgDrop );

	// Put the data in the data source.
	datasrc.CacheGlobalData ( CF_HDROP, hgDrop, &etc );

	// Add in our own custom data, so we know that the drag originated from our 
	// window.  CMyDropTarget::DragEnter() checks for this custom format, and
	// doesn't allow the drop if it's present.  This is how we prevent the user
	// from dragging and then dropping in our own window.
	// The data will just be a dummy bool.
	HGLOBAL hgBool;

	hgBool = GlobalAlloc ( GHND | GMEM_SHARE, sizeof(bool) );

	if ( NULL == hgBool )
	{
		GlobalFree ( hgDrop );
		return;
	}

	// Put the data in the data source.
	etc.cfFormat = m_DropID;
	datasrc.CacheGlobalData ( m_DropID, hgBool, &etc );

	// Start the drag 'n' drop!
	DROPEFFECT dwEffect = datasrc.DoDragDrop ( DROPEFFECT_COPY | DROPEFFECT_MOVE );

	// If the DnD completed OK, we remove all of the dragged items from our
	// list.
	switch ( dwEffect )
	{
		case DROPEFFECT_COPY:
		case DROPEFFECT_MOVE:
		{
			// the copy completed successfully
			// do whatever we need to do here
			TRACE0( "DND from playlist completed successfully. The data has a new owner.\n" );
		}
		break;

		case DROPEFFECT_NONE:
		{
			// This needs special handling, because on NT, DROPEFFECT_NONE
			// is returned for move operations, instead of DROPEFFECT_MOVE.
			// See Q182219 for the details.
			// So if we're on NT, we check each selected item, and if the
			// file no longer exists, it was moved successfully and we can
			// remove it from the list.
			if ( m_IsWinNT )
			{
				// the copy completed successfully
				// on a windows nt machine.
				// do whatever we need to do here

				bool bDeletedAnything = false;
				if ( ! bDeletedAnything )
				{
					// The DnD operation wasn't accepted, or was canceled, so we 
					// should call GlobalFree() to clean up.
					GlobalFree ( hgDrop );
					GlobalFree ( hgBool );

					TRACE0( "DND had a problem. We had to manually free the data.\n" );
				}
			}

			// not windows NT
			else
			{
				// We're on 9x, and a return of DROPEFFECT_NONE always means
				// that the DnD operation was aborted.  We need to free the
				// allocated memory.
				GlobalFree ( hgDrop );
				GlobalFree ( hgBool );

				TRACE0( "DND had a problem. We had to manually free the data.\n" );
			}
		}

		break;
	}	
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CmusikPropTree::OnShowWindow(bShow, nStatus);

	if ( m_Startup )
	{
		FocusLibrary();
		m_Startup = false;
	}
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::RenameItem( CmusikPropTreeItem* pItem, int mode, CPoint pos )
{
	if ( !pItem )
		pItem = GetFocusedItem();

	if ( !pItem )
		return;

	// not trying to rename a root
	if ( pItem->IsRootLevel() )
		return;

	// not renaming the library, now playing,
	// or quick search
	if ( pItem == m_Lib.at( 0 ) || pItem == m_Lib.at( 1 ) )
		return;

	CPoint nPos;
	if ( pos.x == -1 && pos.y == -1 )
		nPos = pItem->GetLocation();
	else
		nPos = pos;

	CRect rcClient;
	GetClientRect( rcClient );
	
	CRect rect( 20, nPos.y + 3, rcClient.Width(), nPos.y + PROPTREEITEM_DEFHEIGHT - 2 );

	m_EditInPlace.EnableWindow( TRUE );
	m_EditInPlace.SetArgs( (void*)mode );
	m_EditInPlace.MoveWindow( rect );
	m_EditInPlace.SetFocus();
	m_EditInPlace.SetString( pItem->GetLabelText() );
	m_EditInPlace.ShowWindow( SW_SHOWDEFAULT );
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::DeleteSel()
{
	CmusikPropTreeItem* pItem = GetFocusedItem();
	if ( pItem )
	{
		// type was standard playlist...
		if ( pItem->GetPlaylistType() == MUSIK_PLAYLIST_TYPE_STANDARD )
		{
			int nID = pItem->GetPlaylistID();
			int nPos = -1;
			int nNextPos = -1;
			
			// find the position...
			for ( size_t i = 0; i < m_StdPlaylists.size() - 1; i++ )
			{
				if ( m_StdPlaylists.at( i ) == pItem )
				{
					nPos = i;

					if ( nPos >= (int)m_StdPlaylists.size() - 2 )
						nNextPos = nPos - 1;
					else
						nNextPos = nPos;

					break;
				}
			}

			// if the position is valid, then delete it
			// and reload the playlists...
			if ( nPos != -1 )
			{
				m_Library->DeleteStdPlaylist( nID );
				
				StdPlaylistsLoad();
				OnPaint();

				// now select the next entry in the
				// list... if -1 select the now playing,
				// if playing, or library if not
				KillFocus( false );
				if ( nNextPos == -1 || m_StdPlaylists.size() <= 1 )
				{
					if ( !m_Player->IsPlaying() )
					{
						FocusLibrary();

						int WM_SOURCESLIBRARY = RegisterWindowMessage( "SOURCESLIBRARY" );
						m_Parent->SendMessage( WM_SOURCESLIBRARY, NULL );
					}
					else
					{
						FocusNowPlaying();

						int WM_SOURCESNOWPLAYING = RegisterWindowMessage( "SOURCESNOWPLAYING" );
						m_Parent->SendMessage( WM_SOURCESNOWPLAYING, NULL );
					}
				}
				else
				{
					KillFocus( false );
					m_StdPlaylists.at( nNextPos )->Select( TRUE );
					SetFocusedItem( m_StdPlaylists.at( nNextPos  ) );

					int WM_SOURCESSTDPLAYLIST = RegisterWindowMessage( "SOURCESSTDPLAYLIST" );
					m_Parent->SendMessage( WM_SOURCESSTDPLAYLIST, NULL );
				}

				Invalidate();
			}
		}		
	}
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// user pressed f2 to rename an entry
	if ( nChar == VK_F2 )
		RenameItem();

	// user requested playlist deletion
	if ( nChar == VK_DELETE )
		DeleteSel();
}

///////////////////////////////////////////////////

LRESULT CmusikSourcesCtrl::OnEditCancel( WPARAM wParam, LPARAM lParam )
{
	SetFocus();

	int cmd = (int)lParam;

	CmusikEditInPlace* sender = (CmusikEditInPlace*)wParam;

	if ( cmd == MUSIK_SOURCES_EDITINPLACE_NEWSTDPLAYLIST )
		m_StdPlaylists.at( m_StdPlaylists.size() - 1 )->SetLabelText( _T( "Create..." ) );

	else if ( cmd == MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH )
	{
		FinishQuickSearch();
		m_Src.at( 0 )->SetLabelText( "Enter Search..." );
	}
	
	m_EditInPlace.EnableWindow( FALSE );
	return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikSourcesCtrl::OnEditCommit( WPARAM wParam, LPARAM lParam )
{
	int cmd = (int)lParam;
	CmusikEditInPlace* sender = (CmusikEditInPlace*)wParam;
	CmusikPropTreeItem* pItem = GetFocusedItem();
	CString sName = sender->GetString();

	if ( cmd == MUSIK_SOURCES_EDITINPLACE_RENAME )
	{
		if ( pItem )
		{
			if ( pItem->GetPlaylistType() == MUSIK_PLAYLIST_TYPE_STANDARD )
			{
				m_Library->RenameStdPlaylist( pItem->GetPlaylistID(), (CmusikString)sName );
				pItem->SetLabelText( sName );
				Invalidate();
			}
		}
	}

	else if ( cmd == MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH )
	{
		if ( pItem )
		{
			m_Src.at( 0 )->SetLabelText( "Enter Search..." );
			FinishQuickSearch();
		}
	}

	else if ( cmd == MUSIK_SOURCES_EDITINPLACE_NEWSTDPLAYLIST )
	{
		if ( m_StdPlaylists.size() > 1 )
		{
			pItem = m_StdPlaylists.at( m_StdPlaylists.size() - 2 );
			if ( pItem )
			{
				if ( pItem->GetPlaylistType() == MUSIK_PLAYLIST_TYPE_STANDARD )
				{
					m_Library->RenameStdPlaylist( pItem->GetPlaylistID(), (CmusikString)sName );
					pItem->SetLabelText( sName );
					Invalidate();
				}
			}
		}
	}

	m_EditInPlace.EnableWindow( FALSE );

	SetFocus();
	return 0L;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	CmusikSourcesBar* pBar = (CmusikSourcesBar*)GetParent();
	pBar->ShowMenu();
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::QuickSearch()
{
	CmusikPropTreeItem* pTemp = m_Src.at( 0 );

	pTemp->SetLabelText( "" );
	
	CPoint nPos = pTemp->GetLocation();		

	CRect rcClient;
	GetClientRect( rcClient );
	
	CRect rect( 20, nPos.y + 3, rcClient.Width(), nPos.y + PROPTREEITEM_DEFHEIGHT - 2 );

	m_EditInPlace.EnableWindow( TRUE );
	m_EditInPlace.SetArgs( (void*)MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH );
	m_EditInPlace.MoveWindow( rect );
	m_EditInPlace.SetFocus();
	m_EditInPlace.SetString( m_EditInPlace.GetString() );
	m_EditInPlace.ShowWindow( SW_SHOWDEFAULT );
}

///////////////////////////////////////////////////

LRESULT CmusikSourcesCtrl::OnEditChange( WPARAM wParam, LPARAM lParam )
{
	int cmd = (int)lParam;

	if ( cmd == MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH )
	{
		CString curr_query;
		m_EditInPlace.GetWindowText( curr_query );

		if ( curr_query.GetLength() >= 2 || curr_query.Left( 1 ) == "!" )
		{
			if ( curr_query.Left( 1 ) == "!" )
				curr_query = curr_query.Right( curr_query.GetLength() - 1 );

			int WM_SOURCESQUICKSEARCH = RegisterWindowMessage( "SOURCESQUICKSEARCH" );
			CmusikSourcesBar* pBar = (CmusikSourcesBar*)GetParent();
			pBar->m_Parent->SendMessage( WM_SOURCESQUICKSEARCH, (WPARAM)&curr_query );
		}
	}

	return 0L;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FinishQuickSearch()
{
	CmusikSourcesBar* parent = (CmusikSourcesBar*)GetParent();

	FocusLibrary();

	int WM_SOURCESLIBRARY = RegisterWindowMessage( "SOURCESLIBRARY" );
	parent->GetParent()->SendMessage( WM_SOURCESLIBRARY, NULL );
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( !m_MouseTrack )
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof( tme );
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = m_hWnd;
		tme.dwHoverTime = HOVER_DEFAULT;
		::_TrackMouseEvent( &tme );

		m_MouseTrack = true; 	
	}

	if ( m_MouseTrack && ( nFlags & MK_LBUTTON ) )
	{
		CmusikPropTreeItem* pItem = FindItem( point );
		if ( pItem && pItem->GetPlaylistType() != -1 )
		{
			DoDrag( pItem );
			return;
		}
	}
}

///////////////////////////////////////////////////

LRESULT CmusikSourcesCtrl::OnMouseLeave( WPARAM wParam, LPARAM lParam )
{
	m_MouseTrack = false;

	return 1L;
}

///////////////////////////////////////////////////