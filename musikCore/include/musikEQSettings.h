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
// Class(s): 
//
//   CmusikEQSettings 
//
// Filename(s): 
//
//   musikEQSettings.h,
//
// Information:
//
//   A simple class to manage the band positions of
//   musik's built in 18 band, dual channel equalizer.
//
// Usage: 
//
//   The UI can construct these objects and pass them to
//   the player via CmusikPlayer::SetEQ()
//
//   Construct the object (takes no args), then use 
//   CmusikEqualizer::Set() to set the values, then
//   CmusikPlayer::SetEQ()
//
///////////////////////////////////////////////////

#ifndef C_MUSIK_EQ_SETTINGS
#define C_MUSIK_EQ_SETTINGS

///////////////////////////////////////////////////

enum 
{
	MUSIK_EQ_SETTINGS_LEFT_BAND = 0,
	MUSIK_EQ_SETTINGS_RIGHT_BAND,
	MUSIK_EQ_SETTINGS_BOTH_BANDS
};

///////////////////////////////////////////////////

#define MUSIK_EQ_SETTINGS_NUM_BANDS 18

///////////////////////////////////////////////////

class CmusikEQSettings
{
public:
	CmusikEQSettings()
	{
		for ( size_t i = 0; i < MUSIK_EQ_SETTINGS_NUM_BANDS; i++ )
		{
			m_Left[i] = 1.0f;
			m_Right[i] = 1.0f;
		}
	}

	~CmusikEQSettings()
	{
	}

	void Set( int which_band, float values[MUSIK_EQ_SETTINGS_NUM_BANDS] )
	{
		// left band
		if ( which_band == MUSIK_EQ_SETTINGS_LEFT_BAND )
		{
			for ( size_t i = 0; i < MUSIK_EQ_SETTINGS_NUM_BANDS; i++ )
				m_Left[i] = values[i];
			return;
		}

		// right band
		else if ( which_band == MUSIK_EQ_SETTINGS_RIGHT_BAND )
		{
			for ( size_t i = 0; i < MUSIK_EQ_SETTINGS_NUM_BANDS; i++ )
				m_Right[i] = values[i];
			return;
		}

		// both bands
		else if ( which_band == MUSIK_EQ_SETTINGS_BOTH_BANDS )
		{
			for ( size_t i = 0; i < MUSIK_EQ_SETTINGS_NUM_BANDS; i++ )
			{
				m_Left[i] = values[i];
				m_Right[i] = values[i];
			}
		}
	}

	float m_Left[MUSIK_EQ_SETTINGS_NUM_BANDS];
	float m_Right[MUSIK_EQ_SETTINGS_NUM_BANDS];

	CStdString m_Name;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
