#include "wx/wxprec.h"
#include "MusikDragGauge.h"

BEGIN_EVENT_TABLE( CMusikDragGauge, wxGauge )
	EVT_LEFT_DOWN			( CMusikDragGauge::OnLeftDown		) 
	EVT_LEFT_UP				( CMusikDragGauge::OnLeftUp			) 
	EVT_MOTION				( CMusikDragGauge::OnMotion			) 
END_EVENT_TABLE()

CMusikDragGauge::CMusikDragGauge( wxWindow* parent, wxWindowID id, long style )
	: wxGauge( parent, id, 100, wxDefaultPosition, wxDefaultSize, style )
{
	
}

CMusikDragGauge::~CMusikDragGauge()
{
}

void CMusikDragGauge::OnLeftDown( wxMouseEvent& event )
{
}

void CMusikDragGauge::OnMotion( wxMouseEvent& event )
{
}