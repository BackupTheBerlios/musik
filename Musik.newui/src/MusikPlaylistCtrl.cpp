#include "wx/wxprec.h"
#include "MusikPlaylistCtrl.h"

CMusikPlaylistCtrl::CMusikPlaylistCtrl( wxWindow* parent, wxWindowID id )
	: CMusikListCtrl( parent, id, wxNO_FULL_REPAINT_ON_RESIZE | wxLC_ALIGN_LEFT | wxLC_REPORT | wxLC_VIRTUAL | wxSIMPLE_BORDER & ~wxHSCROLL )
{
	CreateColumns();
}

CMusikPlaylistCtrl::~CMusikPlaylistCtrl()
{

}

void CMusikPlaylistCtrl::CreateColumns()
{

}


