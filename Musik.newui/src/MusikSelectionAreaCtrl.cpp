#include "wx/wxprec.h"
#include "MusikSelectionAreaCtrl.h"

CMusikSelectionAreaCtrl::CMusikSelectionAreaCtrl( wxWindow *parent, wxWindowID id )
	: wxPanel( parent, id, wxPoint( -1, -1 ), wxSize( -1, -1 ), wxNO_BORDER | wxTRANSPARENT_WINDOW | wxCLIP_CHILDREN )
{
	//-----------------------------------------------------//
	//--- intialize all of the needed variables			---//
	//-----------------------------------------------------//
	m_SelectionSizer = NULL;
	for ( size_t i = 0; i < MAX_SELECTION_CTRLS; i++ )
		m_SelectionCtrls[i] = NULL;

	//-----------------------------------------------------//
	//--- create the controls and set the sizer.		---//
	//-----------------------------------------------------//
	Create();

	SetSizer( m_SelectionSizer );
}

CMusikSelectionAreaCtrl::~CMusikSelectionAreaCtrl()
{

}

void CMusikSelectionAreaCtrl::Create()
{
	if ( HORIZONTAL )
		m_SelectionSizer = new wxBoxSizer( wxHORIZONTAL );
	else
		m_SelectionSizer = new wxBoxSizer( wxVERTICAL );

	for ( size_t i = 0; i < MAX_SELECTION_CTRLS; i++ )
	{
		m_SelectionCtrls[i] = new CMusikSelectionCtrl( this, -1 );
		if ( i < ( MAX_SELECTION_CTRLS - 1 ) )
			m_SelectionSizer->Add( m_SelectionCtrls[i], 1, wxRIGHT | wxEXPAND, 4 );
		else
			m_SelectionSizer->Add( m_SelectionCtrls[i], 1, wxEXPAND );
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
	Create();
}

CMusikSelectionCtrl* CMusikSelectionAreaCtrl::GetControl( size_t nCtrlID )
{
	if ( nCtrlID >= MAX_SELECTION_CTRLS )
		return NULL;
	else
		return m_SelectionCtrls[nCtrlID];
}