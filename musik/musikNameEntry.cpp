///////////////////////////////////////////////////

#include "stdafx.h"

#include "musik.h"
#include "musikNameEntry.h"
#include ".\musiknameentry.h"

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


