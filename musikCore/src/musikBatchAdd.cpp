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
//   CmusikBatchAdd
//
// Filename(s): 
//
//   CmusikBatchAdd.cpp
//
// Information:
//
//   A worker thread function and class to batch add
//   files or directories to the library...
//
// Usage: 
//
//   Create the new CmusikBatchAddTask and a CmusikBatchAdd
//   pass the CmusikBatchAdd to the task's argument.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "../include/musikBatchAdd.h"
#include "../include/musikPlaylist.h"

///////////////////////////////////////////////////

// CmusikBatchAdd

///////////////////////////////////////////////////

CmusikBatchAdd::CmusikBatchAdd()
{
	m_Files					= NULL;
	m_Playlist				= NULL;
	m_Functor				= NULL;
	m_UpdatePlaylist		= false;
	m_DeleteFilelist		= true;
	m_AddToPlayer			= false;
	m_Library				= NULL;
	m_Player				= NULL;
	m_CallFunctorOnAbort	= false;
}

///////////////////////////////////////////////////

CmusikBatchAdd::CmusikBatchAdd( CmusikStringArray* pFiles, CmusikPlaylist* pPlaylist, CmusikLibrary* pLibrary, CmusikPlayer* pPlayer, 
	CmusikFunctor* pFunctor, bool bUpdatePlaylist, bool bAddToPlayer, bool bDeleteFilelist )
{
	m_Files					= pFiles;
	m_Playlist				= pPlaylist;
	m_Functor				= pFunctor;
	m_Library				= pLibrary;
	m_UpdatePlaylist		= bUpdatePlaylist;
	m_DeleteFilelist		= bDeleteFilelist;
	m_Player				= pPlayer;
	m_AddToPlayer			= bAddToPlayer;
	m_CallFunctorOnAbort	= false;
}

///////////////////////////////////////////////////

// CmusikBatchAddTask

///////////////////////////////////////////////////

CmusikBatchAddTask::CmusikBatchAddTask()
	: CmusikTask()
{
	m_Params = NULL;
	m_Type = MUSIK_TASK_TYPE_BATCHADD;
}

///////////////////////////////////////////////////

CmusikBatchAddTask::~CmusikBatchAddTask()
{
	if ( m_Params )
		delete m_Params;
}

///////////////////////////////////////////////////

int CmusikBatchAddTask::open( void* params )
{
	m_Params = (CmusikBatchAdd*)params;
	int ret_code = activate( THR_NEW_LWP | THR_JOINABLE );
	return ret_code;
}

///////////////////////////////////////////////////

int CmusikBatchAddTask::close( u_long flags )
{
	// clean up
	if ( m_Params->m_DeleteFilelist )
		delete m_Params->m_Files;

	m_Finished = true;

	if ( m_Params->m_Functor && ( ( m_Stop && m_Params->m_CallFunctorOnAbort ) || !m_Stop ) )
		m_Params->m_Functor->OnTaskEnd( this );
	
	return 0;
}

///////////////////////////////////////////////////

int CmusikBatchAddTask::svc()
{
	m_Stop = false;
	m_Finished = false;
	m_Active = true;

	size_t curr_prog = 0;
	size_t last_prog = 0;

	CmusikSong song;

	bool verify_failed = false;

	// wait until previous transaction has finished
	while ( m_Params->m_Library->GetOpenTransactions() )
		Sleep( 500 );

	m_Params->m_Library->BeginTransaction();
	m_Params->m_Library->InitTimeAdded();
	for( size_t i = 0; i < m_Params->m_Files->size(); i++ )
	{
		if ( m_Stop )
		{
			TRACE0( "CmusikBatchAdd worker function aborted, but ended gracefully\n" );
			break;
		}

		// add the song
		m_Params->m_Library->AddSong( m_Params->m_Files->at( i ) );

		// adding to now playing
		if ( m_Params->m_AddToPlayer && m_Params->m_Player && m_Params->m_Player->GetPlaylist() )
		{
			m_Params->m_Library->GetSongFromFilename( m_Params->m_Files->at( i ), song );
			m_Params->m_Player->GetPlaylist()->Add( song );
		}

		// adding to current playlist
		else if ( m_Params->m_UpdatePlaylist && m_Params->m_Playlist )
		{
			if ( !verify_failed )
			{
				if ( m_Params->m_Functor->VerifyPlaylist( (void*)m_Params->m_Playlist ) )
				{
					m_Params->m_Library->GetSongFromFilename( m_Params->m_Files->at( i ), song );
					m_Params->m_Playlist->Add( song );
				}
			}
			else
			{
				verify_failed = true;
				CmusikString s;
				s.Format( "Failed to add song to playlist at address %d becuase it couldn't be verified\n", m_Params->m_Playlist );
				TRACE0( s.c_str() );
			}
		}

		// post progress to the functor
		curr_prog = ( 100 * i ) / m_Params->m_Files->size();
		if ( curr_prog != last_prog )
		{
			if ( m_Params->m_Functor )
				m_Params->m_Functor->OnTaskProgress( curr_prog );
			last_prog = curr_prog;
		}

	}

	m_Params->m_Library->EndTransaction();
	return 0;
}

///////////////////////////////////////////////////
