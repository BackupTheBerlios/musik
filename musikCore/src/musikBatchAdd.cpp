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
//   CmusikBatchAdd
//
// Filename(s): 
//
//   CmusikBatchAdd.h, CmusikBatchAdd.cpp
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

#include "../include/musikBatchAdd.h"

#include "../include/musikLibrary.h"
#include "../include/musikPlayer.h"
#include "../include/musikFunctor.h"
#include "../include/musikThread.h"

///////////////////////////////////////////////////

static void musikBatchAddWorker( CmusikBatchAdd* params )
{
	size_t curr_prog = 0;
	size_t last_prog = 0;

	CmusikSong song;

	params->m_Library->BeginTransaction();
	for( size_t i = 0; i < params->m_Files->size(); i++ )
	{
		if ( params->IsKilling() )
		{
			TRACE0( "CmusikBatchAdd thread was killed!\n" );
			break;
		}

		// add the song
		params->m_Library->AddSong( params->m_Files->at( i ) );

		if ( params->m_AddToPlayer && params->m_Player && params->m_Player->GetPlaylist() )
		{
			params->m_Library->GetSongFromFilename( params->m_Files->at( i ), song );
			params->m_Player->GetPlaylist()->Add( song );
		}

		else if ( params->m_UpdatePlaylist && params->m_Playlist )
		{
			if ( params->m_Functor->VerifyPlaylist( (void*)params->m_Playlist ) )
			{
				params->m_Library->GetSongFromFilename( params->m_Files->at( i ), song );
				params->m_Playlist->Add( song );
			}
			else
			{
				CStdString s;
				s.Format( "Failed to add song to playlist at address %d becuase it couldn't be verified", params->m_Playlist );
				TRACE0( s.c_str() );
			}
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

	params->m_Functor->OnThreadEnd( (void*)params );
}

///////////////////////////////////////////////////

CmusikBatchAdd::CmusikBatchAdd()
{
	m_Files				= NULL;
	m_Playlist			= NULL;
	m_Functor			= NULL;
	m_UpdatePlaylist	= false;
	m_DeleteFilelist	= true;
	m_AddToPlayer		= false;
	m_Library			= NULL;
	m_Player			= NULL;
	m_pThread			= NULL;
	m_Kill				= false;
}

///////////////////////////////////////////////////

CmusikBatchAdd::CmusikBatchAdd( CStdStringArray* pFiles, CmusikPlaylist* pPlaylist, CmusikLibrary* pLibrary, CmusikPlayer* pPlayer, 
		CmusikFunctor* pFunctor, bool bUpdatePlaylist, bool bAddToPlayer, bool bDeleteFilelist )
{
	m_Files				= pFiles;
	m_Playlist			= pPlaylist;
	m_Functor			= pFunctor;
	m_Library			= pLibrary;
	m_UpdatePlaylist	= bUpdatePlaylist;
	m_DeleteFilelist	= bDeleteFilelist;
	m_Player			= pPlayer;
	m_pThread			= NULL;
	m_Kill				= false;
	m_AddToPlayer		= bAddToPlayer;
}

///////////////////////////////////////////////////

CmusikBatchAdd::~CmusikBatchAdd()
{
	if ( m_pThread )
	{
		m_pThread->Kill();
		delete m_pThread;
	}
}

///////////////////////////////////////////////////

void CmusikBatchAdd::Run()
{
	m_Kill = false;
	if ( m_Files && m_Functor && !m_pThread )
	{
		m_pThread = new CmusikThread();
		m_pThread->Start( (ACE_THR_FUNC)musikBatchAddWorker, (void*)this );
		return;
	}

	TRACE0( "CmusikBatchAdd::Run() failed\n" );
}

///////////////////////////////////////////////////

void CmusikBatchAdd::Pause()
{
	if ( m_pThread )
		m_pThread->Pause();
}

///////////////////////////////////////////////////

void CmusikBatchAdd::Resume()
{
	if ( m_pThread )
		m_pThread->Resume();
}

///////////////////////////////////////////////////