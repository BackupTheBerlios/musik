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
//   CmusikRemoveOld.cpp
//
// Information:
//
//   A worker thread for removing missing files 
//   from the library
//
// Usage: 
//
//   Create a new CmusikRemoveOldTask and a CmusikRemoveOld,
//   set the specified paramters, then run the task
//   sending the CmusikRemoveOld object as the argument.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "../include/musikRemoveOld.h"
#include "../include/musikPlaylist.h"
#include "../include/musikFilename.h"

///////////////////////////////////////////////////

using namespace musik;

///////////////////////////////////////////////////

using namespace musik;

///////////////////////////////////////////////////

// CmusikRemoveOld

///////////////////////////////////////////////////

CmusikRemoveOld::CmusikRemoveOld()
{
	m_Functor = NULL;
	m_Library = NULL;
	m_CallFunctorOnAbort = false;
}

///////////////////////////////////////////////////

CmusikRemoveOld::CmusikRemoveOld( CmusikLibrary* pLibrary, CmusikFunctor* pFunctor )
{
	m_Functor = pFunctor;
	m_Library = pLibrary;
	m_CallFunctorOnAbort = false;
}

///////////////////////////////////////////////////

// CmusikRemoveOldTask

///////////////////////////////////////////////////

CmusikRemoveOldTask::CmusikRemoveOldTask()
	: CmusikTask()
{
	m_Params = NULL;
	m_Type = MUSIK_TASK_TYPE_REMOVEOLD;
}

///////////////////////////////////////////////////

CmusikRemoveOldTask::~CmusikRemoveOldTask()
{
	if ( m_Params )
		delete m_Params;
}

///////////////////////////////////////////////////

void CmusikRemoveOldTask::close()
{
	m_Finished = true;

	if ( ( m_Params->m_Functor && !m_Stop ) || ( m_Stop && m_Params->m_CallFunctorOnAbort ) )
		m_Params->m_Functor->OnTaskEnd( this );
}

///////////////////////////////////////////////////

void CmusikRemoveOldTask::run()
{
	m_Stop = false;
	m_Finished = false;
	m_Active = true;

	size_t curr_prog = 0;
	size_t last_prog = 0;

	CmusikStringArray all_files;
	m_Params->m_Library->GetAllDistinct( MUSIK_LIBRARY_TYPE_FILENAME, all_files, false );

	bool verify_failed = false;

	// wait until previous transaction has finished
	while ( m_Params->m_Library->GetOpenTransactions() )
		Sleep( 500 );

	m_Params->m_Library->BeginTransaction();
	for( size_t i = 0; i < all_files.size(); i++ )
	{
		// check abort flag
		if ( m_Stop )
		{
			TRACE0( "musikRemoveOldWorker worker function aborted\n" );
			break;
		}

		// see if the file exists
		if ( !CmusikFilename::FileExists( all_files.at ( i ) ) )
			m_Params->m_Library->RemoveSong( all_files.at ( i ) );

		// post progress to the functor
		curr_prog = ( 100 * i ) / all_files.size();
		if ( curr_prog != last_prog )
		{
			if ( m_Params->m_Functor )
				m_Params->m_Functor->OnTaskProgress( curr_prog );

			last_prog = curr_prog;
		}

	}
	m_Params->m_Library->EndTransaction();
	close();
}

///////////////////////////////////////////////////
