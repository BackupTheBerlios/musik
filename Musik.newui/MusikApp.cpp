#include "wx/wxprec.h"
#include "MusikApp.h"

IMPLEMENT_APP( MusikApp )

//--- related frames ---//
#include "Frames/MusikFrame.h"

bool MusikApp::OnInit()
{
    MusikFrame *pMain = new MusikFrame();
	pMain->Show(TRUE);
	SetTopWindow( pMain );

	return TRUE;
}