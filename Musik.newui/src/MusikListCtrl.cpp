#include "wx/wxprec.h"
#include "MusikListCtrl.h"

BEGIN_EVENT_TABLE( CMusikEditInPlaceCtrl, wxTextCtrl )
EVT_CHAR						( CMusikEditInPlaceCtrl::OnChar					)
END_EVENT_TABLE()

CMusikEditInPlaceCtrl::CMusikEditInPlaceCtrl( wxWindow* parent, wxWindowID id )
	: wxTextCtrl( parent, id, wxT( "" ), wxPoint( 0, 0 ), wxSize( -1, -1 ), wxSIMPLE_BORDER | wxTE_PROCESS_ENTER )
{
}

CMusikEditInPlaceCtrl::~CMusikEditInPlaceCtrl()
{
}

void CMusikEditInPlaceCtrl::OnChar( wxKeyEvent& event )
{
	int nKey = event.GetKeyCode();

	if ( nKey == WXK_ESCAPE )
		wxMessageBox( wxT( "Canceled" ) );

	else if ( nKey == WXK_RETURN )
		wxMessageBox( wxT( "Committed" ) );

	event.Skip();
}

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

CMusikListCtrl::CMusikListCtrl( wxWindow *parent, wxWindowID id, long flags )
	: wxListCtrl( parent, id, wxPoint( -1, -1 ), wxSize( -1, -1 ), flags )
{
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ) );
	
	m_EditInPlace = new CMusikEditInPlaceCtrl( this, -1 );
	m_EditInPlace->Show( false );
	m_Parent = parent;
}

CMusikListCtrl::~CMusikListCtrl()
{
}

void CMusikListCtrl::StartEditInPlace()
{
	m_EditInPlace->SetSize( GetClientSize().GetWidth(), -1 );
	m_EditInPlace->Move( 0, ( GetClientSize().GetHeight() ) - ( m_EditInPlace->GetSize().GetHeight() ) );
	m_EditInPlace->Show( true );
	m_EditInPlace->SetFocus();
}

wxString CMusikListCtrl::EndEditInPlace()
{
	wxString sReturn = m_EditInPlace->GetValue();
	m_EditInPlace->Show( false );
	m_EditInPlace->Clear();
	return sReturn;
}