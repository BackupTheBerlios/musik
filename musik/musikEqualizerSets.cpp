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
//   CmusikEqualizerSets
//
// Filename(s): 
//
//   musikEqualizerSets.h, musikEqualizerSets.cpp
//
// Information:
//
//   Dialog used for adding / removing / managing
//   equalizer presets. It front ends the CmusikLibrary.
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "musik.h"
#include "musikEqualizerSets.h"
#include "musikEqualizerCtrl.h"
#include "musikNameEntry.h"

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
	ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_RENAME_SEL, OnBnClickedRenameSel)
	ON_LBN_SELCHANGE(IDC_PRESET_BOX, OnLbnSelchangePresetBox)
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

	CmusikStringArray items;
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

void CmusikEqualizerSets::OnBnClickedAdd()
{
	CmusikEQSettings settings;
	CmusikEqualizerBar* pBar = (CmusikEqualizerBar*)m_Parent;
	pBar->GetCtrl()->BandsToEQSettings( &settings );

	CString name;
	CmusikNameEntry* pDlg = new CmusikNameEntry( this, &name );
	if ( pDlg->DoModal() == IDOK && !name.IsEmpty() )
	{
		settings.m_Name = name;
		int ret = m_Library->CreateEqualizer( settings, true );
		if ( ret == 0 )
		{
			m_PresetBox.AddString( name );
			m_IDs.push_back( settings.m_ID );
		}
		else if ( ret == MUSIK_LIBRARY_ID_EXISTS )
			MessageBox( _T( "Sorry, but an equalizer preset with this name already exists. Please enter a unique name." ), MUSIK_VERSION_STR, MB_OK | MB_ICONWARNING );
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
	else if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE && GetFocus() == &m_PresetBox )
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

				m_PresetBox.SetCurSel( m_PresetBox.GetCount() - 1 );
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

void CmusikEqualizerSets::GetActiveEqualizer( CmusikEQSettings* settings )
{
	CmusikEqualizerBar* pBar = (CmusikEqualizerBar*)m_Parent;
	pBar->GetCtrl()->BandsToEQSettings( settings );
}

///////////////////////////////////////////////////

void CmusikEqualizerSets::OnBnClickedDeleteSel()
{
	int nSel = GetIndex();

	if ( nSel > -1 )
	{
		if ( m_Library->DeleteEqualizer( m_IDs.at( nSel ) ) == MUSIK_LIBRARY_OK )
		{
			m_PresetBox.DeleteString( nSel );
			m_IDs.erase( m_IDs.begin() + nSel );
	
			--nSel;
			if ( nSel == -1 )
				nSel = 0;

			m_PresetBox.SetCurSel( nSel );
		}
	}
}

///////////////////////////////////////////////////
