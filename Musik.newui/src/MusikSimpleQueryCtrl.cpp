#include "wx/wxprec.h"
#include "MusikSimpleQueryCtrl.h"

CMusikSimpleQueryCtrl::CMusikSimpleQueryCtrl( wxWindow* parent, wxWindowID id )
	: wxTextCtrl( parent, id, wxT( "" ), wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER )
{
	m_Parent = parent;
}

CMusikSimpleQueryCtrl::~CMusikSimpleQueryCtrl()
{
}