#ifndef C_MUSIK_DRAG_GAUGE_CTRL
#define C_MUSIK_DRAG_GAUGE_CTRL

#include "wx/wxprec.h"

class CMusikDragGauge : public wxGauge
{
public:
	CMusikDragGauge( wxWindow* parent, wxWindowID id, long type );
	~CMusikDragGauge();

	virtual void OnLeftUp	( wxMouseEvent& event ){ wxASSERT( 0 ); }
	void OnLeftDown			( wxMouseEvent& event );
	void OnMotion			( wxMouseEvent& event );

	DECLARE_EVENT_TABLE()
private:

	wxWindow* m_Parent;
	float f_Val;
};

#endif
