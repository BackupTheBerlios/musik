///////////////////////////////////////////////////

#include "stdafx.h"

#include "musik.h"
#include "musikEqualizerSets.h"
#include "musikEqualizerCtrl.h"
#include "musikNameEntry.h"

#include "../musikCore/include/musikLibrary.h"
#include "../musikCore/include/musikEQSettings.h"
#include ".\musikequalizersets.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikEqualizerSets, CDialog)

///////////////////////////////////////////////////

CmusikEqualizerSets::CmusikEqualizerSets( CWnd* pParent, CmusikLibrary* library )
	: CDialog(CmusikEqualizerSets::IDD, pParent)
{
	m_Parent = pParent;
	m_Library = library;
}

///////////////////////////////////////////////////

CmusikEqualizerSets::~CmusikEqualizerSets()
{
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRESET_BOX, m_PresetBox);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikEqualizerSets, CDialog)
	ON_BN_CLICKED(IDC_CLOSE_DLG, OnBnClickedCloseDlg)
	ON_BN_CLICKED(IDC_LOAD_ACTIVE_FROM_SELECTED, OnBnClickedLoadActiveFromSelected)
	ON_BN_CLICKED(IDC_SAVE_ACTIVE_AS_SELECTED, OnBnClickedSaveActiveAsSelected)
	ON_BN_CLICKED(IDC_SAVE_ACTIVE_AS_NEW, OnBnClickedSaveActiveAsNew)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikEqualizerSets::OnBnClickedCloseDlg()
{
	OnClose();
}

///////////////////////////////////////////////////

BOOL CmusikEqualizerSets::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_Library->GetAllEqualizerPresets( &m_Items, &m_IDs, true );

	m_PresetBox.ResetContent();
	for ( size_t i = 0; i < m_Items.size(); i++ )
		m_PresetBox.AddString( m_Items.at( i ) );

	return TRUE;
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::OnBnClickedLoadActiveFromSelected()
{
	int nSel = GetIndex();

	if ( nSel != -1 )
	{
		CmusikEQSettings settings;
		m_Library->GetEqualizer( m_IDs.at( nSel ), &settings );

		CmusikEqualizerBar* pBar = (CmusikEqualizerBar*)m_Parent;
		pBar->GetCtrl()->SetBandsFrom( settings );
	}

}

///////////////////////////////////////////////////

int CmusikEqualizerSets::GetIndex()
{
	for ( size_t i = 0; i < m_IDs.size(); i++ )
	{
		if ( m_PresetBox.GetSel( i ) )
			return i;
	}

	return -1;
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::OnBnClickedSaveActiveAsSelected()
{
	int nSel = GetIndex();

	if ( nSel != -1 )
	{
		CmusikEQSettings settings;

		CmusikEqualizerBar* pBar = (CmusikEqualizerBar*)m_Parent;
		pBar->GetCtrl()->BandsToEQSettings( &settings );

		m_Library->UpdateEqualizer( m_IDs.at( nSel ), settings );
	}
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::OnBnClickedSaveActiveAsNew()
{
	CmusikEQSettings settings;
	CmusikEqualizerBar* pBar = (CmusikEqualizerBar*)m_Parent;
	pBar->GetCtrl()->BandsToEQSettings( &settings );

	CString name;
	CmusikNameEntry* pDlg = new CmusikNameEntry( this, &name );
	if ( pDlg->DoModal() == IDOK && !name.IsEmpty() )
	{
		if ( m_Library->CreateEqualizer( settings, (CStdString)name, true ) == 0 )
			m_PresetBox.AddString( name );
	}
	delete pDlg;
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::OnClose()
{
	int WM_CLOSEEQUALIZERPRESETS = RegisterWindowMessage( "CLOSEEQUALIZERPRESETS" );
	m_Parent->PostMessage( WM_CLOSEEQUALIZERPRESETS );
}

///////////////////////////////////////////////////

BOOL CmusikEqualizerSets::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE )
	{
		OnClose();
		return true;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////
