///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikDirSync.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikDirSync, CDialog)

///////////////////////////////////////////////////

CmusikDirSync::CmusikDirSync(CWnd* pParent /*=NULL*/)
	: CDialog(CmusikDirSync::IDD, pParent)
{
}

///////////////////////////////////////////////////

CmusikDirSync::~CmusikDirSync()
{
}

///////////////////////////////////////////////////

void CmusikDirSync::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikDirSync, CDialog)
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
	ON_BN_CLICKED(IDC_REMOVE, OnBnClickedRemove)
	ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikDirSync::OnBnClickedClose()
{
	int WM_CLOSEDIRSYNC	= RegisterWindowMessage( "CLOSEDIRSYNC" );
	GetParent()->PostMessage( WM_CLOSEDIRSYNC, NULL, NULL );
}

///////////////////////////////////////////////////

void CmusikDirSync::OnBnClickedRemove()
{
	// TODO: Add your control notification handler code here
}

///////////////////////////////////////////////////

void CmusikDirSync::OnBnClickedAdd()
{
	// TODO: Add your control notification handler code here
}

///////////////////////////////////////////////////

