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

#include "../include/musikLibrary.h"
#include "../include/musikPlayer.h"
#include "../include/musikFunctor.h"
#include "../include/musikPlaylist.h"

///////////////////////////////////////////////////

class CmusikLibrary;
class CmusikPlaylist;
class CmusikFunctor;
class CmusikPlayer;

///////////////////////////////////////////////////

class CmusikBatchAdd
{
public: 

	// construct / destruct
	CmusikBatchAdd()
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

	CmusikBatchAdd( CStdStringArray* pFiles, CmusikPlaylist* pPlaylist, CmusikLibrary* pLibrary, CmusikPlayer* pPlayer, 
		CmusikFunctor* pFunctor, bool bUpdatePlaylist = false, bool bAddToPlayer = false, bool bDeleteFilelist = true )
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

	~CmusikBatchAdd();

	// we'll allow users to manually set
	// these variables.
	CStdStringArray* m_Files;
	CmusikPlaylist* m_Playlist;
	CmusikLibrary* m_Library;
	CmusikPlayer* m_Player;
	CmusikFunctor* m_Functor;
	bool m_UpdatePlaylist;
	bool m_DeleteFilelist;
	bool m_AddToPlayer;				// negates UpdatePlaylist() and m_Playlist()... will add files to player's playlist
	bool m_CallFunctorOnAbort;
};

///////////////////////////////////////////////////

static void musikBatchAddWorker( CmusikThread* thread )
{
	CmusikBatchAdd* params = (CmusikBatchAdd*)thread->GetArgs();

	size_t curr_prog = 0;
	size_t last_prog = 0;

	CmusikSong song;

	// sleep if we go idle
	ACE_Time_Value sleep;
	sleep.set( 0.1f );

	bool verify_failed = false;

	params->m_Library->BeginTransaction();
	for( size_t i = 0; i < params->m_Files->size(); i++ )
	{
		if ( thread->IsSuspended() )
		{
			thread->m_Asleep = true;
			while ( thread->IsSuspended() )
				ACE_OS::sleep( sleep );
			thread->m_Asleep = false;
		}

		if ( thread->m_Abort )
		{
			TRACE0( "CmusikBatchAdd worker function aborted...\n" );
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
			if ( !verify_failed )
			{
				if ( params->m_Functor->VerifyPlaylist( (void*)params->m_Playlist ) )
				{
					params->m_Library->GetSongFromFilename( params->m_Files->at( i ), song );
					params->m_Playlist->Add( song );
				}
			}
			else
			{
				verify_failed = true;
				CStdString s;
				s.Format( "Failed to add song to playlist at address %d becuase it couldn't be verified\n", params->m_Playlist );
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


	// delete thread
	// trigger functor (to delete file list)
	if ( params->m_DeleteFilelist )
		delete params->m_Files;

	if ( ( thread->m_Abort && params->m_CallFunctorOnAbort ) || !thread->m_Abort )
		params->m_Functor->OnThreadEnd( (void*)thread );

	thread->m_Finished = true;
}

///////////////////////////////////////////////////