#include "wx/wxprec.h"
#include "MusikApp.h"

IMPLEMENT_APP( CMusikApp )

//--- related frames ---//
#include "MusikFrame.h"

bool CMusikApp::OnInit()
{
	CheckPaths();

    CMusikFrame *pMain = new CMusikFrame();
	pMain->Show(TRUE);
	SetTopWindow( pMain );

	return TRUE;
}

void CMusikApp::CheckPaths()
{
	if ( !wxDirExists( MUSIK_HOME_DIR ) )
		wxMkDir( MUSIK_HOME_DIR );
}