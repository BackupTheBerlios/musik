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
//   CmusikSaveStdPlaylist
//
// Filename(s): 
//
//   musikSaveStdPlaylist.h, musikSaveStdPlaylist.cpp
//
// Information:
//
//   Dialog used when prompting user to save a standard playlist
// 
// Usage: 
//
//   Void.
//
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
