#include "wx/wxprec.h"
#include "MusikDragGaugeCtrl.h"

BEGIN_EVENT_TABLE( CMusikDragGaugeCtrl, wxGauge )
	EVT_LEFT_DOWN			( CMusikDragGaugeCtrl::OnLeftDown		) 
	EVT_LEFT_UP				( CMusikDragGaugeCtrl::OnLeftUp			) 
	EVT_MOTION				( CMusikDragGaugeCtrl::OnMotion			) 
	EVT_SIZE				( CMusikDragGaugeCtrl::OnSize			)
END_EVENT_TABLE()

CMusikDragGaugeCtrl::CMusikDragGaugeCtrl( wxWindow* parent, wxWindowID id, wxSize size, long style )
	: wxGauge( parent, id, 100, wxDefaultPosition, size, style | wxGA_SMOOTH )
{
	m_Parent	= parent;
	m_Style		= style;
	m_Dragging	= false;
	m_LastPos	= -1;
	m_WndSize	= GetSize();
}

CMusikDragGaugeCtrl::~CMusikDragGaugeCtrl()
{
}

void CMusikDragGaugeCtrl::OnLeftDown( wxMouseEvent& event )
{
	if ( IsEnabled() )
	{
		if ( event.LeftDown() )
		{
			m_Dragging = true;
			CaptureMouse();
			SetPosFromMouse();
		}
	}

	event.Skip();
}

void CMusikDragGaugeCtrl::OnLeftUp( wxMouseEvent& event )
{
	if ( m_Dragging )
	{
		ReleaseMouse();
		m_Dragging = false;

		OnLeftUpV();
	}

	event.Skip();
}

void CMusikDragGaugeCtrl::OnMotion( wxMouseEvent& event )
{
	if ( m_Dragging )
	{
		if ( event.LeftIsDown() )
		{
			SetPosFromMouse();
			OnMotionV();
		}
	}
}

void CMusikDragGaugeCtrl::SetPosFromMouse()
{
	m_MousePos = ScreenToClient( wxGetMousePosition() );

	float nSize;
	float nPos;

	//-----------------------------------------//
	//--- if the gauge is horizontal...		---//
	//-----------------------------------------//
	if ( m_Style == wxGA_HORIZONTAL )
	{
		nPos	= (float)m_MousePos.x;
		nSize	= (float)m_WndSize.GetWidth();
	}

	//-----------------------------------------//
	//--- if the gauge is vertical...		---//
	//-----------------------------------------//
	else
	{
		nPos	= (float)m_MousePos.y;
		nSize	= (float)m_WndSize.GetHeight();
	}

	m_Val = 100.0f * ( nPos / nSize );

	if		( m_Val < 0.0f )	m_Val = 0.0f;
	else if ( m_Val > 100.0f )	m_Val = 100.0f;

	if ( (int)m_Val != m_LastPos )
	{
		SetValue( (int)m_Val );
		m_LastPos = (int)m_Val;
	}	
}

void CMusikDragGaugeCtrl::OnSize( wxSizeEvent& event )
{
	m_WndSize = GetSize();
}
