///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikEqualizerSets.h"
#include "musikEqualizerCtrl.h"
#include ".\musikequalizersets.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikEqualizerSets, CDialog)

///////////////////////////////////////////////////

CmusikEqualizerSets::CmusikEqualizerSets(CWnd* pParent /*=NULL*/)
	: CDialog(CmusikEqualizerSets::IDD, pParent)
{
	m_Parent = pParent;
}

///////////////////////////////////////////////////

CmusikEqualizerSets::~CmusikEqualizerSets()
{
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikEqualizerSets, CDialog)
	ON_BN_CLICKED(IDC_CLOSE_DLG, OnBnClickedCloseDlg)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikEqualizerSets::OnBnClickedCloseDlg()
{
	int WM_CLOSEEQUALIZERPRESETS = RegisterWindowMessage( "CLOSEEQUALIZERPRESETS" );
	m_Parent->PostMessage( WM_CLOSEEQUALIZERPRESETS );
}

///////////////////////////////////////////////////
