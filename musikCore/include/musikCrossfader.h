
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
//   CmusikCrossfader 
//
// Filename(s): 
//
//   musikCrossfader.h
//
// Information:
//
//   A simple class to manage crossfade durations for
//   various events.
//
// Usage: 
//
//   The UI can construct these objects and pass them to
//   the player via CmusikPlayer::SetCrossfader()
//
//   CmusikLibrary, however, will load the default values
//   on a per song basis.
//
//   Construct the object (takes no args), then use 
//   CmusikCrossfader::Set() to set the values, then
//   CmusikPlayer::SetCrossfader()
//
///////////////////////////////////////////////////

#ifndef C_MUSIK_CROSSFADER
#define C_MUSIK_CROSSFADER

///////////////////////////////////////////////////

#include "musikConfig.h"

///////////////////////////////////////////////////

namespace musik {

///////////////////////////////////////////////////

enum 
{
	MUSIK_CROSSFADER_NONE = -1,
	MUSIK_CROSSFADER_NEW_SONG,
	MUSIK_CROSSFADER_PAUSE_RESUME,
	MUSIK_CROSSFADER_SEEK,
	MUSIK_CROSSFADER_STOP,
	MUSIK_CROSSFADER_EXIT
};

///////////////////////////////////////////////////

class CmusikCrossfader
{

public:

	// construct and destruct
	CmusikCrossfader();
	~CmusikCrossfader(){}

	// sets
	void Set( int type, float duration );
	void Set( float newsong, float pauseresume, float seek, float stop, float exit );
	void SetName( const CmusikString& name ){ m_Name = name; }

	// gets
	float GetDuration( int type );
	CmusikString GetName(){	return m_Name;	}

	// can be modified directly
	float m_NewSong;
	float m_PauseResume;
	float m_Seek;
	float m_Stop;
	float m_Exit;
	int m_ID;
	CmusikString m_Name;
};


///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
