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
//   musikCrossfader.cpp
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

#include "stdafx.h"
#include "../include/musikCrossfader.h"

///////////////////////////////////////////////////

CmusikCrossfader::CmusikCrossfader()
{
	m_NewSong		= 0.0f;
	m_PauseResume	= 0.0f;
	m_Seek			= 0.0f;
	m_Stop			= 0.0f;
	m_Exit			= 0.0f;

	m_ID = -1;
}

///////////////////////////////////////////////////

void CmusikCrossfader::Set( int type, float duration )
{
	switch( type )
	{
	case MUSIK_CROSSFADER_NEW_SONG:
		m_NewSong = duration;
		return;

	case MUSIK_CROSSFADER_PAUSE_RESUME:
		m_PauseResume = duration;
		return;

	case MUSIK_CROSSFADER_SEEK:
		m_Seek = duration;
		return;

	case MUSIK_CROSSFADER_STOP:
		m_Stop = duration;
		return;

	case MUSIK_CROSSFADER_EXIT:
		m_Exit = duration;
		return;
	}
}

///////////////////////////////////////////////////

void CmusikCrossfader::Set( float newsong, float pauseresume, float seek, float stop, float exit )
{
	m_NewSong		= newsong;
	m_PauseResume	= pauseresume;
	m_Seek			= seek;
	m_Stop			= stop;
	m_Exit			= exit;
}

///////////////////////////////////////////////////

float CmusikCrossfader::GetDuration( int type )
{
	switch( type )
	{
	case MUSIK_CROSSFADER_NONE:
		return 0;

	case MUSIK_CROSSFADER_NEW_SONG:
		return m_NewSong;

	case MUSIK_CROSSFADER_PAUSE_RESUME:
		return m_PauseResume;

	case MUSIK_CROSSFADER_SEEK:
		return m_Seek;

	case MUSIK_CROSSFADER_STOP:
		return m_Stop;

	case MUSIK_CROSSFADER_EXIT:
		return m_Exit;
	}

	return 0.0f;
}

///////////////////////////////////////////////////
