#ifndef C_MUSIK_DRAG_GAUGE_CTRL
#define C_MUSIK_DRAG_GAUGE_CTRL

#include "wx/wxprec.h"

class CMusikDragGaugeCtrl : public wxGauge
{
public:
	CMusikDragGaugeCtrl( wxWindow* parent, wxWindowID id, wxSize size, long type );
	~CMusikDragGaugeCtrl();

	void OnLeftUp			( wxMouseEvent& event );
	void OnLeftDown			( wxMouseEvent& event );
	void OnMotion			( wxMouseEvent& event );
	void OnSize				( wxSizeEvent& event );

	//---------------------------------------------------------//
	//--- these two functions will be called during the		---//
	//--- process... both on left up and motion. they are	---//
	//--- here so inherited classes can override them to do	---//
	//--- custom stuff.										---//
	//---------------------------------------------------------//
	virtual void OnLeftUpV	(){}		
	virtual void OnMotionV	(){}

	DECLARE_EVENT_TABLE()
private:
	
	wxWindow*	m_Parent;
	float		m_Val;
	long		m_Style;
	bool		m_Dragging;
	int			m_LastPos;
	wxSize		m_WndSize;
	wxPoint		m_MousePos;
};

#endif
