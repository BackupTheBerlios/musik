#ifndef MUSIKFXGAUGE_H
#define MUSIKFXGAUGE_H

//--- wx ---//
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 

enum EMUSIK_FX_CHANNEL
{
	MUSIK_FX_LEFT = 0,
	MUSIK_FX_RIGHT
};

class CMusikFXHandler;
class MusikFXFrame;

class CMusikFXGauge : public wxGauge
{
public:
	CMusikFXGauge( MusikFXFrame* parent, size_t nChannel, size_t nBandID );
	~CMusikFXGauge();

	void OnEraseBackground ( wxEraseEvent& event );
	void OnLeftDown			( wxMouseEvent& event );
	void OnLeftUp			( wxMouseEvent& event );
	void OnMouseMove		( wxMouseEvent& event );
	void SetFromMousePos	( wxMouseEvent& event );

	void SetEQ();
	void SetPos( size_t m_Pos );
	void Colourize( );

	DECLARE_EVENT_TABLE()

private:
	MusikFXFrame *m_Parent;
	size_t m_Channel;
	size_t m_BandID;

	//--- for the mouse events ---//
	wxPoint m_MousePos;
	wxSize m_WndSize;
	float m_Pos;
	float m_Temp;
	float m_Value;
};

#endif
