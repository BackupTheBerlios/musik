#ifndef C_MUSIK_APP_H
#define C_MUSIK_APP_H

#include "wx/wxprec.h"

#define MUSIK_HOME_DIR wxFileName::GetHomeDir() + wxFileName::GetPathSeparator() + wxT( ".Musik" ) + wxFileName::GetPathSeparator()
#define MUSIK_DB_FILENAME wxFileName::GetHomeDir() + wxFileName::GetPathSeparator() + wxT( ".Musik" ) + wxFileName::GetPathSeparator() + wxT( "musiklib.db" )

class CMusikApp : public wxApp
{
public:
	virtual bool OnInit();

private:
	void CheckPaths();
};

DECLARE_APP( CMusikApp )

#endif
