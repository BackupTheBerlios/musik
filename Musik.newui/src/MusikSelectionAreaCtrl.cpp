#include "wx/wxprec.h"
#include "MusikSelectionAreaCtrl.h"

CMusikSelectionAreaCtrl::CMusikSelectionAreaCtrl( wxWindow *parent, CMusikLibrary* library, wxWindowID id )
	: wxPanel( parent, id, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxNO_BORDER /*| wxTRANSPARENT_WINDOW | wxCLIP_CHILDREN*/ )
{
	//-----------------------------------------------------//
	//--- intialize all of the needed variables			---//
	//-----------------------------------------------------//
	m_MusikLibrary = library;
	m_SelectionSizer = NULL;
	for ( size_t i = 0; i < MAX_SELECTION_CTRLS; i++ )
		m_SelectionCtrls[i] = NULL;

	//-----------------------------------------------------//
	//--- create the controls and set the sizer.		---//
	//-----------------------------------------------------//
	CreateSashes();
	CreateControls();

	SetSizer( m_SelectionSizer );
}

CMusikSelectionAreaCtrl::~CMusikSelectionAreaCtrl()
{

}

void CMusikSelectionAreaCtrl::CreateSashes()
{

}

void CMusikSelectionAreaCtrl::CreateControls()
{
	if ( HORIZONTAL )
		m_SelectionSizer = new wxBoxSizer( wxHORIZONTAL );
	else
		m_SelectionSizer = new wxBoxSizer( wxVERTICAL );

	for ( size_t i = 0; i < MAX_SELECTION_CTRLS; i++ )
	{
		m_SelectionCtrls[i] = new CMusikSelectionCtrl( this, m_MusikLibrary, -1 );
		if ( i < ( MAX_SELECTION_CTRLS - 1 ) )
			m_SelectionSizer->Add( m_SelectionCtrls[i], 1, wxRIGHT | wxEXPAND, 4 );
		else
			m_SelectionSizer->Add( m_SelectionCtrls[i], 1, wxRIGHT | wxEXPAND, 0 );
	}

	Layout();
}

void CMusikSelectionAreaCtrl::Delete()
{
	if ( m_SelectionSizer )
	{
		delete m_SelectionSizer;
		m_SelectionSizer = NULL;
	}

	for ( size_t i = 0; i < MAX_SELECTION_CTRLS; i++ )
	{
		if ( m_SelectionCtrls[i] != NULL )
		{
			delete m_SelectionCtrls[i];
			m_SelectionCtrls[i] = NULL;
		}
	}
}

void CMusikSelectionAreaCtrl::Reset()
{
	Delete();
	CreateControls();
}

CMusikSelectionCtrl* CMusikSelectionAreaCtrl::GetControl( size_t nCtrlID )
{
	if ( nCtrlID >= MAX_SELECTION_CTRLS )
		return NULL;
	else
		return m_SelectionCtrls[nCtrlID];
}