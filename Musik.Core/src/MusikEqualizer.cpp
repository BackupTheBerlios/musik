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

#include "stdafx.h"

#include "../include/MusikLibrary.h"
#include "../include/MusikEqualizer.h"
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

CMusikEqualizer::CMusikEqualizer( CMusikLibrary* library )
{
	m_Library = library;
}

///////////////////////////////////////////////////

CMusikEqualizer::~CMusikEqualizer()
{

}

///////////////////////////////////////////////////

void CMusikEqualizer::Set( int songid )
{

}

///////////////////////////////////////////////////

void CMusikEqualizer::SetLibrary( CMusikLibrary* library )
{
	m_Library = library;
}

///////////////////////////////////////////////////

void CMusikEqualizer::InitEqualizer()
{
	// 14 is the magic number
	equ_init( 14 ); 
	equ_makeTable( m_EQ.m_Left, m_EQ.m_Right, &paramroot, 44100 );
}

///////////////////////////////////////////////////

void CMusikEqualizer::CleanEqualizer()
{
	equ_quit();
}

///////////////////////////////////////////////////

void CMusikEqualizer::ProcessDSP( void* buffer, int length, int channels, int bitspersample )
{
	equ_modifySamples( (char*)buffer, length, channels, bitspersample );
}

///////////////////////////////////////////////////
