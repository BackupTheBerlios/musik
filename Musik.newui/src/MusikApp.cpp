#include "wx/wxprec.h"
#include "MusikApp.h"

IMPLEMENT_APP( CMusikApp )

//--- related frames ---//
#include "MusikFrame.h"

bool CMusikApp::OnInit()
{
    CMusikFrame *pMain = new CMusikFrame();
	pMain->Show(TRUE);
	SetTopWindow( pMain );

	return TRUE;
}