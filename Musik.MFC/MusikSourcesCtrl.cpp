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
	pMusikiPod = InsertItem( new CPropTreeItem(), pSourcesRoot );
	pMusikiPod->SetLabelText( _T( "Zen" ) );

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

	return 0;
}