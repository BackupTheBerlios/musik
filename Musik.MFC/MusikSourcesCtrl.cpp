///////////////////////////////////////////////////

#include "stdafx.h"

#include "../Musik.Core/include/MusikPlayer.h"

#include "MainFrm.h"

#include "Musik.h"
#include "MusikSourcesCtrl.h"
#include "MusikSourcesDropTarget.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CMusikSourcesCtrl, CMusikPropTree )

///////////////////////////////////////////////////

CMusikSourcesCtrl::CMusikSourcesCtrl( CFrameWnd* parent, CMusikLibrary* library, CMusikPlayer* player, CMusikPrefs* prefs, UINT dropid )
	: CMusikPropTree( prefs, library, dropid )
{
	m_DropTarget = new CMusikSourcesDropTarget( this, dropid );

	m_Parent			= parent;

	m_LibrariesRoot		= NULL;
	m_StdPlaylistRoot	= NULL;
	m_DynPlaylistRoot	= NULL;

	m_Startup			= true;

	m_Player			= player;
}

///////////////////////////////////////////////////

CMusikSourcesCtrl::~CMusikSourcesCtrl()
{
	if ( m_DropTarget )
	{
		m_DropTarget->Revoke();
		delete m_DropTarget;
	}
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP( CMusikSourcesCtrl, CMusikPropTree )
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CMusikSourcesCtrl::InitItems()
{
	// libraries / devices
	if ( m_LibrariesRoot )
	{
		DeleteItem( m_LibrariesRoot );
		delete m_LibrariesRoot;
	}

	CMusikPlaylistInfo info;

	info.Set( "Music", -1, -1 );
	m_LibrariesRoot = InsertItem( new CMusikPropTreeItem() );
	m_LibrariesRoot->SetPlaylistInfo( info );
	m_LibrariesRoot->Expand( true );
	
	LoadLibraries();

	// standard playlists
	if ( m_StdPlaylistRoot )
	{
		DeleteItem( m_StdPlaylistRoot );
		delete m_StdPlaylistRoot;
	}

	info.Set( "Standard Playlists", -1, -1 );
	m_StdPlaylistRoot = InsertItem( new CMusikPropTreeItem() );
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
	m_DynPlaylistRoot = InsertItem( new CMusikPropTreeItem() );
	m_DynPlaylistRoot->SetPlaylistInfo( info );
	m_DynPlaylistRoot->Expand( true );
}

///////////////////////////////////////////////////

void CMusikSourcesCtrl::FocusLibrary()
{
	KillFocus( false );

	m_Libraries.at( 0 )->Select( TRUE );
	SetFocusedItem( m_Libraries.at( 0 ) );
}

///////////////////////////////////////////////////

void CMusikSourcesCtrl::FocusNowPlaying()
{
	KillFocus( false );

	m_Libraries.at( 1 )->Select( TRUE );
	SetFocusedItem( m_Libraries.at( 1 ) );
}

///////////////////////////////////////////////////

int CMusikSourcesCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CMusikPropTree::OnCreate(lpCreateStruct) == -1 )
		return -1;

	if ( m_DropTarget )
		m_DropTarget->Register( this );

	InitItems();

	return 0;
}

///////////////////////////////////////////////////

void CMusikSourcesCtrl::LoadLibraries()
{
	CMusikPlaylistInfo info;
	CMusikPropTreeItem* temp;

	// library
	info.Set( "Library", MUSIK_SOURCES_TYPE_LIBRARY, NULL );
	temp = InsertItem( new CMusikPropTreeItem(), m_LibrariesRoot );
	temp->SetPlaylistInfo( info );
	m_Libraries.push_back( temp );

	// now playing
	info.Set( "Now Playing", MUSIK_SOURCES_TYPE_NOWPLAYING, NULL );
	temp = InsertItem( new CMusikPropTreeItem(), m_LibrariesRoot );
	temp->SetPlaylistInfo( info );
	m_Libraries.push_back( temp );
}

///////////////////////////////////////////////////

void CMusikSourcesCtrl::LoadStdPlaylists()
{
	for ( size_t i = 0; i < m_StdPlaylists.size(); i++ )
		DeleteItem( m_StdPlaylists.at( i ) );

	m_StdPlaylists.clear();

	CMusikPlaylistInfoArray items;
	m_Library->GetAllStdPlaylists( &items );

	CMusikPropTreeItem* temp;
	for ( size_t i = 0; i < items.size(); i++ )
	{
		temp = InsertItem( new CMusikPropTreeItem(), m_StdPlaylistRoot );
		temp->SetPlaylistInfo( items.at( i ) );

		m_StdPlaylists.push_back( temp );
	}
}

///////////////////////////////////////////////////

void CMusikSourcesCtrl::LoadDynPlaylists()
{

}

///////////////////////////////////////////////////

void CMusikSourcesCtrl::OnDropFiles(HDROP hDropInfo)
{
	// set cursor back to hour glass
	SetCursor( LoadCursor( NULL, IDC_WAIT ) );

	// see if the drag landed on an existing
	// playlist, if it did, we'll append
	CPoint pos;
	::GetCursorPos( &pos );
	ScreenToClient( &pos );

	CMusikPropTreeItem* pItem = FindItem( pos );

	// make sure the item isn't root
	if ( pItem != NULL && pItem->IsRootLevel() )
		return;

	if ( pItem )
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
				m_Dir.m_Threaded = false;
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
			m_Library->BeginTransaction();
			CMusikSong song;
			for ( size_t i = 0; i < files.size(); i++ )
			{
				// add song (if necessary)
				m_Library->AddSong( files.at( i ) );

				m_Library->GetSongFromFilename( files.at( i ), song );
				m_Player->GetPlaylist()->Add( song );
			}
			m_Library->EndTransaction();
		}
	}

	// else make a new playlist
	else
	{
		CStdString playlist_str;
		playlist_str.Format( _T( "New Playlist %d" ), m_StdPlaylists.size() );
		m_Library->CreateStdPlaylist( playlist_str.c_str(), files );
		LoadStdPlaylists();
	}

	// didn't hit an item, a new 
	// playlist was created, so it
	// was pushed to the back of the list
	if ( !pItem )
	{
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

void CMusikSourcesCtrl::KillFocus( bool redraw )
{
	CMusikPropTreeItem* pItem = GetFocusedItem();
	if ( pItem )
	{
		pItem->Select( FALSE );
		SetFocusedItem( NULL );

		if ( redraw )
			Invalidate();
	}
}

///////////////////////////////////////////////////

void CMusikSourcesCtrl::DoDrag( CMusikPropTreeItem* pItem )
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
		MessageBox( "This operation is not supported yet.", "Musik", MB_ICONINFORMATION | MB_OK );


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

CMusikPropTreeItem* CMusikSourcesCtrl::FindItem( const POINT& pt )
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

	// item couldn't be found
	return NULL;
}

///////////////////////////////////////////////////

void CMusikSourcesCtrl::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CMusikPropTree::OnShowWindow(bShow, nStatus);

	if ( m_Startup )
	{
		FocusLibrary();
		m_Startup = false;
	}
}

///////////////////////////////////////////////////

