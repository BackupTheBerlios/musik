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
//   CmusikBatchAdd
//
// Filename(s): 
//
//   CmusikRemoveOld.h
//
// Information:
//
//   A worker thread for removing missing files 
//   from the library
//
// Usage: 
//
//   Create a new CmusikThread and a CmusikRemoveOld,
//   set the specified paramters, then run the thread
//   sending the CmusikRemoveOld object as the argument,
//   and musikRemoveOldWorker as the worker thread.
//
///////////////////////////////////////////////////

#include "musikLibrary.h"
#include "musikFunctor.h"
#include "musikPlaylist.h"

///////////////////////////////////////////////////

class CmusikRemoveOld
{
public: 

	// construct / destruct
	CmusikRemoveOld()
	{
		m_Functor = NULL;
		m_Library = NULL;
		m_CallFunctorOnAbort = false;
	}

	CmusikRemoveOld( CmusikLibrary* pLibrary, CmusikFunctor* pFunctor )
	{
		m_Functor = pFunctor;
		m_Library = pLibrary;
		m_CallFunctorOnAbort = false;
	}

	~CmusikRemoveOld()
	{
	}

	// we'll allow users to manually set
	// these variables.
	CmusikLibrary* m_Library;
	CmusikFunctor* m_Functor;
	bool m_CallFunctorOnAbort;
};

///////////////////////////////////////////////////

static void musikRemoveOldWorker( CmusikThread* thread )
{
	CmusikRemoveOld* params = (CmusikRemoveOld*)thread->GetArgs();

	size_t curr_prog = 0;
	size_t last_prog = 0;

	CStdStringArray all_files;
	params->m_Library->GetAllDistinct( MUSIK_LIBRARY_TYPE_FILENAME, all_files, false );

	// sleep if we go idle
	ACE_Time_Value sleep;
	sleep.set( 0.1f );

	bool verify_failed = false;

	params->m_Library->BeginTransaction();
	for( size_t i = 0; i < all_files.size(); i++ )
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
			TRACE0( "musikRemoveOldWorker worker function aborted...\n" );
			break;
		}

		// see if the file exists
		if ( !CmusikFilename::FileExists( all_files.at ( i ) ) )
			params->m_Library->RemoveSong( all_files.at ( i ) );

		// post progress to the functor
		curr_prog = ( 100 * i ) / all_files.size();
		if ( curr_prog != last_prog )
		{
			if ( params->m_Functor )
				params->m_Functor->OnThreadProgress( curr_prog );

			last_prog = curr_prog;
		}

	}
	params->m_Library->EndTransaction();

	// clean up
	if ( ( params->m_Functor && !thread->m_Abort ) || ( thread->m_Abort && params->m_CallFunctorOnAbort ) )
		params->m_Functor->OnThreadEnd( (void*)thread );

	delete params;

	// flag as finishe&d
	thread->m_Finished = true;
}

///////////////////////////////////////////////////