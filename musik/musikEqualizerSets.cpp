// musikEqualizerSets.cpp : implementation file
//

#include "stdafx.h"
#include "musik.h"
#include "musikEqualizerSets.h"
#include "musikEqualizerCtrl.h"
#include ".\musikequalizersets.h"


// CmusikEqualizerSets dialog

IMPLEMENT_DYNAMIC(CmusikEqualizerSets, CDialog)
CmusikEqualizerSets::CmusikEqualizerSets(CWnd* pParent /*=NULL*/)
	: CDialog(CmusikEqualizerSets::IDD, pParent)
{
}

CmusikEqualizerSets::~CmusikEqualizerSets()
{
}

void CmusikEqualizerSets::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CmusikEqualizerSets, CDialog)
	ON_BN_CLICKED(IDC_CLOSE_DLG, OnBnClickedCloseDlg)
END_MESSAGE_MAP()


// CmusikEqualizerSets message handlers

void CmusikEqualizerSets::OnBnClickedCloseDlg()
{
	int WM_CLOSEEQUALIZERSETS = RegisterWindowMessage( "CLOSEEQUALIZERSETS" );

	CmusikEqualizerBar* pBar = (CmusikEqualizerBar*)GetParent();
	pBar->PostMessage( WM_CLOSEEQUALIZERSETS );
}
