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

#include "stdafx.h"

#include "../include/musikLibrary.h"
#include "../include/musikEqualizer.h"
#include "../include/paramlist.hpp"

///////////////////////////////////////////////////

// shibatch

extern void		equ_init			( int wb );
extern void		equ_makeTable		( float* lbc, float* rbc, paramlist*, float fs );
extern void		equ_quit			( );
extern int		equ_modifySamples	( char *buf, int nsamples, int nch, int bps );
extern void		equ_clearbuf		( int, int );
static			paramlist			paramroot;

///////////////////////////////////////////////////

CmusikEqualizer::CmusikEqualizer( CmusikLibrary* library )
{
	m_TableSet = false;
	m_EQ_Values_Modified = false;
	m_SongID = -1;

	m_Library = library;

	// if the equalizer is active,
	// create the table with the correct
	// band info...
	InitEqualizer();
}

///////////////////////////////////////////////////

CmusikEqualizer::~CmusikEqualizer()
{
	if ( m_TableSet )
		CleanEqualizer();
}

///////////////////////////////////////////////////

void CmusikEqualizer::SetLibrary( CmusikLibrary* library )
{
	m_Library = library;
}

///////////////////////////////////////////////////

void CmusikEqualizer::InitEqualizer()
{
	if ( m_TableSet )
		CleanEqualizer();

	// 14 is the magic number
	equ_init( 14 ); 

	// MUST be called before CmusikEqualizer::ProcessDSP()
	equ_makeTable( m_EQ_Values.m_Left, m_EQ_Values.m_Right, &paramroot, 44100 );

	m_TableSet = true;
}

///////////////////////////////////////////////////

void CmusikEqualizer::CleanEqualizer()
{
	equ_quit();
	m_TableSet = false;
}

///////////////////////////////////////////////////

void CmusikEqualizer::ProcessDSP( void* buffer, int length, int channels, int bitspersample )
{
	equ_modifySamples( (char*)buffer, length, channels, bitspersample );
}

///////////////////////////////////////////////////

void CmusikEqualizer::GetSongEq( int songid )
{
	// save the old equalizer if it has been
	// flagged as modified...
	if ( m_SongID != -1 )
	{
		if ( m_EQ_Values_Modified )
		{
			if ( m_EQ_Values.m_ID == -1 )
			{
				CStdString fn;
				m_Library->GetFieldFromID( m_SongID, MUSIK_LIBRARY_TYPE_FILENAME, fn );

				m_Library->CreateEqualizer( m_EQ_Values, fn );
			}
			else
				m_Library->UpdateEqualizer( m_EQ_Values.m_ID, m_EQ_Values );

			m_EQ_Values_Modified = false;
		}
	}

	// load the current equalizer
	if ( m_Library )
	{
		int nID = m_Library->GetEqualizerIDFromSongID( songid );

		if ( nID > -1 )
		{
			CmusikEQSettings eq;
			m_Library->GetEqualizer( nID, &eq );

			m_EQ_Values.Set( MUSIK_EQ_SETTINGS_LEFT_BAND, eq.m_Left );
			m_EQ_Values.Set( MUSIK_EQ_SETTINGS_RIGHT_BAND, eq.m_Right );
		}
		else
		{
			// TODO: load default equalizer!
		}
	}

	m_SongID = songid;
}

///////////////////////////////////////////////////

