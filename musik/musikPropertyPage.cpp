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

CmusikPropertyPage::CmusikPropertyPage( UINT nIDTemplate, CmusikPrefs* prefs, CmusikLibrary* library, CmusikPlayer* player )
	: CPropertyPage( nIDTemplate )
{
	m_Prefs = prefs;
	m_Library = library;
	m_Player = player;

	m_Modified = false;
}

///////////////////////////////////////////////////

// CmusikPropertySheet

///////////////////////////////////////////////////

CmusikPropertySheet::CmusikPropertySheet( LPCSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage )
	: CTreePropSheet( pszCaption, pParentWnd, iSelectPage )
{

}

///////////////////////////////////////////////////

BOOL CmusikPropertySheet::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch ( wParam )
	{
	case IDCANCEL:

		return CTreePropSheet::OnCommand( IDCANCEL, lParam );
		break;

	case IDOK:

		CommitChanges();
		return CTreePropSheet::OnCommand( IDCANCEL, lParam );

		return true;
		break;

	case ID_APPLY_NOW:

		CommitChanges();

		return true;
		break;
	}
	
	return CTreePropSheet::OnCommand( wParam, lParam );
}

///////////////////////////////////////////////////

void CmusikPropertySheet::CommitChanges()
{
	CmusikPropertyPage* ptrPage = NULL;
	for ( int i = 0; i < GetPageCount(); i++ )
	{
		ptrPage = (CmusikPropertyPage*)GetPage( i );
		if ( ptrPage->IsModified() )
		{
			ptrPage->CommitChanges();
			ptrPage->SetModified( FALSE );
		}
	}	
}

///////////////////////////////////////////////////
