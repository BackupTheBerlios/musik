///////////////////////////////////////////////////
//
// Class(s): 
//
//   CMusikEqualizer 
//
// Filename(s): 
//
//   MusikEqualizer.h,
//   MusikEqualizer.cpp
//
// Information:
//
//   Musik's 18 band, dual channel equalizer based on
//   Shibatch.
//
// Example: 
//
//   The user shouldn't have to use this directly, it should
//   only be called within CMusikPlayer as a private member
//   object. It just contains DSP callbacks to modify the streams
//
// Usage: 
//
//   Void.
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2003
//
//   Casey Langen   : Lead Developer, Project Manager
//   Dustin Carter  : Developer, pain in the ass
//   Simon Windmill : Developer, the most helpful person thats immediately available ;)
// 
// License:
//
//   See license.txt included with this distribution
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

private:

	CMusikEQSettings m_EQ;
	CMusikLibrary* m_Library;

	float m_Left[MUSIK_EQ_SETTINGS_NUM_BANDS], m_Right[MUSIK_EQ_SETTINGS_NUM_BANDS];
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////