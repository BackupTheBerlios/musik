//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

//--- globals ---//
#include "../MusikGlobals.h"
#include "../MusikUtils.h"

#include "MusikFXGauge.h"


//-----------------------------------------//
//--- this is the gauge					---//
//-----------------------------------------//
MusikFXGauge::MusikFXGauge( wxFrame* parent, int nChannel, float *nBandID )
	: wxGauge( parent, -1, 100, wxPoint( 0, 0 ), wxSize( wxSystemSettings::GetMetric( wxSYS_HSCROLL_Y ), 100 ), wxGA_SMOOTH | wxGA_VERTICAL | wxCLIP_CHILDREN )
{
	m_Channel = nChannel;
	m_BandID = nBandID;

	m_EvtHandler = new MusikFXHandler( this );
	PushEventHandler( m_EvtHandler );
}

MusikFXGauge::~MusikFXGauge()
{
	PopEventHandler();
	delete m_EvtHandler;
}

//-----------------------------------------//
//--- this is the gauge's event	handler	---//
//-----------------------------------------//

BEGIN_EVENT_TABLE(MusikFXHandler, wxEvtHandler)
	EVT_LEFT_DOWN			(MusikFXHandler::OnLeftDown		) 
	EVT_MOTION				(MusikFXHandler::OnMouseMove	) 
END_EVENT_TABLE()

MusikFXHandler::MusikFXHandler( MusikFXGauge *pParent )
{
	m_Parent = pParent;
}

MusikFXHandler::~MusikFXHandler()
{

}

void MusikFXHandler::OnLeftDown( wxMouseEvent& event )
{
    SetFromMousePos( event );
}

void MusikFXHandler::OnMouseMove( wxMouseEvent& event )
{
	if ( event.LeftIsDown() )
		SetFromMousePos( event );
}

void MusikFXHandler::SetFromMousePos( wxMouseEvent& event )
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