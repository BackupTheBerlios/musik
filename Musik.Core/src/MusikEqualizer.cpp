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