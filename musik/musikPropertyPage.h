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
//   CmusikPropertyPage
//
// Filename(s): 
//
//   musikPropertyPage.h
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

#pragma once

///////////////////////////////////////////////////

#include "musikPrefs.h"

///////////////////////////////////////////////////

class CmusikPropertyPage : public CPropertyPage
{
public:

	CmusikPropertyPage( UINT nIDTemplate, CmusikPrefs* prefs );
	virtual ~CmusikPropertyPage(){}

protected:

	CmusikPrefs* m_Prefs;
};

///////////////////////////////////////////////////

CmusikPropertyPage::CmusikPropertyPage( UINT nIDTemplate, CmusikPrefs* prefs )
	: CPropertyPage( nIDTemplate )
{
	m_Prefs = prefs;
}

///////////////////////////////////////////////////