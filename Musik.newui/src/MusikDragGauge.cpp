#include "wx/wxprec.h"
#include "MusikDragGauge.h"

BEGIN_EVENT_TABLE( CMusikDragGauge, wxGauge )
	EVT_LEFT_DOWN			( CMusikDragGauge::OnLeftDown		) 
	EVT_LEFT_UP				( CMusikDragGauge::OnLeftUp			) 
	EVT_MOTION				( CMusikDragGauge::OnMotion			) 
	EVT_SIZE				( CMusikDragGauge::OnSize			)
END_EVENT_TABLE()

CMusikDragGauge::CMusikDragGauge( wxWindow* parent, wxWindowID id, long style )
	: wxGauge( parent, id, 100, wxDefaultPosition, wxDefaultSize, style | wxGA_SMOOTH )
{
	m_Parent	= parent;
	m_Style		= style;
	m_Dragging	= false;
	m_LastPos	= -1;
	m_WndSize	= GetSize();
}

CMusikDragGauge::~CMusikDragGauge()
{
}

void CMusikDragGauge::OnLeftDown( wxMouseEvent& event )
{
	if ( IsEnabled() )
	{
		if ( event.LeftDown() )
		{
			m_Dragging = true;
			CaptureMouse();
		}
	}

	event.Skip();
}

void CMusikDragGauge::OnLeftUp( wxMouseEvent& event )
{
	if ( m_Dragging )
	{
		ReleaseMouse();
		m_Dragging = false;

		OnLeftUpV();
	}

	event.Skip();
}

void CMusikDragGauge::OnMotion( wxMouseEvent& event )
{
	if ( m_Dragging )
	{
		if ( event.LeftIsDown() )
		{
			m_MousePos = ScreenToClient( wxGetMousePosition() );

			float nSize;
			float nPos;

			//-----------------------------------------//
			//--- if the gauge is horizontal...		---//
			//-----------------------------------------//
			if ( m_Style == wxGA_HORIZONTAL )
			{
				nSize	= (float)m_MousePos.x;
				nPos	= (float)m_WndSize.GetWidth();
			}

			//-----------------------------------------//
			//--- if the gauge is vertical...		---//
			//-----------------------------------------//
			else
			{
				nSize	= (float)m_MousePos.y;
				nPos	= (float)m_WndSize.GetHeight();
			}

			m_Val = 100.0f * ( nPos / nSize );

			if		( m_Val < 0.0f )	m_Val = 0.0f;
			else if ( m_Val > 100.0f )	m_Val = 100.0f;

			if ( (int)m_Val != m_LastPos )
			{
				SetValue( (int)m_Val );
				m_LastPos = (int)m_Val;
			}

			OnMotionV();
		}
	}
}

void CMusikDragGauge::OnSize( wxSizeEvent& event )
{
	m_WndSize = event.GetSize();
}
