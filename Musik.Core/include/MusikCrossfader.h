///////////////////////////////////////////////////
//
// Class(s): 
//
//   CMusikCrossfader 
//
// Filename(s): 
//
//   MusikCrossfader.h,
//
// Information:
//
//   A simple class to manage crossfade durations for
//   various events.
//
// Example: 
//
//   The UI can construct these objects and pass them to
//   the player via CMusikPlayer::SetCrossfader()
//
//   CMusikLibrary, however, will load the default values
//   on a per song basis.
//
// Usage: 
//
//   Construct the object (takes no args), then use 
//   CMusikCrossfader::Set() to set the values, then
//   CMusikPlayer::SetCrossfader()
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

#ifndef C_MUSIK_CROSSFADER
#define C_MUSIK_CROSSFADER

///////////////////////////////////////////////////

#include "StdString.h"

///////////////////////////////////////////////////

enum 
{
	MUSIK_CROSSFADER_NONE = -1,
	MUSIK_CROSSFADER_NEW_SONG,
	MUSIK_CROSSFADER_PAUSE_RESUME,
	MUSIK_CROSSFADER_STOP,
	MUSIK_CROSSFADER_EXIT
};

///////////////////////////////////////////////////

class CMusikCrossfader
{
public:
	CMusikCrossfader()
	{
		m_NewSong		= 2.0f;
		m_PauseResume	= 0.5f;
		m_Stop			= 1.0f;
		m_Exit			= 3.0f;
	}

	~CMusikCrossfader()
	{
	}

	void Set( int type, float duration )
	{
		switch( type )
		{
		case MUSIK_CROSSFADER_NEW_SONG:
			m_NewSong = duration;
			return;
		case MUSIK_CROSSFADER_PAUSE_RESUME:
			m_PauseResume = duration;
			return;
		case MUSIK_CROSSFADER_STOP:
			m_Stop = duration;
			return;
		case MUSIK_CROSSFADER_EXIT:
			m_Exit = duration;
			return;
		}
	}

	void Set( float newsong, float pauseresume, float stop, float exit )
	{
		m_NewSong		= newsong;
		m_PauseResume	= pauseresume;
		m_Stop			= stop;
		m_Exit			= exit;
	}

	float GetDuration( int type )
	{
		switch( type )
		{
		case MUSIK_CROSSFADER_NONE:
			return 0;

		case MUSIK_CROSSFADER_NEW_SONG:
			return m_NewSong;

		case MUSIK_CROSSFADER_PAUSE_RESUME:
			return m_PauseResume;

		case MUSIK_CROSSFADER_STOP:
			return m_Stop;

		case MUSIK_CROSSFADER_EXIT:
			return m_Exit;
		}

		return 0.0f;
	}

	void SetName( const CStdString& name )
	{
		m_Name = name;
	}

	CStdString GetName()
	{
			return m_Name;
	}

private:

	float m_NewSong;
	float m_PauseResume;
	float m_Stop;
	float m_Exit;

	CStdString m_Name;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
