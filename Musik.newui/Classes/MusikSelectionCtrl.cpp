#include "wx/wxprec.h"
#include "MusikSelectionCtrl.h"

CMusikSelectionCtrl::CMusikSelectionCtrl( wxWindow* parent, wxWindowID id )
	: CMusikListCtrl( parent, id )
{
	Reset( true, true );
}

CMusikSelectionCtrl::~CMusikSelectionCtrl()
{
}

void CMusikSelectionCtrl::RescaleColumns( bool refresh )
{
	SetColumnWidth( 0, GetClientSize().GetWidth() );
	if ( refresh )
		Refresh( false );
}

void CMusikSelectionCtrl::Reset( bool rescale, bool refresh )
{
	ClearAll();
	InsertColumn( 0, wxT( "SelectionBox" ) );
	if ( rescale )
		RescaleColumns( refresh );
}

void CMusikSelectionCtrl::Update( bool refresh )
{
	if ( refresh )
		Refresh( false );
}

void CMusikSelectionCtrl::OnResize( wxSizeEvent& event )
{
	RescaleColumns();
}