///////////////////////////////////////////////////

#include "stdafx.h"

#include "musik.h"
#include "musikSourcesBar.h"

#include "../musikCore/include/musikLibrary.h"

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
