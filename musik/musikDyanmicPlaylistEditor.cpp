///////////////////////////////////////////////////

#include "stdafx.h"

#include "musik.h"
#include "musikDyanmicPlaylistEditor.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikDyanmicPlaylistEditor, CDialog)

///////////////////////////////////////////////////

CmusikDyanmicPlaylistEditor::CmusikDyanmicPlaylistEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CmusikDyanmicPlaylistEditor::IDD, pParent)
{
}

///////////////////////////////////////////////////

CmusikDyanmicPlaylistEditor::~CmusikDyanmicPlaylistEditor()
{
}

///////////////////////////////////////////////////

void CmusikDyanmicPlaylistEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikDyanmicPlaylistEditor, CDialog)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

