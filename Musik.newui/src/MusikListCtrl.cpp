#include "wx/wxprec.h"
#include "MusikListCtrl.h"

BEGIN_EVENT_TABLE( CMusikEditInPlaceCtrl, wxTextCtrl )
	EVT_CHAR						( CMusikEditInPlaceCtrl::OnChar					)
	EVT_KILL_FOCUS					( CMusikEditInPlaceCtrl::OnKillFocus			)
END_EVENT_TABLE()

CMusikEditInPlaceCtrl::CMusikEditInPlaceCtrl( wxWindow* parent, wxWindowID id )
	: wxTextCtrl( parent, id, wxT( "" ), wxPoint( 0, 0 ), wxSize( -1, -1 ), wxSIMPLE_BORDER | wxTE_PROCESS_ENTER )
{
	m_Parent = parent;
}

CMusikEditInPlaceCtrl::~CMusikEditInPlaceCtrl()
{
}

void CMusikEditInPlaceCtrl::OnChar( wxKeyEvent& event )
{
	int nKey = event.GetKeyCode();

	if ( nKey == WXK_ESCAPE )
	{
		Hide();
	}

	else if ( nKey == WXK_RETURN )
	{
		m_LastValue = GetValue();
		Hide();
		wxCommandEvent commit_done( wxEVT_COMMAND_MENU_SELECTED, MUSIK_EDIT_IN_PLACE_COMMIT );
		wxPostEvent( m_Parent, commit_done );
	}

	event.Skip();
}

wxString CMusikEditInPlaceCtrl::GetStr()
{
	return m_LastValue;
}

void CMusikEditInPlaceCtrl::OnKillFocus( wxFocusEvent& event )
{
	Hide();
}

void CMusikEditInPlaceCtrl::Hide()
{
	Show( false );
	Enable( false );
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
	EVT_MENU						( MUSIK_EDIT_IN_PLACE_COMMIT, CMusikListCtrl::EndEditInPlace )
END_EVENT_TABLE()

CMusikListCtrl::CMusikListCtrl( wxWindow *parent, wxWindowID id, long flags )
	: wxListCtrl( parent, id, wxPoint( -1, -1 ), wxSize( -1, -1 ), flags )
{
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ) );
	
	m_EditInPlace = new CMusikEditInPlaceCtrl( this, -1 );
	GetEditInPlaceCtrl()->Enable( false );
	GetEditInPlaceCtrl()->Show( false );
	m_Parent = parent;
}

CMusikListCtrl::~CMusikListCtrl()
{
}

void CMusikListCtrl::StartEditInPlace()
{
	GetEditInPlaceCtrl()->SetSize( GetClientSize().GetWidth(), -1 );
	GetEditInPlaceCtrl()->Move( 0, ( GetClientSize().GetHeight() ) - ( GetEditInPlaceCtrl()->GetSize().GetHeight() ) );
	GetEditInPlaceCtrl()->Enable( true );
	GetEditInPlaceCtrl()->Show( true );
	GetEditInPlaceCtrl()->SetFocus();
}

void CMusikListCtrl::EndEditInPlace( wxCommandEvent& event )
{
	wxMessageBox( GetEditInPlaceCtrl()->GetStr() );
}