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
//   CmusikPropertyPage, CmusikPropertySheet
//
// Filename(s): 
//
//   musikPropertyPage.h, musikPropertyPage.cpp
//
// Information:
//
//   Property page that accepts a pointer to a CmusikPrefs obj
// 
// Usage: 
//
//   Use like a regular property page
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "musikPropertyPage.h"

///////////////////////////////////////////////////

// CmusikPropertyPage

///////////////////////////////////////////////////

CmusikPropertyPage::CmusikPropertyPage( UINT nIDTemplate, CmusikPrefs* prefs )
	: CPropertyPage( nIDTemplate )
{
	m_Prefs = prefs;
}

///////////////////////////////////////////////////

// CmusikPropertySheet

///////////////////////////////////////////////////

CmusikProptertySheet::CmusikProptertySheet( LPCSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage )
	: CTreePropSheet( pszCaption, pParentWnd, iSelectPage )
{

}

///////////////////////////////////////////////////

BOOL CmusikProptertySheet::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch ( wParam )
	{
	case IDCANCEL:

		return CTreePropSheet::OnCommand( wParam, lParam );
		break;

	case IDOK:

		//ok

		return true;
		break;

	case ID_APPLY_NOW:

		// apply

		return true;
		break;
	}
	
	return CTreePropSheet::OnCommand( wParam, lParam );
}

///////////////////////////////////////////////////
