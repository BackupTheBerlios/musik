///////////////////////////////////////////////////

#include "stdafx.h"

#include "../Musik.Core/include/MusikPlayer.h"

#include "Musik.h"
#include "MusikSourcesCtrl.h"
#include "MusikSourcesDropTarget.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CMusikSourcesCtrl, CMusikPropTree )

///////////////////////////////////////////////////

CMusikSourcesCtrl::CMusikSourcesCtrl( CMusikLibrary* library, CMusikPlayer* player, CMusikPrefs* prefs, UINT dropid )
	: CMusikPropTree( prefs, library, dropid )
{
	m_DropTarget = new CMusikSourcesDropTarget( this, dropid );

	m_LibrariesRoot		= NULL;
	m_StdPlaylistRoot	= NULL;
	m_DynPlaylistRoot	= NULL;

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
	size_t nNumFiles;
	TCHAR szNextFile [MAX_PATH];
	
	nNumFiles = DragQueryFile ( hDropInfo, -1, NULL, 0 );
	CStdStringArray files;

	for ( size_t i = 0; i < nNumFiles; i++ )
	{
		if ( DragQueryFile( hDropInfo, i, szNextFile, MAX_PATH ) > 0 )
			files.push_back( szNextFile );
	}

	DragFinish( hDropInfo );

	CPoint pos;
	::GetCursorPos( &pos );
	ScreenToClient( &pos );

	// see if the drag landed on an existing
	// playlist, if it did, we'll append
	CMusikPropTreeItem* pItem = HitTestEx( pos );

	// did we actually hit an item?
	if ( pItem )
	{
		// standard playlist
		if ( pItem->GetPlaylistType() == MUSIK_PLAYLIST_TYPE_STANDARD )
			m_Library->AppendStdPlaylist( pItem->GetPlaylistID(), files, true );

		// hit now playing?
		else if ( pItem->GetPlaylistType() == MUSIK_SOURCES_TYPE_NOWPLAYING )
		{
			m_Library->BeginTransaction();
			CMusikSong song;
			for ( size_t i = 0; i < files.size(); i++ )
			{
				if ( !m_Library->IsSongInLibrary( files.at( i ) ) )
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
		m_Library->CreateStdPlaylist( playlist_str.c_str(), files, true );
	}

	// show
	LoadStdPlaylists();
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


