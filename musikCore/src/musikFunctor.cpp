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
//   CmusikFunctor
//
// Filename(s): 
//
//   musikFunctor.cpp
//
// Information:
//
//   This class provides a generic way for CmusikPlayer
//   to pass events to a UI.
// 
// Usage: 
//
//   This can be used to notify a UI when the currently 
//   playing song has changed.
//
//   This object is pure virtual, so instantiate a new
//   object from it, overriding the Call() function. When
//   constructing a CmusikPlayer object, send this new 
//   object as the Functor.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "../include/musikFunctor.h"

///////////////////////////////////////////////////

using namespace musik;

///////////////////////////////////////////////////

CmusikFunctor::CmusikFunctor()
{
	// pure virtual
};

///////////////////////////////////////////////////

CmusikFunctor::~CmusikFunctor()
{
	// pure virtual
}

///////////////////////////////////////////////////

void CmusikFunctor::OnNewSong()
{
	// pure virtual
}

///////////////////////////////////////////////////

void CmusikFunctor::OnPause()
{
	// pure virtual
}

///////////////////////////////////////////////////

void CmusikFunctor::OnResume()
{
	// pure virtual
}

///////////////////////////////////////////////////

void CmusikFunctor::OnStop()
{
	// pure virtual
}

///////////////////////////////////////////////////

void CmusikFunctor::OnTaskStart()
{
	// pure virtual
}

///////////////////////////////////////////////////

void CmusikFunctor::OnTaskEnd( CmusikTask* task_addr )
{
	// pure virtual
}

///////////////////////////////////////////////////

void CmusikFunctor::OnTaskProgress( size_t progress )
{
	// pure virtual
}

///////////////////////////////////////////////////

void CmusikFunctor::OnPlaybackFailed()
{
	// pure virtual
}

///////////////////////////////////////////////////

void CmusikFunctor::OnNewEqualizer()
{
	// pure virtual
}

///////////////////////////////////////////////////

bool CmusikFunctor::VerifyPlaylist( void* pl_addr )
{
	return true;
}

///////////////////////////////////////////////////

