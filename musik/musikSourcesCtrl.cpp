///////////////////////////////////////////////////

#include "stdafx.h"

#include "../musikCore/include/musikPlayer.h"

#include "MainFrm.h"

#include "musik.h"
#include "musikSourcesCtrl.h"
#include "musikSourcesDropTarget.h"
#include ".\musiksourcesctrl.h"

///////////////////////////////////////////////////

// CmusikSourcesBar

///////////////////////////////////////////////////

CmusikSourcesBar::CmusikSourcesBar( CFrameWnd* parent, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs, UINT dropid )
	: CmusikDockBar( prefs )
{
	m_wndChild = new CmusikSourcesCtrl( parent, library, player, prefs, dropid );
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
    
	if ( pNMPropTree->pItem )
	{
		// we don't care about root level items,
		// they just act as headers.
		if ( pNMPropTree->pItem->IsRootLevel() || pNMPropTree->pItem->IsActivated() )
			return;

		// library selected
		if ( pNMPropTree->pItem->GetPlaylistType() == MUSIK_SOURCES_TYPE_LIBRARY )
		{
			int WM_SOURCESLIBRARY = RegisterWindowMessage( "SOURCESLIBRARY" );
			m_Parent->SendMessage( WM_SOURCESLIBRARY, NULL );
		}

		// quick search
		if ( pNMPropTree->pItem->GetPlaylistType() == MUSIK_SOURCES_TYPE_QUICKSEARCH )
		{
			CmusikSourcesCtrl* pCtrl = (CmusikSourcesCtrl*)GetCtrl();
			pCtrl->QuickSearch();
		}

		// now playing selected
		else if ( pNMPropTree->pItem->GetPlaylistType() == MUSIK_SOURCES_TYPE_NOWPLAYING )
		{
			int WM_SOURCESNOWPLAYING = RegisterWindowMessage( "SOURCESNOWPLAYING" );
			m_Parent->SendMessage( WM_SOURCESNOWPLAYING, NULL );
		}

		// standard playlist selected
		else if ( pNMPropTree->pItem->GetPlaylistType() == MUSIK_PLAYLIST_TYPE_STANDARD )
		{
			int WM_SOURCESSTDPLAYLIST = RegisterWindowMessage( "SOURCESSTDPLAYLIST" );
			m_Parent->SendMessage( WM_SOURCESSTDPLAYLIST, NULL );
		}

		// standard playlist selected
		else if ( pNMPropTree->pItem->GetPlaylistType() == MUSIK_PLAYLIST_TYPE_DYNAMIC )
		{
			int WM_SOURCESDYNPLAYLIST = RegisterWindowMessage( "SOURCESDYNDPLAYLIST" );		
			m_Parent->SendMessage( WM_SOURCESDYNPLAYLIST, NULL );
		}
	}
	else
		GetCtrl()->FocusLibrary();

	*plResult = 0;
}

///////////////////////////////////////////////////

void CmusikSourcesBar::OnOptions()
{
	ShowMenu();
}

///////////////////////////////////////////////////

void CmusikSourcesBar::ShowMenu()
{
	CPoint pos;
	::GetCursorPos( &pos );

	CMenu main_menu;
	CMenu* popup_menu;

	main_menu.LoadMenu( IDR_SOURCES_MENU );
	popup_menu = main_menu.GetSubMenu( 0 );

	popup_menu->TrackPopupMenu( 0, pos.x, pos.y, this );
}


///////////////////////////////////////////////////

