///////////////////////////////////////////////////
// 
// Info:
//
//   Musik is a a cross platform, open source
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
//   CMusikEqualizer
//
// Filename(s): 
//
//   MusikEqualizer.h, MusikEqualizer.cpp
//
// Information:
//
//   CMusikEqualizer is a front end to the shibatch equalizer
//   tools. 
// 
// Usage: 
//
//   CMusikEqualizer should not be used, except by CMusikPlayer
//
///////////////////////////////////////////////////

#include "MusikEQSettings.h"

///////////////////////////////////////////////////

#ifndef C_MUSIK_EQUALIZER
#define C_MUSIK_EQUALIZER

///////////////////////////////////////////////////

class CMusikEQSettings;

///////////////////////////////////////////////////

class CMusikEqualizer
{
public:
	CMusikEqualizer( CMusikLibrary* library );
	~CMusikEqualizer();

	void Set( int songid );
	void SetLibrary( CMusikLibrary* library );

	void InitEqualizer();
	void CleanEqualizer();
	void ProcessDSP( void* buffer, int length, int channels, int bitspersample );

private:

	CMusikEQSettings m_EQ;
	CMusikLibrary* m_Library;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
