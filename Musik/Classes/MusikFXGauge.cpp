//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

#include "MusikFXGauge.h"

//-----------------------------------------//
//--- this is the gauge					---//
//-----------------------------------------//
CMusikFXGauge::CMusikFXGauge( wxFrame* parent, int nChannel, float *nBandID )
	: wxGauge( parent, -1, 100, wxPoint( 0, 0 ), wxSize( wxSystemSettings::GetMetric( wxSYS_HSCROLL_Y ), 100 ), wxGA_SMOOTH | wxGA_VERTICAL | wxCLIP_CHILDREN )
{
	m_Channel = nChannel;
	m_BandID = nBandID;

	m_EvtHandler = new CMusikFXHandler( this );
	PushEventHandler( m_EvtHandler );
}

CMusikFXGauge::~CMusikFXGauge()
{
	PopEventHandler();
	delete m_EvtHandler;
}

//-----------------------------------------//
//--- this is the gauge's event	handler	---//
//-----------------------------------------//

BEGIN_EVENT_TABLE(CMusikFXHandler, wxEvtHandler)
	EVT_LEFT_DOWN			(CMusikFXHandler::OnLeftDown	) 
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
		m_Parent->SetValue( 100 - (int)m_Pos );
	}

	/*
		SET THE EQ BAND
	*/
}