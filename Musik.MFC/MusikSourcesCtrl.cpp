// MusikSourcesCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Musik.h"
#include "MusikSourcesCtrl.h"

// CMusikSourcesCtrl

IMPLEMENT_DYNAMIC( CMusikSourcesCtrl, CPropTree )

CMusikSourcesCtrl::CMusikSourcesCtrl()
{
}

CMusikSourcesCtrl::~CMusikSourcesCtrl()
{
}


BEGIN_MESSAGE_MAP( CMusikSourcesCtrl, CPropTree )
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CMusikSourcesCtrl message handlers
int CMusikSourcesCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CPropTree::OnCreate(lpCreateStruct) == -1 )
		return -1;

	CPropTreeItem* pLibraryRoot;
	pLibraryRoot = InsertItem( new CPropTreeItem() );
	pLibraryRoot->SetLabelText( _T( "Libraries" ) );
	pLibraryRoot->SetInfoText(_T(""));
	pLibraryRoot->Expand( true );

	CPropTreeItem* pMusikLibrary;
	pMusikLibrary = InsertItem( new CPropTreeItem(), pLibraryRoot );
	pMusikLibrary->SetLabelText( _T( "Musik Library" ) );

	CPropTreeItem* pStandardRoot;
	pStandardRoot = InsertItem( new CPropTreeItem() );
	pStandardRoot->SetLabelText( _T( "Standard Playlists" ) );
	pStandardRoot->SetInfoText(_T(""));

	CPropTreeItem* pDynamicRoot;
	pDynamicRoot = InsertItem( new CPropTreeItem() );
	pDynamicRoot->SetLabelText( _T( "Dynamic Playlists" ) );
	pDynamicRoot->SetInfoText(_T(""));

	return 0;
}