///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a a cross platform, open source
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
// Class(s): 
//
//   CmusikEqualizer
//
// Filename(s): 
//
//   musikEqualizer.h, musikEqualizer.cpp
//
// Information:
//
//   CmusikEqualizer is a front end to the shibatch equalizer
//   tools. 
// 
// Usage: 
//
//   CmusikEqualizer should not be used, except by CmusikPlayer
//
///////////////////////////////////////////////////

#include "musikEQSettings.h"

///////////////////////////////////////////////////

#ifndef C_MUSIK_EQUALIZER
#define C_MUSIK_EQUALIZER

///////////////////////////////////////////////////

class CmusikEQSettings;

///////////////////////////////////////////////////

class CmusikEqualizer
{
public:
	CmusikEqualizer( CmusikLibrary* library );
	~CmusikEqualizer();

	void Set( int songid );
	void SetLibrary( CmusikLibrary* library );

	void InitEqualizer();
	void CleanEqualizer();
	void ProcessDSP( void* buffer, int length, int channels, int bitspersample );

private:

	CmusikEQSettings m_EQ;
	CmusikLibrary* m_Library;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
