///////////////////////////////////////////////////

#include "stdafx.h"

#include "../musikCore/include/musikPlayer.h"

#include "MainFrm.h"

#include "musik.h"
#include "musikSourcesCtrl.h"
#include "musikSourcesDropTarget.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CmusikSourcesCtrl, CmusikPropTree )

///////////////////////////////////////////////////

int WM_MUSIKEDITCOMMIT_SOURCES = RegisterWindowMessage( "MUSIKEDITCOMMIT" );
int WM_MUSIKEDITCANCEL_SOURCES = RegisterWindowMessage( "MUSIKEDITCANCEL" );

///////////////////////////////////////////////////

CmusikSourcesCtrl::CmusikSourcesCtrl( CFrameWnd* parent, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs, UINT dropid )
	: CmusikPropTree( prefs, library, dropid )
{
	m_DropTarget		= new CmusikSourcesDropTarget( this, dropid );
	m_Parent			= parent;
	m_LibrariesRoot		= NULL;
	m_StdPlaylistRoot	= NULL;
	m_DynPlaylistRoot	= NULL;
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
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_REGISTERED_MESSAGE(WM_MUSIKEDITCOMMIT_SOURCES,OnEditCommit)
	ON_REGISTERED_MESSAGE(WM_MUSIKEDITCANCEL_SOURCES,OnEditCancel)
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


	// edit in place
	m_EditInPlace.Create( WS_CHILD | WS_CLIPCHILDREN, CRect( 0, 0, 0, 0 ), this, 123 );
	m_EditInPlace.EnableWindow( FALSE );

	CFont font;
	font.CreateStockObject( DEFAULT_GUI_FONT );
	m_EditInPlace.SetFont( &font );

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

	// make sure the item isn't root
	if ( pItem != NULL && pItem != m_StdPlaylistRoot )
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
			CmusikSong song;
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

void CmusikSourcesCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_LockHover = true;

	// user pressed f2 to rename an entry
	if ( nChar == VK_F2 )
	{
		CmusikPropTreeItem* pItem = GetFocusedItem();
	
		// not trying to rename a root
		if ( pItem->IsRootLevel() )
			return;

		// not renaming the library or the
		// now playing...
		if ( pItem == m_Libraries.at( 0 ) || pItem == m_Libraries.at( 1 ) )
			return;

		if ( pItem )
		{
			CPoint nPos = pItem->GetLocation();		
	
			CRect rcClient;
			GetClientRect( rcClient );
			
			CRect rect( 20, nPos.y + 1, rcClient.Width(), nPos.y + PROPTREEITEM_DEFHEIGHT - 2 );

			m_EditInPlace.EnableWindow( TRUE );
			m_EditInPlace.MoveWindow( rect );
			m_EditInPlace.SetFocus();
			m_EditInPlace.SetString( pItem->GetLabelText() );
			m_EditInPlace.ShowWindow( SW_SHOWDEFAULT );
		}
	}

	// user requested playlist deletion
	if ( nChar == VK_DELETE )
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
					// list... if -1 select the library...
					KillFocus( false );
					if ( nNextPos == -1 )
					{
						m_Libraries.at( 0 )->Select( TRUE );
						SetFocusedItem( m_Libraries.at( 0 ) );
						int WM_SOURCESLIBRARY = RegisterWindowMessage( "SOURCESLIBRARY" );
						m_Parent->SendMessage( WM_SOURCESLIBRARY, NULL );
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

	m_EditInPlace.EnableWindow( FALSE );

	return 0L;
}

///////////////////////////////////////////////////

LRESULT CmusikSourcesCtrl::OnEditCommit( WPARAM wParam, LPARAM lParam )
{
	CString sName = m_EditInPlace.GetString();
	
    CmusikPropTreeItem* pItem = GetFocusedItem();
	if ( GetFocusedItem() )
	{
		if ( pItem->GetPlaylistType() == MUSIK_PLAYLIST_TYPE_STANDARD )
		{
			m_Library->RenameStdPlaylist( pItem->GetPlaylistID(), (CStdString)sName );
			pItem->SetLabelText( sName );
			Invalidate();
		}
	}

	m_EditInPlace.EnableWindow( FALSE );

	SetFocus();
	return 0L;
}

///////////////////////////////////////////////////