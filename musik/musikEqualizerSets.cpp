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
	ON_BN_CLICKED(IDC_SAVE_ACTIVE_AS_SELECTED, OnBnClickedSaveActiveAsSelected)
	ON_BN_CLICKED(IDC_SAVE_ACTIVE_AS_NEW, OnBnClickedSaveActiveAsNew)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RENAME_SEL, OnBnClickedRenameSel)
	ON_LBN_SELCHANGE(IDC_PRESET_BOX, OnLbnSelchangePresetBox)
	ON_BN_CLICKED(IDC_RESET_DEFAULTS, OnBnClickedResetDefaults)
	ON_BN_CLICKED(IDC_SET_AS_DEFAULT, OnBnClickedSetAsDefault)
	ON_BN_CLICKED(IDC_DELETE_SEL, OnBnClickedDeleteSel)
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

	CStdStringArray items;
	m_Library->GetAllEqualizerPresets( &items, &m_IDs, true );

	m_PresetBox.ResetContent();
	for ( size_t i = 0; i < items.size(); i++ )
		m_PresetBox.AddString( items.at( i ) );

	return TRUE;
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
		CString sSel;
		m_PresetBox.GetText( nSel, sSel );

		CmusikEQSettings settings;
		settings.m_ID = m_IDs.at( nSel );
		settings.m_Name = sSel;

		GetActiveEqualizer( &settings );
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
		{
			m_PresetBox.AddString( name );
			m_IDs.push_back( settings.m_ID );
		}
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
	else if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE )
	{
		OnBnClickedDeleteSel();
		return true;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::OnBnClickedRenameSel()
{
	int nSel = GetIndex();

	if ( nSel > -1 )
	{
		CString rename;
		CmusikNameEntry* pDlg = new CmusikNameEntry( this, &rename );
		if ( pDlg->DoModal() == IDOK && !rename.IsEmpty() )
		{
			CmusikEQSettings settings;
			settings.m_ID = m_IDs.at( nSel );
			settings.m_Name = rename;

			if ( m_Library->UpdateEqualizer( m_IDs.at( nSel ), settings ) == 0 )
			{
				m_PresetBox.DeleteString( nSel );
				m_IDs.erase( m_IDs.begin() + nSel );

				m_PresetBox.InsertString( nSel, rename );
				m_IDs.insert( m_IDs.begin() + nSel, settings.m_ID );
			}
		}	
	}
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::OnLbnSelchangePresetBox()
{
	int nSel = GetIndex();

	if ( nSel != -1 )
	{
		CmusikEQSettings settings;
		m_Library->GetEqualizer( m_IDs.at( nSel ), &settings );

		CmusikEqualizerBar* pBar = (CmusikEqualizerBar*)m_Parent;
		pBar->GetCtrl()->SetBandsFrom( settings );
		pBar->GetCtrl()->OnBandChange( NULL, NULL );
	}
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::OnBnClickedResetDefaults()
{
	CmusikEQSettings settings;

	CmusikEqualizerBar* pBar = (CmusikEqualizerBar*)m_Parent;
	pBar->GetCtrl()->SetBandsFrom( settings );
	pBar->GetCtrl()->OnBandChange( NULL, NULL );
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::OnBnClickedSetAsDefault()
{
	CmusikEQSettings settings;
	GetActiveEqualizer( &settings );

	m_Library->UpdateDefaultEqualizer( settings );
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::GetActiveEqualizer( CmusikEQSettings* settings )
{
	CmusikEqualizerBar* pBar = (CmusikEqualizerBar*)m_Parent;
	pBar->GetCtrl()->BandsToEQSettings( settings );
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::OnBnClickedDeleteSel()
{
	int nSel = GetIndex();
	{
		if ( m_Library->DeleteEqualizer( m_IDs.at( nSel ) ) == 0 )
		{
			m_PresetBox.DeleteString( nSel );
			m_IDs.erase( m_IDs.begin() + nSel );
	
			--nSel;
			if ( nSel == -1 )
				nSel = 0;

			m_PresetBox.SetSel( nSel );
		}
	}
}

///////////////////////////////////////////////////
