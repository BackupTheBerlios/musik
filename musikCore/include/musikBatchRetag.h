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
//   CmusikBatchRetag
//
// Filename(s): 
//
//   CmusikBatchRetag.h
//
// Information:
//
//   A worker thread for updating tags 
//
// Usage: 
//
//   Create a new CmusikThread and a CmusikBatchRetag,
//   set the specified paramters, then run the thread
//   sending the CmusikBatchRetag object as the argument,
//   and musikBatchRetagWorker as the worker thread.
//
///////////////////////////////////////////////////

#include "musikArrays.h"
#include "musikLibrary.h"
#include "musikFunctor.h"
#include "musikMP3Info.h"
#include "musikOggInfo.h"

///////////////////////////////////////////////////

class CmusikBatchRetag
{
public: 

	// construct / destruct
	CmusikBatchRetag()
	{
		m_Functor = NULL;
		m_Library = NULL;
		m_UpdatedTags = NULL;
		m_CallFunctorOnAbort = false;
		m_DeleteUpdatedTags = true;
		m_WriteToFile = false;
	}

	CmusikBatchRetag( CmusikLibrary* pLibrary, CmusikFunctor* pFunctor, CmusikSongInfoArray* pUpdatedTags )
	{
		m_Functor = pFunctor;
		m_Library = pLibrary;
		m_UpdatedTags = pUpdatedTags;
		m_CallFunctorOnAbort = false;
		m_DeleteUpdatedTags = true;
		m_WriteToFile = false;
	}

	~CmusikBatchRetag()
	{
	}

	// we'll allow users to manually set
	// these variables.
	CmusikLibrary* m_Library;
	CmusikFunctor* m_Functor;
	CmusikSongInfoArray* m_UpdatedTags;
	bool m_CallFunctorOnAbort;
	bool m_DeleteUpdatedTags;
	bool m_WriteToFile;
};

///////////////////////////////////////////////////

static void musikBatchRetagWorker( CmusikThread* thread )
{
	CmusikBatchRetag* params = (CmusikBatchRetag*)thread->GetArgs();

	size_t curr_prog = 0;
	size_t last_prog = 0;

	// sleep if we go idle
	ACE_Time_Value sleep;
	sleep.set( 0.1f );

	params->m_Library->BeginTransaction();
	int nFormat;
	for( size_t i = 0; i < params->m_UpdatedTags->size(); i++ )
	{
		// sleep if we're told
		if ( thread->IsSuspended() )
		{
			thread->m_Asleep = true;

			while ( thread->IsSuspended() )
				ACE_OS::sleep( sleep );

			thread->m_Asleep = false;
		}

		// check abort flag
		if ( thread->m_Abort )
		{
			TRACE0( "musikBatchRetagWorker worker function aborted...\n" );
			break;
		}

		bool success = false;	
		if ( params->m_WriteToFile )
		{
			params->m_Library->GetSongFormatFromID( params->m_UpdatedTags->at( i ).GetID(), &nFormat );
			if ( nFormat == MUSIK_LIBRARY_FORMAT_MP3 )
				success = CmusikMp3Info::WriteInfo( params->m_UpdatedTags->at( i ) );
			else if ( nFormat == MUSIK_LIBRARY_FORMAT_OGG )
				success = CmusikOggInfo::WriteInfo( params->m_UpdatedTags->at( i ) );
		}
		else
			success = true;

		if ( success )
		{
			if ( !params->m_WriteToFile )
				params->m_UpdatedTags->at( i ).SetDirtyFlag( "1" );

			params->m_Library->SetSongInfo( &params->m_UpdatedTags->at( i ) );
		}

		// post progress to the functor
		curr_prog = ( 100 * i ) / params->m_UpdatedTags->size();
		if ( curr_prog != last_prog )
		{
			if ( params->m_Functor )
				params->m_Functor->OnThreadProgress( curr_prog );

			last_prog = curr_prog;
		}

	}
	params->m_Library->EndTransaction();

	// clean up
	if ( params->m_DeleteUpdatedTags )
		delete params->m_UpdatedTags;

	if ( params->m_Functor && ( !thread->m_Abort || ( thread->m_Abort && params->m_CallFunctorOnAbort ) ) )
		params->m_Functor->OnThreadEnd( (void*)thread );

	delete params;

	// flag as finished
	thread->m_Finished = true;
}

///////////////////////////////////////////////////