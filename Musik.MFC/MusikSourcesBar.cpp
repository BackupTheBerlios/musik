///////////////////////////////////////////////////

#include "stdafx.h"

#include "Musik.h"
#include "MusikSourcesBar.h"

#include "../Musik.Core/include/MusikLibrary.h"

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////

CMusikSourcesBar::CMusikSourcesBar( CFrameWnd* parent, CMusikLibrary* library )
{
	m_wndChild = new CMusikSourcesCtrl( library );
	m_Parent = parent;
}

///////////////////////////////////////////////////

CMusikSourcesBar::~CMusikSourcesBar()
{
	delete m_wndChild;
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMusikSourcesBar, baseCMusikSourcesBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY( PTN_SELCHANGE, IDC_SOURCES, OnItemChanged )
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CMusikSourcesBar::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if ( baseCMusikSourcesBar::OnCreate(lpCreateStruct) == -1 )
		return -1;

	if ( !m_wndChild->Create( WS_CHILD | WS_VISIBLE | PTS_NOTIFY, CRect( 0, 0, 0, 0 ), this, IDC_SOURCES ) )
		return -1;

	if ( !m_Font.CreateStockObject( DEFAULT_GUI_FONT ) )
		return -1;

	m_wndChild->SetFont( &m_Font );

	return 0;
}

///////////////////////////////////////////////////

void CMusikSourcesBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBarCF::OnSize(nType, cx, cy);

	CRect client_size;
	GetClientRect( &client_size );

	m_wndChild->MoveWindow( 0, 0, client_size.Width(), client_size.Height() );
}

///////////////////////////////////////////////////

void CMusikSourcesBar::OnItemChanged( NMHDR* pNotifyStruct, LRESULT* plResult )
{
	LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;
    
	if ( pNMPropTree->pItem )
	{
		// we don't care about root level items,
		// they just act as headers.
		if ( pNMPropTree->pItem->IsRootLevel() )
			return;

		// library or device selected
		if ( pNMPropTree->pItem->GetSourcesType() == MUSIK_SOURCES_LIBRARY_OR_DEVICE )
		{
			int WM_SOURCESLIBRARY = RegisterWindowMessage( "SOURCESLIBRARY" );
			m_Parent->SendMessage( WM_SOURCESLIBRARY, NULL );
		}

		// standard playlist selected
		if ( pNMPropTree->pItem->GetSourcesType() == MUSIK_SOURCES_STANDARD_PLAYLIST )
		{
			int WM_SOURCESSTDPLAYLIST = RegisterWindowMessage( "SOURCESSTDPLAYLIST" );
			m_Parent->SendMessage( WM_SOURCESSTDPLAYLIST, NULL );
		}

		// standard playlist selected
		if ( pNMPropTree->pItem->GetSourcesType() == MUSIK_SOURCES_DYNAMIC_PLAYLIST )
		{
			int WM_SOURCESDYNPLAYLIST = RegisterWindowMessage( "SOURCESDYNDPLAYLIST" );		
			m_Parent->SendMessage( WM_SOURCESDYNPLAYLIST, NULL );
		}
	}

	*plResult = 0;
}

///////////////////////////////////////////////////
