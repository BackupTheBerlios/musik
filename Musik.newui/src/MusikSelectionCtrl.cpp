#include "wx/wxprec.h"
#include "MusikSelectionCtrl.h"

CMusikSelectionCtrl::CMusikSelectionCtrl( wxWindow* parent, CMusikLibrary* library, wxWindowID id )
	: CMusikListCtrl( parent, id, wxNO_FULL_REPAINT_ON_RESIZE | wxLC_ALIGN_LEFT | wxLC_REPORT | wxLC_VIRTUAL | wxSIMPLE_BORDER & ~wxHSCROLL )
{
	m_MusikLibrary = library;

	SetType( MUSIK_SELECTION_ARTISTS, false );	//--- this is where we will set the box type
	Reset( true, true );
}

CMusikSelectionCtrl::~CMusikSelectionCtrl()
{
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

	//---------------------------------------------------------//
	//--- we don't want the user to do a bounding box		---//
	//--- selection in windows, becuase it causes an		---//
	//--- event to get triggered for every item selected.	---//
	//--- bounding box selections are only available in the	---//
	//--- first column (0), so make an empty column and		---//
	//--- hide it.											---//
	//---------------------------------------------------------//
	InsertColumn( 0, wxT( "" ) );
	InsertColumn( 1, m_MusikLibrary->GetSongField( m_Type ) );
	if ( rescale )
		RescaleColumns( refresh );
}

void CMusikSelectionCtrl::SetType( int type, bool reset )
{
	m_Type = type;
	if ( reset )
		Reset();
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

void CMusikSelectionCtrl::OnKeyDown( wxListEvent& event )
{
	if ( event.GetKeyCode() == WXK_F2 )
		StartEditInPlace();

	event.Skip();
}