#include "wx/wxprec.h"
#include "MusikSeekCtrl.h"

CMusikSeekCtrl::CMusikSeekCtrl( wxWindow* parent, wxWindowID id )
	: CMusikDragGaugeCtrl( parent, id, wxSize( wxSize( 200, wxSystemSettings::GetMetric( wxSYS_HSCROLL_Y ) ) ), wxGA_HORIZONTAL )
{
}

CMusikSeekCtrl::~CMusikSeekCtrl()
{
}

void CMusikSeekCtrl::OnLeftUpV()
{
	wxMessageBox( wxT( "The song would jump to this position" ) );
}

void CMusikSeekCtrl::OnMotionV()
{

}