///////////////////////////////////////////////////

#include "stdafx.h"

#include "Musik.h"
#include "MusikSourcesCtrl.h"

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
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CMusikSourcesCtrl::CreateHeaders()
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

	CreateHeaders();

	/*
	CPropTreeItem* pSourcesRoot;
	pSourcesRoot = InsertItem( new CPropTreeItem() );
	pSourcesRoot->SetLabelText( _T( "Libraries / Devices" ) );
	pSourcesRoot->SetInfoText(_T(""));
	pSourcesRoot->Expand( true );

	CPropTreeItem* pMusikLibrary;
	pMusikLibrary = InsertItem( new CPropTreeItem(), pSourcesRoot );
	pMusikLibrary->SetLabelText( _T( "Musik Library" ) );

	CPropTreeItem* pMusikiPod;
	pMusikiPod = InsertItem( new CPropTreeItem(), pSourcesRoot );
	pMusikiPod->SetLabelText( _T( "iPod" ) );

	CPropTreeItem* pMusikZen;
	pMusikZen = InsertItem( new CPropTreeItem(), pSourcesRoot );
	pMusikZen->SetLabelText( _T( "Zen" ) );

	CPropTreeItem* pStandardRoot;
	pStandardRoot = InsertItem( new CPropTreeItem() );
	pStandardRoot->SetLabelText( _T( "Standard Playlists" ) );
	pStandardRoot->SetInfoText(_T(""));
	pStandardRoot->Expand( true );

	CPropTreeItem* pStandardObsure;
	pStandardObsure = InsertItem( new CPropTreeItem(), pStandardRoot );
	pStandardObsure->SetLabelText( _T( "Obsure" ) );

	CPropTreeItem* pStandardRemix;
	pStandardRemix = InsertItem( new CPropTreeItem(), pStandardRoot );
	pStandardRemix->SetLabelText( _T( "remix" ) );

	CPropTreeItem* pDynamicRoot;
	pDynamicRoot = InsertItem( new CPropTreeItem() );
	pDynamicRoot->SetLabelText( _T( "Dynamic Playlists" ) );
	pDynamicRoot->SetInfoText(_T(""));
	pDynamicRoot->Expand( true );

	CPropTreeItem* pDynamicHighRated;
	pDynamicHighRated = InsertItem( new CPropTreeItem(), pDynamicRoot );
	pDynamicHighRated->SetLabelText( _T( "High Rated" ) );
	pDynamicHighRated->SetInfoText( _T( "" ) );
	*/

	return 0;
}

///////////////////////////////////////////////////

void CMusikSourcesCtrl::LoadLibraries()
{

}

///////////////////////////////////////////////////

void CMusikSourcesCtrl::LoadStdPlaylists()
{

}

///////////////////////////////////////////////////

void CMusikSourcesCtrl::LoadDynPlaylists()
{

}

///////////////////////////////////////////////////