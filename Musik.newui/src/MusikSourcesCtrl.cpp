#include "wx/wxprec.h"
#include "MusikSourcesCtrl.h"

CMusikSourcesCtrl::CMusikSourcesCtrl( wxWindow* parent, wxWindowID id )
	: CMusikListCtrl( parent, id, wxNO_FULL_REPAINT_ON_RESIZE | wxLC_ALIGN_LEFT | wxLC_REPORT | wxLC_VIRTUAL | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL | wxSIMPLE_BORDER & ~wxHSCROLL )
{
	Reset( true, true );
}

CMusikSourcesCtrl::~CMusikSourcesCtrl()
{
}

void CMusikSourcesCtrl::RescaleColumns( bool refresh )
{
	Freeze();

	SetColumnWidth( 0, GetClientSize().GetWidth() );
	if ( refresh )
		Refresh( false );

	Thaw();
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