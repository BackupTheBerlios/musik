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
//   CMusikBatchAdd
//
// Filename(s): 
//
//   CMusikBatchAdd.h, CMusikBatchAdd.cpp
//
// Information:
//
//   A thread to add files to a given library. 
//
// Usage: 
//
//   Create the class and call Run(). Upon completion,
//   the functor's OnThreadComplete() function will be
//   called.
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "../include/MusikBatchAdd.h"

#include "../include/MusikLibrary.h"
#include "../include/MusikFunctor.h"
#include "../include/MusikThread.h"

///////////////////////////////////////////////////

static void MusikBatchAddWorker( CMusikBatchAdd* params )
{
	size_t curr_prog = 0;
	size_t last_prog = 0;

	params->m_Library->BeginTransaction();
	for( size_t i = 0; i < params->m_Files->size(); i++ )
	{
		if ( params->IsKilling() )
		{
			TRACE0( "CMusikBatchAdd thread was killed!\n" );
			break;
		}

		// add the song
		params->m_Library->AddSong( params->m_Files->at( i ) );

		if ( params->m_UpdatePlaylist && params->m_Playlist )
		{
			
		}

		// post progress to the functor
		curr_prog = ( 100 * i ) / params->m_Files->size();
		if ( curr_prog != last_prog )
		{
			params->m_Functor->OnThreadProgress( curr_prog );
			last_prog = curr_prog;
		}

	}
	params->m_Library->EndTransaction();

	if ( params->m_DeleteFilelist )
		delete params->m_Files;

	params->m_Functor->OnThreadEnd();
}

///////////////////////////////////////////////////

CMusikBatchAdd::CMusikBatchAdd()
{
	m_Files = NULL;
	m_Playlist = NULL;
	m_Functor = NULL;
	m_UpdatePlaylist = false;
	m_DeleteFilelist = true;
	m_Library = NULL;

	m_pThread = NULL;
	m_Kill = false;
}

///////////////////////////////////////////////////

CMusikBatchAdd::CMusikBatchAdd( CStdStringArray* pFiles, CMusikPlaylist* pPlaylist, 
	CMusikLibrary* pLibrary, CMusikFunctor* pFunctor, bool bUpdatePlaylist, 
	bool bDeleteFilelist )
{
	m_Files = pFiles;
	m_Playlist = pPlaylist;
	m_Functor = pFunctor;
	m_Library = pLibrary;
	m_UpdatePlaylist = bUpdatePlaylist;
	m_DeleteFilelist = bDeleteFilelist;

	m_pThread = NULL;
	m_Kill = false;
}

///////////////////////////////////////////////////

CMusikBatchAdd::~CMusikBatchAdd()
{
	if ( m_pThread )
	{
		m_pThread->Kill();
		delete m_pThread;
	}
}

///////////////////////////////////////////////////

void CMusikBatchAdd::Run()
{
	m_Kill = false;
	if ( m_Files && m_Functor && !m_pThread )
	{
		m_pThread = new CMusikThread();
		m_pThread->Start( (ACE_THR_FUNC)MusikBatchAddWorker, (void*)this );
		return;
	}

	TRACE0( "CMusikBatchAdd::Run() failed\n" );
}

///////////////////////////////////////////////////

void CMusikBatchAdd::Pause()
{
	m_pThread->Pause();
}

///////////////////////////////////////////////////

void CMusikBatchAdd::Resume()
{
	m_pThread->Resume();
}

///////////////////////////////////////////////////