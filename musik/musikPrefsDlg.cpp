///////////////////////////////////////////////////

#include "stdafx.h"

#include "musik.h"
#include "musikPrefsDlg.h"
#include ".\musikprefsdlg.h"

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
}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikPrefsInterfaceGeneral, CPropertyPage)
END_MESSAGE_MAP()

///////////////////////////////////////////////////
