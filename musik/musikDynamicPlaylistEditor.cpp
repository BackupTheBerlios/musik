///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikDynamicPlaylistEditor.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikDynamicPlaylistEditor, CDialog)

///////////////////////////////////////////////////

CmusikDynamicPlaylistEditor::CmusikDynamicPlaylistEditor(CWnd* pParent /*=NULL*/)
	: CDialog( IDD_DYNPLAYLISTEDITOR, pParent )
{
}

///////////////////////////////////////////////////

CmusikDynamicPlaylistEditor::~CmusikDynamicPlaylistEditor()
{
}

///////////////////////////////////////////////////

void CmusikDynamicPlaylistEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikDynamicPlaylistEditor, CDialog)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

