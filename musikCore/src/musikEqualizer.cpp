///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a a cross platform, open source
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
	m_Library = library;
}

///////////////////////////////////////////////////

CmusikEqualizer::~CmusikEqualizer()
{

}

///////////////////////////////////////////////////

void CmusikEqualizer::Set( int songid )
{

}

///////////////////////////////////////////////////

void CmusikEqualizer::SetLibrary( CmusikLibrary* library )
{
	m_Library = library;
}

///////////////////////////////////////////////////

void CmusikEqualizer::InitEqualizer()
{
	// 14 is the magic number
	equ_init( 14 ); 
	equ_makeTable( m_EQ.m_Left, m_EQ.m_Right, &paramroot, 44100 );
}

///////////////////////////////////////////////////

void CmusikEqualizer::CleanEqualizer()
{
	equ_quit();
}

///////////////////////////////////////////////////

void CmusikEqualizer::ProcessDSP( void* buffer, int length, int channels, int bitspersample )
{
	equ_modifySamples( (char*)buffer, length, channels, bitspersample );
}

///////////////////////////////////////////////////
