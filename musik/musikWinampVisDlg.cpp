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
//   CmusikWinampVisDlg
//
// Filename(s): 
//
//   musikWinampVisDlg.h, musikWinampVisDlg.cpp
//
// Information:
//
//   Winamp vis chooser
// 
// Usage: 
//
//   Its a dialog. Use it.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikPrefs.h"
#include "musikWinampVisDlg.h"
#include ".\musikwinampvisdlg.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikWinampVisDlg, CDialog)

///////////////////////////////////////////////////

CmusikWinampVisDlg::CmusikWinampVisDlg( CFrameWnd* parent, CmusikStringArray* vislist, CmusikPrefs* prefs )
	: CDialog(CmusikWinampVisDlg::IDD, parent)
{
	m_Parent = parent;
	m_VisList = vislist;
	m_Prefs = prefs;
}

///////////////////////////////////////////////////

CmusikWinampVisDlg::~CmusikWinampVisDlg()
{
}

///////////////////////////////////////////////////

void CmusikWinampVisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_VisDropDown);
	DDX_Control(pDX, IDC_LIST1, m_VisListBox);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikWinampVisDlg, CDialog)
	ON_WM_CLOSE()
	ON_LBN_SELCHANGE(IDC_LIST1, OnPluginBoxChange)
	ON_BN_CLICKED(IDC_CANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_OK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_SETTINGS, OnBnClickedSettings)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikWinampVisDlg::OnClose()
{
	int WM_CLOSEWINAMPVISCONFIG = RegisterWindowMessage( "CLOSEWINAMPVISCONFIG" );
	m_Parent->PostMessage( WM_CLOSEWINAMPVISCONFIG );	
}

///////////////////////////////////////////////////

BOOL CmusikWinampVisDlg::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE )
	{
		OnClose();
		return true;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////

BOOL CmusikWinampVisDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int vis_at = -1;
	for ( size_t i = 0; i < m_VisList->size(); i++ )
	{
		m_VisListBox.AddString( m_VisList->at( i ) );
		
		if ( vis_at == -1 && m_VisList->at( i ) == m_Prefs->GetWinampVisName() )
			vis_at = i;
	}

    if ( vis_at != -1 )
	{
		m_VisListBox.SetCurSel( vis_at );
		OnPluginBoxChange();
	}

	return TRUE;
}

///////////////////////////////////////////////////

void CmusikWinampVisDlg::OnPluginBoxChange()
{
	CmusikStringArray list;
	int cur_sel = GetIndex();

	int WM_GETWINAMPVISMODULES = RegisterWindowMessage( "GETWINAMPVISMODULES" );
	m_Parent->SendMessage( WM_GETWINAMPVISMODULES, (WPARAM)cur_sel, (LPARAM)&list );	

	m_VisDropDown.ResetContent();

	if ( !m_VisDropDown.GetCount() && list.size() )
	{
		for ( size_t i = 0; i < list.size(); i++ )
			m_VisDropDown.AddString( list.at( i ) );

		// if the item from the preferences is selected in
		// the list box, then try to select its last known
		// module...
		if ( (int)m_VisList->size() > GetIndex() &&
			m_VisList->at( GetIndex() ) == m_Prefs->GetWinampVisName() &&
			(int)list.size() > m_Prefs->GetWinampVisModule() )
		{
			m_VisDropDown.SelectString(  
				m_Prefs->GetWinampVisModule(), 
				list.at( m_Prefs->GetWinampVisModule() ) );
		}
		// else select first item...
		else
			m_VisDropDown.SelectString( 0, list.at( 0 ) );
	}
}

///////////////////////////////////////////////////

int CmusikWinampVisDlg::GetIndex()
{
	for ( int i = 0; i < m_VisListBox.GetCount(); i++ )
	{
		if ( m_VisListBox.GetSel( i ) )
			return i;
	}

	return -1;
}

///////////////////////////////////////////////////

void CmusikWinampVisDlg::OnBnClickedCancel()
{
	OnClose();
}

///////////////////////////////////////////////////

void CmusikWinampVisDlg::OnBnClickedOk()
{
	CString str;
	m_VisDropDown.GetWindowText( str );

	if ( GetIndex() == -1 || str.IsEmpty() )
	{
		MessageBox( "Please choose a valid winamp visualization plugin and module to continue.", MUSIK_VERSION_STR, MB_ICONINFORMATION );
		return;
	}

	m_VisListBox.GetText( GetIndex(), str );
	m_Prefs->SetWinampVisName( (string)str );
	m_Prefs->SetWinampVisModule( m_VisDropDown.GetCurSel() );
	m_Prefs->SetWinampVis( GetIndex() );

	OnClose();
}

///////////////////////////////////////////////////

void CmusikWinampVisDlg::OnBnClickedSettings()
{
	int WM_CONFIGWINAMPVIS = RegisterWindowMessage( "CONFIGWINAMPVIS" );
	m_Parent->SendMessage( WM_CONFIGWINAMPVIS, (WPARAM)GetIndex(), (LPARAM)m_VisDropDown.GetCurSel() );	
}

///////////////////////////////////////////////////