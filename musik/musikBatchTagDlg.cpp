///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2003
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey@bak.rr.com
//   Dustin Carter  : Developer, pain in the ass
//   Simon Windmill : Developer, the most helpful person thats immediately available ;)
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Class(es): 
//
//   CmusikBatchTagDlg
//
// Filename(s): 
//
//   musikBatchTagDlg.h, musikBatchTagDlg.cpp
//
// Information:
//
//   Dialog used for batch tag editing
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikBatchTagDlg.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikBatchTagDlg, CDialog)

///////////////////////////////////////////////////

CmusikBatchTagDlg::CmusikBatchTagDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CmusikBatchTagDlg::IDD, pParent)
{
}

///////////////////////////////////////////////////

CmusikBatchTagDlg::~CmusikBatchTagDlg()
{
}

///////////////////////////////////////////////////

void CmusikBatchTagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikBatchTagDlg, CDialog)
END_MESSAGE_MAP()

///////////////////////////////////////////////////


