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
BEGIN_EVENT_TABLE(CMusikFXGauge, wxEvtHandler)
	EVT_ERASE_BACKGROUND		( CMusikFXGauge::OnEraseBackground ) 
	EVT_LEFT_DOWN				( CMusikFXGauge::OnLeftDown	) 
	EVT_LEFT_UP					( CMusikFXGauge::OnLeftUp		)
	EVT_MOTION					( CMusikFXGauge::OnMouseMove	) 
END_EVENT_TABLE()

CMusikFXGauge::CMusikFXGauge( MusikFXFrame* parent, size_t nChannel, size_t nBandID )
	: wxGauge( parent, -1, 100, wxPoint( 0, 0 ), wxSize( wxSystemSettings::GetMetric( wxSYS_HSCROLL_Y ), 100 ), wxGA_SMOOTH | wxGA_VERTICAL | wxCLIP_CHILDREN )
{
	m_Channel = nChannel;
	m_BandID = nBandID;
	m_Parent = parent;	
}

CMusikFXGauge::~CMusikFXGauge()
{
}

void CMusikFXGauge::OnEraseBackground( wxEraseEvent& event )
{
	Refresh( FALSE );
}

void CMusikFXGauge::SetPos( size_t m_Pos )
{
	SetValue( (int)m_Pos );
	Colourize();
	if ( m_Parent->chkLock->IsChecked() )
	{
		if ( m_Channel == MUSIK_FX_LEFT )
		{
			m_Parent->slRight[m_BandID]->SetValue( m_Pos );
			m_Parent->slRight[m_BandID]->Colourize();
		}
		else
		{
			m_Parent->slLeft[m_BandID]->SetValue( m_Pos );
			m_Parent->slLeft[m_BandID]->Colourize();
		}
	}
}

void CMusikFXGauge::Colourize()
{
	/*
	if ( g_Prefs.nEQColourize != 1 )
		return;
	*/
	int nGreen = (int)( 200.0f * (float)GetValue() / 100.0f );
	SetForegroundColour( wxColour( 200 - nGreen, nGreen, 0 ) );
}

void CMusikFXGauge::SetEQ()
{
	m_Parent->BandsFromSliders();
}

void CMusikFXGauge::OnLeftDown( wxMouseEvent& event )
{
	CaptureMouse();
    SetFromMousePos( event );
}

void CMusikFXGauge::OnMouseMove( wxMouseEvent& event )
{
	if ( event.LeftIsDown() )
		SetFromMousePos( event );
}

void CMusikFXGauge::OnLeftUp( wxMouseEvent& event )
{
	SetEQ();
	ReleaseMouse();
}

void CMusikFXGauge::SetFromMousePos( wxMouseEvent& event )
{
	//--- stuff we'll need for calculation ---//
	m_MousePos	= ScreenToClient( wxGetMousePosition() );
	m_WndSize	= GetSize();	

	//--- set value ---//
	m_Temp = (float)100* ( (float)m_MousePos.y / (float)m_WndSize.GetHeight() );
	if ( m_Temp != m_Pos )
	{
		m_Pos = m_Temp;
		SetPos( (size_t)( 100 - (int)m_Pos ) );
	}	
}
