#ifndef C_MUSIK_APP_H
#define C_MUSIK_APP_H

#include "wx/wxprec.h"

class CMusikApp : public wxApp
{
public:
	virtual bool OnInit();
};

DECLARE_APP( CMusikApp )

#endif
