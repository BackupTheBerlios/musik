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
//   CmusikNameEntry
//
// Filename(s): 
//
//   musikNameEntry.h, musikNameEntry.cpp
//
// Information:
//
//   Dialog used for prompting user for a string.
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "musik.h"
#include "musikNameEntry.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikNameEntry, CDialog)

///////////////////////////////////////////////////

CmusikNameEntry::CmusikNameEntry( CWnd* pParent, CString* str )
	: CDialog(CmusikNameEntry::IDD, pParent)
{
	m_Str = str;
}

///////////////////////////////////////////////////

CmusikNameEntry::~CmusikNameEntry()
{
}

///////////////////////////////////////////////////

void CmusikNameEntry::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT, m_Edit);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikNameEntry, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikNameEntry::OnBnClickedOk()
{
	if ( m_Str )
		m_Edit.GetWindowText( *m_Str );

	OnOK();
}

///////////////////////////////////////////////////

BOOL CmusikNameEntry::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Edit.SetFocus();

	return TRUE;
}

///////////////////////////////////////////////////

void CmusikNameEntry::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if ( bShow )
		m_Edit.SetFocus();
}

///////////////////////////////////////////////////


