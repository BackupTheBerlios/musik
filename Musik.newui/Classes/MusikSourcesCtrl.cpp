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
	if ( GetColumnWidth( 0 ) != 0 )
		SetColumnWidth( 0, 0 );

	SetColumnWidth( 1, GetClientSize().GetWidth() );
	if ( refresh )
		Refresh( false );
}

void CMusikSourcesCtrl::Reset( bool rescale, bool refresh )
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
	InsertColumn( 1, wxT( "Sources" ) );
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