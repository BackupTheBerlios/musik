///////////////////////////////////////////////////

#include "stdafx.h"

#include "Musik.h"
#include "MusikSourcesCtrl.h"

#include "../Musik.Core/include/MusikLibrary.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CMusikSourcesCtrl, CPropTree )

///////////////////////////////////////////////////

CMusikSourcesCtrl::CMusikSourcesCtrl( CMusikLibrary* library )
{
	m_LibrariesRoot		= NULL;
	m_StdPlaylistRoot	= NULL;
	m_DynPlaylistRoot	= NULL;;
	m_Library			= library;
}

///////////////////////////////////////////////////

CMusikSourcesCtrl::~CMusikSourcesCtrl()
{
	// items will delete themselves
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP( CMusikSourcesCtrl, CPropTree )
	ON_WM_CREATE()
	ON_WM_DROPFILES()
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

	m_LibrariesRoot = InsertItem( new CPropTreeItem() );
	m_LibrariesRoot->SetLabelText( _T( "Libraries / Devices" ) );
	m_LibrariesRoot->SetInfoText(_T(""));
	m_LibrariesRoot->Expand( true );

	// standard playlists
	if ( m_StdPlaylistRoot )
	{
		DeleteItem( m_StdPlaylistRoot );
		delete m_StdPlaylistRoot;
	}

	m_StdPlaylistRoot = InsertItem( new CPropTreeItem() );
	m_StdPlaylistRoot->SetLabelText( _T( "Standard Playlists" ) );
	m_StdPlaylistRoot->SetInfoText(_T(""));
	m_StdPlaylistRoot->Expand( true );
	LoadStdPlaylists();

	// dynamic playlists
	if ( m_DynPlaylistRoot )
	{
		DeleteItem( m_DynPlaylistRoot );
		delete m_DynPlaylistRoot;
	}

	m_DynPlaylistRoot = InsertItem( new CPropTreeItem() );
	m_DynPlaylistRoot->SetLabelText( _T( "Dynamic Playlists" ) );
	m_DynPlaylistRoot->SetInfoText(_T(""));
	m_DynPlaylistRoot->Expand( true );
}

///////////////////////////////////////////////////

int CMusikSourcesCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CPropTree::OnCreate(lpCreateStruct) == -1 )
		return -1;

	DragAcceptFiles( true );
	InitItems();

	return 0;
}

///////////////////////////////////////////////////

void CMusikSourcesCtrl::LoadLibraries()
{

}

///////////////////////////////////////////////////

void CMusikSourcesCtrl::LoadStdPlaylists()
{
	for ( size_t i = 0; i < m_StdPlaylists.size(); i++ )
		DeleteItem( m_StdPlaylists.at( i ) );

	m_StdPlaylists.clear();

	CMusikPlaylistInfoArray items;
	m_Library->GetAllStdPlaylists( &items );

	CPropTreeItem* temp;
	for ( size_t i = 0; i < items.size(); i++ )
	{
		temp = InsertItem( new CPropTreeItem(), m_StdPlaylistRoot );
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

	m_Library->CreateStdPlaylist( "New Playlist 1", files );

	CPropTree::OnDropFiles(hDropInfo);
}

///////////////////////////////////////////////////

void CMusikSourcesCtrl::KillFocus()
{
	CPropTreeItem* pItem = GetFocusedItem();
	if ( pItem )
	{
		pItem->Select( FALSE );
		SetFocusedItem( NULL );
	}
}

///////////////////////////////////////////////////
