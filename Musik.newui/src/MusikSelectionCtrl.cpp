#include "wx/wxprec.h"
#include "MusikSelectionCtrl.h"

CMusikSelectionCtrl::CMusikSelectionCtrl( wxWindow* parent, CMusikLibrary* library, wxWindowID id )
	: CMusikListCtrl( parent, id, wxNO_FULL_REPAINT_ON_RESIZE | wxLC_ALIGN_LEFT | wxLC_REPORT | wxLC_VIRTUAL | wxSIMPLE_BORDER & ~wxHSCROLL )
{
	m_MusikLibrary = library;

	ResetStyles();
	SetType( MUSIK_SELECTION_ARTISTS, false );	//--- this is where we will set the box type
	Reset( true, true );
}

CMusikSelectionCtrl::~CMusikSelectionCtrl()
{
}


wxString CMusikSelectionCtrl::OnGetItemText( long item, long column ) const
{
	switch ( column )
	{
	case 0:
		return wxT( "" );
		break;
	case 1:
		return m_Items.Item( item );
		break;
	}

	return wxT( "" );
}

wxListItemAttr* CMusikSelectionCtrl::OnGetItemAttr( long item ) const
{
	return ( wxListItemAttr* )&m_LightAttr;
}


void CMusikSelectionCtrl::Update( bool refresh )
{
	Freeze();

	SetItemCount( m_Items.GetCount() );
	
	if ( refresh )
		Refresh( false );

	Thaw();
}

void CMusikSelectionCtrl::RescaleColumns( bool refresh )
{
	Freeze();
	if ( GetColumnWidth( 0 ) != 0 )
		SetColumnWidth( 0, 0 );
	SetColumnWidth( 1, GetClientSize().GetWidth() );
	Thaw();
	if ( refresh )
		Refresh( false );
}

void CMusikSelectionCtrl::Reset( bool rescale, bool refresh )
{
	ClearAll();

	InsertColumn( 0, wxT( "" ) );
	InsertColumn( 1, m_MusikLibrary->GetSongField( m_Type ) );
	ResetContents( false );

	if ( rescale )
		RescaleColumns( refresh );
}

void CMusikSelectionCtrl::ResetContents( bool refresh )
{
	m_MusikLibrary->GetAllDistinct( m_Type, m_Items );
	Update( false );
}

void CMusikSelectionCtrl::ResetStyles()
{
	m_LightAttr = wxListItemAttr( *wxBLACK, wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ), wxNullFont );
}

void CMusikSelectionCtrl::SetType( int type, bool reset )
{
	m_Type = type;
	if ( reset )
		Reset();
}

void CMusikSelectionCtrl::OnResize( wxSizeEvent& event )
{
	RescaleColumns();
}

void CMusikSelectionCtrl::OnColBeginDrag( wxListEvent& event )
{
	event.Veto();
}

void CMusikSelectionCtrl::OnKeyDown( wxListEvent& event )
{
	if ( event.GetKeyCode() == WXK_F2 )
		StartEditInPlace();

	event.Skip();
}