void CmusikSourcesBar::OnSourcesRename()
{
	GetCtrl()->RenameSel();
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

CmusikSourcesCtrl::CmusikSourcesCtrl( CFrameWnd* parent, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs, UINT dropid )
	: CmusikPropTree( prefs, library, dropid )
{
	m_DropTarget		= new CmusikSourcesDropTarget( this, dropid );
	m_Parent			= parent;
	m_LibrariesRoot		= NULL;
	m_StdPlaylistRoot	= NULL;
	m_DynPlaylistRoot	= NULL;
	m_QuickSearch		= NULL;
	m_DropArrange		= false;
	m_Startup			= true;
	m_Player			= player;
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
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()

	// custom message maps
	ON_REGISTERED_MESSAGE(WM_SOURCES_EDIT_COMMIT, OnEditCommit)
	ON_REGISTERED_MESSAGE(WM_SOURCES_EDIT_CANCEL, OnEditCancel)
	ON_REGISTERED_MESSAGE(WM_SOURCES_EDIT_CHANGE, OnEditChange)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikSourcesCtrl::InitItems()
{
	// libraries / devices
	if ( m_LibrariesRoot )
	{
		DeleteItem( m_LibrariesRoot );
		delete m_LibrariesRoot;
	}

	CmusikPlaylistInfo info;

	info.Set( "Music", -1, -1 );
	m_LibrariesRoot = InsertItem( new CmusikPropTreeItem() );
	m_LibrariesRoot->SetPlaylistInfo( info );
	m_LibrariesRoot->Expand( true );
	
	LoadLibraries();

	// quick search
	if ( m_QuickSearch )
	{
		DeleteItem( m_QuickSearch );
		delete m_QuickSearch;
	}

	info.Set( "Quick Search", -1, -1 );
	m_QuickSearchRoot = InsertItem( new CmusikPropTreeItem() );
	m_QuickSearchRoot->SetPlaylistInfo( info );
	m_QuickSearchRoot->Expand( 1 );

	info.Set( "Search...", MUSIK_SOURCES_TYPE_QUICKSEARCH, -1 ); 
	m_QuickSearch = InsertItem( new CmusikPropTreeItem(), m_QuickSearchRoot );
	m_QuickSearch->SetPlaylistInfo( info );

	// standard playlists
	if ( m_StdPlaylistRoot )
	{
		DeleteItem( m_StdPlaylistRoot );
		delete m_StdPlaylistRoot;
	}

	info.Set( "Standard Playlists", -1, -1 );
	m_StdPlaylistRoot = InsertItem( new CmusikPropTreeItem() );
	m_StdPlaylistRoot->SetPlaylistInfo( info );
	m_StdPlaylistRoot->Expand( true );
	LoadStdPlaylists();

	// dynamic playlists
	if ( m_DynPlaylistRoot )
	{
		DeleteItem( m_DynPlaylistRoot );
		delete m_DynPlaylistRoot;
	}

	info.Set( "Dynamic Playlists", -1, -1 );
	m_DynPlaylistRoot = InsertItem( new CmusikPropTreeItem() );
	m_DynPlaylistRoot->SetPlaylistInfo( info );
	m_DynPlaylistRoot->Expand( true );
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FocusLibrary()
{
	KillFocus( false );

	m_Libraries.at( 0 )->Select( TRUE );
	SetFocusedItem( m_Libraries.at( 0 ) );
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FocusQuickSearch()
{
	KillFocus( false );

	m_QuickSearch->Select( TRUE );
	SetFocusedItem( m_QuickSearch );
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::FocusNowPlaying()
{
	KillFocus( false );

	m_Libraries.at( 1 )->Select( TRUE );
	SetFocusedItem( m_Libraries.at( 1 ) );
}

///////////////////////////////////////////////////

int CmusikSourcesCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CmusikPropTree::OnCreate(lpCreateStruct) == -1 )
		return -1;

	if ( m_DropTarget )
		m_DropTarget->Register( this );

	// quick search
	m_QuickSearchCtrl.Create( WS_CHILD | WS_CLIPCHILDREN, CRect( 0, 0, 0, 0 ), this, IDC_QUICKSEARCH );
	m_QuickSearchCtrl.EnableWindow( FALSE );

	// edit in place
	m_EditInPlace.Create( WS_CHILD | WS_CLIPCHILDREN, CRect( 0, 0, 0, 0 ), this, 123 );
	m_EditInPlace.EnableWindow( FALSE );

	CFont font;
	font.CreateStockObject( DEFAULT_GUI_FONT );

	m_EditInPlace.SetFont( &font );
	m_QuickSearchCtrl.SetFont( &font );

	InitItems();

	return 0;
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::LoadLibraries()
{
	CmusikPlaylistInfo info;
	CmusikPropTreeItem* temp;

	// library
	info.Set( "Library", MUSIK_SOURCES_TYPE_LIBRARY, NULL );
	temp = InsertItem( new CmusikPropTreeItem(), m_LibrariesRoot );
	temp->SetPlaylistInfo( info );
	m_Libraries.push_back( temp );

	// now playing
	info.Set( "Now Playing", MUSIK_SOURCES_TYPE_NOWPLAYING, NULL );
	temp = InsertItem( new CmusikPropTreeItem(), m_LibrariesRoot );
	temp->SetPlaylistInfo( info );
	m_Libraries.push_back( temp );
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::LoadStdPlaylists()
{
	for ( size_t i = 0; i < m_StdPlaylists.size(); i++ )
		DeleteItem( m_StdPlaylists.at( i ) );

	m_StdPlaylists.clear();

	CmusikPlaylistInfoArray items;
	m_Library->GetAllStdPlaylists( &items );

	CmusikPropTreeItem* temp;
	for ( size_t i = 0; i < items.size(); i++ )
	{
		temp = InsertItem( new CmusikPropTreeItem(), m_StdPlaylistRoot );
		temp->SetPlaylistInfo( items.at( i ) );

		m_StdPlaylists.push_back( temp );
	}
}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::LoadDynPlaylists()
{

}

///////////////////////////////////////////////////

void CmusikSourcesCtrl::OnDropFiles(HDROP hDropInfo)
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

	if ( pItem && !pItem->IsRootLevel() )
	{
		KillFocus();
		pItem->Select( TRUE );
		SetFocusedItem( pItem );	
	}

	// dnd stuff
	size_t nNumFiles;
	TCHAR szNextFile [MAX_PATH];
	SHFILEINFO  rFileInfo;
	
	nNumFiles = DragQueryFile ( hDropInfo, -1, NULL, 0 );
	CStdStringArray files;

	CStdString sTemp;
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
	if ( pItem )
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
	}

	// didn't hit an item, a new 
	// playlist was created, so it
	// was pushed to the back of the list
	if ( !pItem || pItem == m_StdPlaylistRoot )
	{
		CStdString playlist_str;
		playlist_str.Format( _T( "New Playlist %d" ), m_StdPlaylists.size() );
		m_Library->CreateStdPlaylist( playlist_str.c_str(), files );
		LoadStdPlaylists();

		pItem = m_StdPlaylists.at( m_StdPlaylists.size() - 1 );

		// focus
		KillFocus();
		pItem->Select( TRUE );
		SetFocusedItem( pItem );
	}

	if ( pItem )
		SendNotify( PTN_SELCHANGE, pItem );
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
	CStdStringArray files;

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

int CmusikSourcesCtrl::FindInDynPlaylists( CmusikPropTreeItem* pItem )
{
	for ( size_t i = 0; i < m_DynPlaylists.size(); i++ )
	{
		if ( m_DynPlaylists.at( i ) == pItem )
			return i;
	}

	return -1;
}

///////////////////////////////////////////////////

int CmusikSourcesCtrl::FindInStdPlaylists( CmusikPropTreeItem* pItem )
{
	for ( size_t i = 0; i < m_StdPlaylists.size(); i++ )
	{
		if ( m_StdPlaylists.at( i ) == pItem )
			return i;
	}

	return -1;
}

///////////////////////////////////////////////////

int CmusikSourcesCtrl::FindInLibraries( CmusikPropTreeItem* pItem )
{
	for ( size_t i = 0; i < m_Libraries.size(); i++ )
	{
		if ( m_Libraries.at( i ) == pItem )
			return i;
	}

	return -1;
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikSourcesCtrl::FindItem( const POINT& pt )
{
	CPoint p = pt;
	CPoint ipt;

	// convert screen to tree coordinates
	p.y += m_ScrollPos;

	// try the library section first...
	for ( size_t i = 0; i < m_Libraries.size(); i++ )
	{
		ipt = m_Libraries.at( i )->GetLocation();
		if ( p.y >= ipt.y && p.y < ipt.y + m_Libraries.at( i )->GetHeight() )
			return m_Libraries.at( i );
	}

	// quick search
	ipt = m_QuickSearch->GetLocation();
	if ( p.y >= ipt.y && p.y < ipt.y + m_QuickSearch->GetHeight() )
		return m_QuickSearch;

	// standard playlists...
	for ( size_t i = 0; i < m_StdPlaylists.size(); i++ )
	{
		ipt = m_StdPlaylists.at( i )->GetLocation();
		if ( p.y >= ipt.y && p.y < ipt.y + m_StdPlaylists.at( i )->GetHeight() )
			return m_StdPlaylists.at( i );
	}

	// standard playlists...
	for ( size_t i = 0; i < m_DynPlaylists.size(); i++ )
	{
		ipt = m_DynPlaylists.at( i )->GetLocation();
		if ( p.y >= ipt.y && p.y < ipt.y + m_DynPlaylists.at( i )->GetHeight() )
			return m_DynPlaylists.at( i );
	}

	// fall back to root items...
	ipt = m_LibrariesRoot->GetLocation();
	if ( p.y >= ipt.y && p.y < ipt.y + m_LibrariesRoot->GetHeight() )
		return m_LibrariesRoot;
	
	ipt = m_StdPlaylistRoot->GetLocation();
	if ( p.y >= ipt.y && p.y < ipt.y + m_StdPlaylistRoot->GetHeight() )
		return m_StdPlaylistRoot;

	ipt = m_DynPlaylistRoot->GetLocation();
	if ( p.y >= ipt.y && p.y < ipt.y + m_DynPlaylistRoot->GetHeight() )
		return m_DynPlaylistRoot;

	ipt = m_QuickSearchRoot->GetLocation();
	if ( p.y >= ipt.y && p.y < ipt.y + m_DynPlaylistRoot->GetHeight() )
		return m_QuickSearchRoot;

	// item couldn't be found
	return NULL;
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

void CmusikSourcesCtrl::RenameSel()
{
	CmusikPropTreeItem* pItem = GetFocusedItem();

	// not trying to rename a root
	if ( pItem->IsRootLevel() )
		return;

	// not renaming the library, now playing,
	// or quick search
	if ( pItem == m_Libraries.at( 0 ) || pItem == m_Libraries.at( 1 ) || pItem == m_QuickSearch )
		return;

	if ( pItem )
	{
		CPoint nPos = pItem->GetLocation();		

		CRect rcClient;
		GetClientRect( rcClient );
		
		CRect rect( 20, nPos.y + 3, rcClient.Width(), nPos.y + PROPTREEITEM_DEFHEIGHT - 2 );

		m_EditInPlace.EnableWindow( TRUE );
		m_EditInPlace.MoveWindow( rect );
		m_EditInPlace.SetFocus();
		m_EditInPlace.SetString( pItem->GetLabelText() );
		m_EditInPlace.ShowWindow( SW_SHOWDEFAULT );
	}
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
			for ( size_t i = 0; i < m_StdPlaylists.size(); i++ )
			{
				if ( m_StdPlaylists.at( i ) == pItem )
				{
					nPos = i;

					if ( nPos == m_StdPlaylists.size() - 1 )
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
				LoadStdPlaylists();

				// now select the next entry in the
				// list... if -1 select the now playing,
				// if playing, or library if not
				KillFocus( false );
				if ( nNextPos == -1 )
				{
					if ( !m_Player->IsPlaying() )
					{
						m_Libraries.at( 0 )->Select( TRUE );
						SetFocusedItem( m_Libraries.at( 0 ) );
						int WM_SOURCESLIBRARY = RegisterWindowMessage( "SOURCESLIBRARY" );
						m_Parent->SendMessage( WM_SOURCESLIBRARY, NULL );
					}
					else
					{
						m_Libraries.at( 1 )->Select( TRUE );
						SetFocusedItem( m_Libraries.at( 1 ) );
						int WM_SOURCESNOWPLAYING = RegisterWindowMessage( "SOURCESNOWPLAYING" );
						m_Parent->SendMessage( WM_SOURCESNOWPLAYING, NULL );
					}
				}
				else
				{
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
	m_LockHover = true;

	// user pressed f2 to rename an entry
	if ( nChar == VK_F2 )
		RenameSel();

	// user requested playlist deletion
	if ( nChar == VK_DELETE )
		DeleteSel();

	m_LockHover = false;
}

///////////////////////////////////////////////////

// something here breaks if deleting
// items while moving the mouse... not
// exactly sure why. 

void CmusikSourcesCtrl::OnNewHoveredItem( int nIndex )
{
	/*
	// hover is still locked from OnMouseMove()
	// its waiting for the function to return 
	// to unlock...
	CmusikPropTreeItem* pItem = FindItemAtIndex( nIndex );
	if ( pItem )
	{
		CmusikPropTreeItem* pOldItem = GetHoveredItem();
		if ( pOldItem )
			pOldItem->Hover( FALSE );

		pItem->Hover( TRUE );
		SetHoveredItem( pItem );
		Invalidate();
	}
	*/
}

///////////////////////////////////////////////////

CmusikPropTreeItem* CmusikSourcesCtrl::FindItemAtIndex( int nIndex )
{
	CmusikPropTreeItem* pItem = NULL;

	// inside library array?
    for ( size_t i = 0; i < m_Libraries.size(); i++ )
	{
		if ( nIndex == ( i + 1 ) )
			pItem = m_Libraries.at( i );
	}

	// inside std array?
    for ( size_t i = 0; i < m_StdPlaylists.size(); i++ )
	{
		if ( nIndex == ( i + m_Libraries.size() + 2 ) )
			pItem = m_StdPlaylists.at( i );
	}

	// inside dyn array?
    for ( size_t i = 0; i < m_DynPlaylists.size(); i++ )
	{
		if ( nIndex ==  ( i + m_Libraries.size() + m_StdPlaylists.size() + 3 ) )
			pItem = m_DynPlaylists.at( i );
	}

	m_LockHover = false;

	return pItem;

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

LRESULT CmusikSourcesCtrl::OnEditCancel( WPARAM wParam, LPARAM lParam )
{
	SetFocus();

	CmusikEditInPlace* sender = (CmusikEditInPlace*)wParam;

	if ( sender == &m_EditInPlace )
		m_EditInPlace.EnableWindow( FALSE );
	else
	{
		FinishQuickSearch();
		m_QuickSearch->SetLabelText( "Search..." );
		m_QuickSearchCtrl.EnableWindow( FALSE );
	}

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikSourcesCtrl::OnEditCommit( WPARAM wParam, LPARAM lParam )
{
	CmusikEditInPlace* sender = (CmusikEditInPlace*)wParam;
	CmusikPropTreeItem* pItem = GetFocusedItem();

	if ( sender == &m_EditInPlace )
	{
		CString sName = sender->GetString();
		
		if ( pItem )
		{
			if ( pItem->GetPlaylistType() == MUSIK_PLAYLIST_TYPE_STANDARD )
			{
				m_Library->RenameStdPlaylist( pItem->GetPlaylistID(), (CStdString)sName );
				pItem->SetLabelText( sName );
				Invalidate();
			}
		}

		m_EditInPlace.EnableWindow( FALSE );
	}
	else if ( sender == &m_QuickSearchCtrl )
	{
		if ( pItem )
		{
			m_QuickSearch->SetLabelText( "Search..." );
			FinishQuickSearch();
		}
	}

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
	if ( m_QuickSearch )
	{
		m_QuickSearch->SetLabelText( "" );
		
		CPoint nPos = m_QuickSearch->GetLocation();		

		CRect rcClient;
		GetClientRect( rcClient );
		
		CRect rect( 20, nPos.y + 3, rcClient.Width(), nPos.y + PROPTREEITEM_DEFHEIGHT - 2 );

		m_QuickSearchCtrl.EnableWindow( TRUE );
		m_QuickSearchCtrl.MoveWindow( rect );
		m_QuickSearchCtrl.SetFocus();
		m_QuickSearchCtrl.SetString( m_QuickSearchCtrl.GetString() );
		m_QuickSearchCtrl.ShowWindow( SW_SHOWDEFAULT );
	}
}

///////////////////////////////////////////////////

LRESULT CmusikSourcesCtrl::OnEditChange( WPARAM wParam, LPARAM lParam )
{
	CmusikEditInPlace* sender = (CmusikEditInPlace*)wParam;

	if ( sender == &m_QuickSearchCtrl )
	{
		CString curr_query;
		m_QuickSearchCtrl.GetWindowText( curr_query );

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

