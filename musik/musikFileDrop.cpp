///////////////////////////////////////////////////

#include "stdafx.h"

#include "musik.h"
#include "musikFileDrop.h"
#include ".\musikfiledrop.h"

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CmusikFileDrop, CDialog)

///////////////////////////////////////////////////

CmusikFileDrop::CmusikFileDrop( CWnd* pParent )
	: CDialog( CmusikFileDrop::IDD, pParent )
{
	m_Ret = MUSIK_FILEDROP_ADDNOWPLAYING;
	m_FirstRun = true;
}

///////////////////////////////////////////////////

CmusikFileDrop::~CmusikFileDrop()
{

}

///////////////////////////////////////////////////

void CmusikFileDrop::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikFileDrop, CDialog)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RADIO_ADD_PLAYLIST, OnBnClickedRadioAddPlaylist)
	ON_BN_CLICKED(IDC_RADIO_ADD_NOW_PLAYING, OnBnClickedRadioAddNowPlaying)
	ON_BN_CLICKED(IDC_RADIO_ADD_LIBRARY, OnBnClickedRadioAddLibrary)
	ON_WM_SHOWWINDOW()
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
