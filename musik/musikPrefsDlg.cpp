///////////////////////////////////////////////////

#include "stdafx.h"

#include "musik.h"
#include "musikPrefsDlg.h"

#include "../musikCore/include/musikFilename.h"

///////////////////////////////////////////////////

// General::Interface

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikPrefsInterfaceGeneral, CPropertyPage)

///////////////////////////////////////////////////

CmusikPrefsInterfaceGeneral::CmusikPrefsInterfaceGeneral( CmusikPrefs* prefs )
	: CmusikPropertyPage( CmusikPrefsInterfaceGeneral::IDD, prefs )
{
}

///////////////////////////////////////////////////

CmusikPrefsInterfaceGeneral::~CmusikPrefsInterfaceGeneral()
{
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_ENABLETRAYICON, m_EnableTrayIcon);
	DDX_Control(pDX, IDC_CHECK1, m_PromptSavePlaylists);
	DDX_Control(pDX, IDC_CHECK2, m_PromptDropOnPlaylist);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPrefsInterfaceGeneral, CPropertyPage)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

BOOL CmusikPrefsInterfaceGeneral::OnCommand(WPARAM wParam, LPARAM lParam)
{
	SetModified( TRUE );

	return CmusikPropertyPage::OnCommand(wParam, lParam);
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceGeneral::LoadPrefs()
{
	m_EnableTrayIcon.SetCheck( m_Prefs->MinimizeToTray() );
	m_PromptSavePlaylists.SetCheck( m_Prefs->GetStdPlaylistPrompt() == -1 ? true : false );
	m_PromptDropOnPlaylist.SetCheck( m_Prefs->GetFileDropPrompt() == -1 ? true : false );
}

///////////////////////////////////////////////////

void CmusikPrefsInterfaceGeneral::CommitChanges()
{
	m_Prefs->SetMinimizeToTray( (bool)m_EnableTrayIcon.GetCheck() );

	if ( !m_PromptSavePlaylists.GetCheck() )
	{
		if ( m_Prefs->GetStdPlaylistPrompt() == -1 )
			m_Prefs->SetStdPlaylistPrompt( 1 );
	}
	else
		m_Prefs->SetStdPlaylistPrompt( -1 );

	if ( !m_PromptDropOnPlaylist.GetCheck() )
	{
		if ( m_Prefs->GetFileDropPrompt() == -1 )
			m_Prefs->SetFileDropPrompt( 1 );
	}
	else
		m_Prefs->SetFileDropPrompt( -1 );
}

///////////////////////////////////////////////////

BOOL CmusikPrefsInterfaceGeneral::OnInitDialog()
{
	CmusikPropertyPage::OnInitDialog();
	LoadPrefs();
	return TRUE; 
}

///////////////////////////////////////////////////
