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

enum 
{
	MUSIK_CROSSFADER_NEW_SONG = 0,
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
		m_NewSong		= 0.0f;
		m_PauseResume	= 0.0f;
		m_Stop			= 0.0f;
		m_Exit			= 0.0f;
	}

	~CMusikCrossfader()
	{
	}

	void Set( int songid )
	{
		// will set from a song id
	}

	void Set( float newsong, float pauseresume, float stop, float exit )
	{
		m_NewSong		= newsong;
		m_PauseResume	= pauseresume;
		m_Stop			= stop;
		m_Exit			= exit;
	}

private:

	float m_NewSong;
	float m_PauseResume;
	float m_Stop;
	float m_Exit;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////