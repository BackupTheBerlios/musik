#include "wx/wxprec.h"
#include "MusikListCtrl.h"

BEGIN_EVENT_TABLE( CMusikListCtrl, wxListCtrl )
	EVT_LIST_ITEM_ACTIVATED			( -1,	CMusikListCtrl::OnItemActivate		)
	EVT_LIST_ITEM_SELECTED			( -1,	CMusikListCtrl::OnItemSelect		)
	EVT_LIST_BEGIN_DRAG				( -1,	CMusikListCtrl::OnItemBeginDrag		)
	EVT_LIST_BEGIN_LABEL_EDIT		( -1,	CMusikListCtrl::OnItemBeginEdit		)
	EVT_LIST_END_LABEL_EDIT			( -1,	CMusikListCtrl::OnItemEndEdit		)
	EVT_LIST_COL_BEGIN_DRAG			( -1,	CMusikListCtrl::OnColBeginDrag		)
	EVT_LIST_COL_CLICK				( -1,	CMusikListCtrl::OnColClick			)
	EVT_LIST_KEY_DOWN				( -1,	CMusikListCtrl::OnKeyDown			)
	EVT_CONTEXT_MENU				(		CMusikListCtrl::OnContextMenu		)
	EVT_SIZE						(		CMusikListCtrl::OnResize			)
END_EVENT_TABLE()

CMusikListCtrl::CMusikListCtrl( wxWindow *parent, wxWindowID id )
	: wxListCtrl( parent, id, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxNO_FULL_REPAINT_ON_RESIZE | wxLC_ALIGN_LEFT | wxLC_REPORT | wxLC_VIRTUAL | wxLC_EDIT_LABELS | wxLC_SINGLE_SEL | wxSIMPLE_BORDER & ~wxHSCROLL )
{
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ) );
}

CMusikListCtrl::~CMusikListCtrl()
{
}