#ifndef C_MUSIK_SEEK_CTRL
#define C_MUSIK_SEEK_CTRL

#include "wx/wxprec.h"
#include "MusikDragGaugeCtrl.h"

class CMusikSeekCtrl : public CMusikDragGauge
{
public:
	CMusikSeekCtrl( wxWindow* parent, wxWindowID id );
	~CMusikSeekCtrl();

	virtual void OnLeftUpV();
	virtual void OnMotionV();

};

#endif
