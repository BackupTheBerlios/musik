///////////////////////////////////////////////////
//
// Class(s): 
//
//   CMusikEqualizer 
//
// Filename(s): 
//
//   MusikEqualizer.h,
//
// Information:
//
//   A simple class to manage the band positions of
//   Musik's built in 18 band, dual channel equalizer.
//
// Example: 
//
//   The UI can construct these objects and pass them to
//   the player via CMusikPlayer::SetEQ()
//
// Usage: 
//
//   Construct the object (takes no args), then use 
//   CMusikEqualizer::Set() to set the values, then
//   CMusikPlayer::SetEQ()
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

#ifndef C_MUSIK_EQUALIZER
#define C_MUSIK_EQUALIZER

///////////////////////////////////////////////////

enum 
{
	MUSIK_EQUALIZER_LEFT_BAND = 0,
	MUSIK_EQUALIZER_RIGHT_BAND,
	MUSIK_EQUALIZER_BOTH_BANDS
};

///////////////////////////////////////////////////

#define MUSIK_EQUALIZER_NUM_BANDS 18

///////////////////////////////////////////////////

class CMusikEqualizer
{
public:
	CMusikEqualizer()
	{
		for ( size_t i = 0; i < MUSIK_EQUALIZER_NUM_BANDS; i++ )
		{
			m_Left[i] = 1.0f;
			m_Right[i] = 1.0f;
		}
	}

	~CMusikEqualizer()
	{
	}

	void Set( int which_band, float values[MUSIK_EQUALIZER_NUM_BANDS] )
	{
		// left band
		if ( which_band == MUSIK_EQUALIZER_LEFT_BAND )
		{
			for ( size_t i = 0; i < MUSIK_EQUALIZER_NUM_BANDS; i++ )
				m_Left[i] = values[i];
			return;
		}

		// right band
		else if ( which_band == MUSIK_EQUALIZER_RIGHT_BAND )
		{
			for ( size_t i = 0; i < MUSIK_EQUALIZER_NUM_BANDS; i++ )
				m_Right[i] = values[i];
			return;
		}

		// both bands
		else if ( which_band == MUSIK_EQUALIZER_BOTH_BANDS )
		{
			for ( size_t i = 0; i < MUSIK_EQUALIZER_NUM_BANDS; i++ )
			{
				m_Left[i] = values[i];
				m_Right[i] = values[i];
			}
		}
	}

private:

	float m_Left[MUSIK_EQUALIZER_NUM_BANDS], m_Right[MUSIK_EQUALIZER_NUM_BANDS];
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////