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

#ifndef C_MUSIK_EQUALIZER
#define C_MUSIK_EQUALIZER

///////////////////////////////////////////////////

#include "musikConfig.h"
#include "musikEQSettings.h"
#include "musikLibrary.h"

///////////////////////////////////////////////////

class CmusikEqualizer
{
public:
	CmusikEqualizer( CmusikLibrary* library );
	~CmusikEqualizer();

	CmusikEQSettings m_EQ_Values;
	bool m_EQ_Values_Modified;

	void SaveCurr();
		
	void SetNewSong( int songid );
	void SetLibrary( CmusikLibrary* library );

	void UpdateTable();

	void ProcessDSP( void* buffer, int length, int channels, int bitspersample );

private:

	// internal functions for initializing and
	// cleaning the eq settings...
	void InitEqualizer();
	void CleanEqualizer();

	bool m_TableSet;
	int m_SongID;

	// pointer to the library that contains song
	// and equalizer info
	CmusikLibrary* m_Library;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
