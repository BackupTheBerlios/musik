#include "wx/wxprec.h"
#include "MusikSourcesCtrl.h"

CMusikSourcesCtrl::CMusikSourcesCtrl( wxWindow* parent, wxWindowID id )
	: CMusikListCtrl( parent, id )
{
	Reset( true, true );
}

CMusikSourcesCtrl::~CMusikSourcesCtrl()
{
}

void CMusikSourcesCtrl::RescaleColumns( bool refresh )
{
	SetColumnWidth( 0, GetClientSize().GetWidth() );
	if ( refresh )
		Refresh( false );
}

void CMusikSourcesCtrl::Reset( bool rescale, bool refresh )
{
	ClearAll();
	InsertColumn( 0, wxT( "Sources" ) );
	if ( rescale )
		RescaleColumns( refresh );
}

void CMusikSourcesCtrl::Update( bool refresh )
{
	if ( refresh )
		Refresh( false );
}

void CMusikSourcesCtrl::OnResize( wxSizeEvent& event )
{
	RescaleColumns();
}

void CMusikSourcesCtrl::OnColBeginDrag( wxListEvent& event )
{
	event.Veto();
}