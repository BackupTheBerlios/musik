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

class CMusikFXGauge : public wxGauge
{
public:
	CMusikFXGauge( wxFrame* parent, int nChannel, float *nBandID );
	~CMusikFXGauge();

private:
	CMusikFXHandler *m_EvtHandler;
	int m_Channel;
	float *m_BandID;
};

class CMusikFXHandler : public wxEvtHandler
{
public:
	CMusikFXHandler( CMusikFXGauge *parent );
	~CMusikFXHandler();
	
	void OnLeftDown			( wxMouseEvent& event );
	void OnMouseMove		( wxMouseEvent& event );
	void SetFromMousePos	( wxMouseEvent& event );

	DECLARE_EVENT_TABLE()

private:
	wxPoint m_MousePos;
	wxSize m_WndSize;
	CMusikFXGauge *m_Parent;
	float m_Pos;
	float m_Temp;
	float m_Value;
};


#endif