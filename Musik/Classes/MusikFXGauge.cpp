//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

#include "MusikFXGauge.h"
#include "../Frames/MusikFXFrame.h"

//-----------------------------------------//
//--- this is the gauge					---//
//-----------------------------------------//
CMusikFXGauge::CMusikFXGauge( MusikFXFrame* parent, size_t nChannel, size_t nBandID )
	: wxGauge( parent, -1, 100, wxPoint( 0, 0 ), wxSize( wxSystemSettings::GetMetric( wxSYS_HSCROLL_Y ), 100 ), wxGA_SMOOTH | wxGA_VERTICAL | wxCLIP_CHILDREN )
{
	m_Channel = nChannel;
	m_BandID = nBandID;
	m_Parent = parent;	

	m_EvtHandler = new CMusikFXHandler( this );
	PushEventHandler( m_EvtHandler );
}

CMusikFXGauge::~CMusikFXGauge()
{
	PopEventHandler();
	delete m_EvtHandler;
}

void CMusikFXGauge::SetPos( size_t m_Pos )
{
	SetValue( (int)m_Pos );
	if ( m_Parent->chkLock->IsChecked() )
	{
		if ( m_Channel == MUSIK_FX_LEFT )
			m_Parent->slRight[m_BandID]->SetValue( m_Pos );
		else
			m_Parent->slLeft[m_BandID]->SetValue( m_Pos );
	}
}

void CMusikFXGauge::SetEQ()
{
	m_Parent->BandsFromSliders();
}

//-----------------------------------------//
//--- this is the gauge's event	handler	---//
//-----------------------------------------//

BEGIN_EVENT_TABLE(CMusikFXHandler, wxEvtHandler)
	EVT_LEFT_DOWN			(CMusikFXHandler::OnLeftDown	) 
	EVT_LEFT_UP				(CMusikFXHandler::OnLeftUp		)
	EVT_MOTION				(CMusikFXHandler::OnMouseMove	) 
END_EVENT_TABLE()

CMusikFXHandler::CMusikFXHandler( CMusikFXGauge *pParent )
{
	m_Parent = pParent;
}

CMusikFXHandler::~CMusikFXHandler()
{

}

void CMusikFXHandler::OnLeftDown( wxMouseEvent& event )
{
    SetFromMousePos( event );
}

void CMusikFXHandler::OnMouseMove( wxMouseEvent& event )
{
	if ( event.LeftIsDown() )
		SetFromMousePos( event );
}

void CMusikFXHandler::OnLeftUp( wxMouseEvent& event )
{
	//-----------------------------------------------------//
	//--- sheer luck. this will only get called once	---//
	//--- becuase the event will only get caught from	---//
	//--- the current gauge window.						---//
	//-----------------------------------------------------//
	m_Parent->SetEQ();
}

void CMusikFXHandler::SetFromMousePos( wxMouseEvent& event )
{
	//--- stuff we'll need for calculation ---//
	m_MousePos	= m_Parent->ScreenToClient( wxGetMousePosition() );
	m_WndSize	= m_Parent->GetSize();	

	//--- set value ---//
	m_Temp = (float)100* ( (float)m_MousePos.y / (float)m_WndSize.GetHeight() );
	if ( m_Temp != m_Pos )
	{
		m_Pos = m_Temp;
		m_Parent->SetPos( (size_t)( 100 - (int)m_Pos ) );
	}	
}
