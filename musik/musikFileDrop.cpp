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
//   CmusikFileDrop
//
// Filename(s): 
//
//   musikFileDrop.h, musikFileDrop.cpp
//
// Information:
//
//   Dialog used when files are dropped on a CmusikPlaylistCtrl
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "musikFileDrop.h"

#include "musik.h"
#include "musikPrefs.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikFileDrop, CDialog)

///////////////////////////////////////////////////

CmusikFileDrop::CmusikFileDrop( CWnd* pParent, CmusikPrefs* pPrefs )
	: CDialog( IDD_FILEDROP, pParent )
{
	m_Ret = MUSIK_FILEDROP_ADDNOWPLAYING;
	m_FirstRun = true;
	m_Prefs = pPrefs;
}

///////////////////////////////////////////////////

CmusikFileDrop::~CmusikFileDrop()
{

}

///////////////////////////////////////////////////

void CmusikFileDrop::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REMEMBER, m_Remember);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikFileDrop, CDialog)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RADIO_ADD_PLAYLIST, OnBnClickedRadioAddPlaylist)
	ON_BN_CLICKED(IDC_RADIO_ADD_NOW_PLAYING, OnBnClickedRadioAddNowPlaying)
	ON_BN_CLICKED(IDC_RADIO_ADD_LIBRARY, OnBnClickedRadioAddLibrary)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

///////////////////////////////////////////////////

INT_PTR CmusikFileDrop::DoModal()
{
	CDialog::DoModal();

	return m_Ret;
}

///////////////////////////////////////////////////

void CmusikFileDrop::OnBnClickedCancel()
{
	m_Ret = MUSIK_FILEDROP_CANCEL;
	OnCancel();
}

///////////////////////////////////////////////////

void CmusikFileDrop::OnBnClickedRadioAddPlaylist()
{
	m_Ret = MUSIK_FILEDROP_ADDPLAYLIST;
}

///////////////////////////////////////////////////

void CmusikFileDrop::OnBnClickedRadioAddNowPlaying()
{
	m_Ret = MUSIK_FILEDROP_ADDNOWPLAYING;
}

///////////////////////////////////////////////////

void CmusikFileDrop::OnBnClickedRadioAddLibrary()
{
	m_Ret = MUSIK_FILEDROP_ADDLIBRARY;
}

///////////////////////////////////////////////////

void CmusikFileDrop::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if ( m_FirstRun )
	{
		CButton* pBtn = (CButton*)GetDlgItem( IDC_RADIO_ADD_NOW_PLAYING );
		pBtn->SetCheck( true );
		m_FirstRun = false;

		::SetWindowPos( GetSafeHwnd(), wndTopMost, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW );
	}
}

///////////////////////////////////////////////////

void CmusikFileDrop::OnBnClickedOk()
{
	if ( m_Remember.GetCheck() && m_Prefs )
		m_Prefs->SetFileDropPrompt( m_Ret );

	OnOK();
}

///////////////////////////////////////////////////
