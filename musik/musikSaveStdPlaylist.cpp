///////////////////////////////////////////////////

#include "stdafx.h"
#include "musik.h"
#include "musikSaveStdPlaylist.h"

#include "musikPrefs.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikSaveStdPlaylist, CDialog)

///////////////////////////////////////////////////

CmusikSaveStdPlaylist::CmusikSaveStdPlaylist( CWnd* pParent, CmusikPrefs* prefs )
	: CDialog( IDD_SAVESTDPLAYLIST, pParent )
{
	m_Prefs = prefs;
}

///////////////////////////////////////////////////

CmusikSaveStdPlaylist::~CmusikSaveStdPlaylist()
{
}

///////////////////////////////////////////////////

void CmusikSaveStdPlaylist::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REMEMBER, m_Remember);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikSaveStdPlaylist, CDialog)
	ON_BN_CLICKED(IDC_BTNNO, OnBnClickedNo)
	ON_BN_CLICKED(IDC_BTNYES, OnBnClickedYes)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

void CmusikSaveStdPlaylist::OnBnClickedNo()
{
	if ( m_Remember.GetCheck() )
		m_Prefs->SetStdPlaylistPrompt( 0 );
	else
		m_Prefs->SetStdPlaylistPrompt( -1 );

	OnCancel();
}

///////////////////////////////////////////////////

void CmusikSaveStdPlaylist::OnBnClickedYes()
{
	if ( m_Remember.GetCheck() )
		m_Prefs->SetStdPlaylistPrompt( 1 );
	else
		m_Prefs->SetStdPlaylistPrompt( -1 );

	OnOK();
}

///////////////////////////////////////////////////
