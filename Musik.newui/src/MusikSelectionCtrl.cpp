#include "wx/wxprec.h"
#include "MusikSelectionCtrl.h"

CMusikSelectionCtrl::CMusikSelectionCtrl( wxWindow* parent, wxWindowID id )
	: CMusikListCtrl( parent, id, wxNO_FULL_REPAINT_ON_RESIZE | wxLC_ALIGN_LEFT | wxLC_REPORT | wxLC_VIRTUAL | wxSIMPLE_BORDER & ~wxHSCROLL )
{
	Reset( true, true );
}

CMusikSelectionCtrl::~CMusikSelectionCtrl()
{
}

void CMusikSelectionCtrl::RescaleColumns( bool refresh )
{
	if ( GetColumnWidth( 0 ) != 0 )
		SetColumnWidth( 0, 0 );

	SetColumnWidth( 1, GetClientSize().GetWidth() );
	if ( refresh )
		Refresh( false );
}

void CMusikSelectionCtrl::Reset( bool rescale, bool refresh )
{
	ClearAll();

	//---------------------------------------------------------//
	//--- we don't want the user to do a bounding box		---//
	//--- selection in windows, becuase it causes an		---//
	//--- event to get triggered for every item selected.	---//
	//--- bounding box selections are only available in the	---//
	//--- first column (0), so make an empty column and		---//
	//--- hide it.											---//
	//---------------------------------------------------------//
	InsertColumn( 0, wxT( "" ) );
	InsertColumn( 1, wxT( "SelectionBox" ) );
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

void CMusikSelectionCtrl::OnColBeginDrag( wxListEvent& event )
{
	event.Veto();
}