#ifndef C_MUSIK_APP_H
#define C_MUSIK_APP_H

#include "wx/wxprec.h"

class MusikApp : public wxApp
{
public:
	virtual bool OnInit();
};

DECLARE_APP(MusikApp)

#endif
