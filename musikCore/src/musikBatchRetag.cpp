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
//   CmusikBatchRetag.cpp
//
// Information:
//
//   A worker thread for updating tags 
//
// Usage: 
//
//   Create a new CmusikBatchRetagTask and a CmusikBatchRetag,
//   set the specified paramters, then run the task
//   sending the CmusikBatchRetag object as the argument.
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "../include/musikBatchRetag.h"
#include "../include/musikMp3Info.h"
#include "../include/musikOggInfo.h"

///////////////////////////////////////////////////

using namespace musik;

///////////////////////////////////////////////////

// CmusikBatchRetag

///////////////////////////////////////////////////

CmusikBatchRetag::CmusikBatchRetag()
{
	m_Functor = NULL;
	m_Library = NULL;
	m_UpdatedTags = NULL;
	m_CallFunctorOnAbort = false;
	m_DeleteUpdatedTags = true;
	m_WriteToFile = false;
}

///////////////////////////////////////////////////

CmusikBatchRetag::CmusikBatchRetag( CmusikLibrary* pLibrary, CmusikFunctor* pFunctor, CmusikSongInfoArray* pUpdatedTags )
{
	m_Functor = pFunctor;
	m_Library = pLibrary;
	m_UpdatedTags = pUpdatedTags;
	m_CallFunctorOnAbort = false;
	m_DeleteUpdatedTags = true;
	m_WriteToFile = false;
}

///////////////////////////////////////////////////

// CmusikBatchRetagTask

///////////////////////////////////////////////////

CmusikBatchRetagTask::CmusikBatchRetagTask()
	: CmusikTask()
{
	m_Params = NULL;
	m_Type = MUSIK_TASK_TYPE_BATCHRETAG;
}

///////////////////////////////////////////////////

CmusikBatchRetagTask::~CmusikBatchRetagTask()
{
	if ( m_Params )
		delete m_Params;
}

///////////////////////////////////////////////////

void CmusikBatchRetagTask::close()
{
	if ( m_Params->m_DeleteUpdatedTags )
		delete m_Params->m_UpdatedTags;

	m_Finished = true;

	if ( m_Params->m_Functor && ( !m_Stop || ( m_Stop && m_Params->m_CallFunctorOnAbort ) ) )
		m_Params->m_Functor->OnTaskEnd( this );
}

///////////////////////////////////////////////////

void CmusikBatchRetagTask::run()
{
	m_Stop = false;
	m_Finished = false;
	m_Active = true;

	size_t curr_prog = 0;
	size_t last_prog = 0;

	// wait until previous transaction has finished
	while ( m_Params->m_Library->GetOpenTransactions() )
		Sleep( 500 );

	m_Params->m_Library->BeginTransaction();
	int nFormat;
	for( size_t i = 0; i < m_Params->m_UpdatedTags->size(); i++ )
	{
		// check abort flag
		if ( m_Stop )
		{
			TRACE0( "musikBatchRetagWorker worker function aborted\n" );
			break;
		}

		bool success = false;	
		if ( m_Params->m_WriteToFile )
		{
			m_Params->m_Library->GetSongFormatFromID( m_Params->m_UpdatedTags->at( i ).GetID(), &nFormat );
			if ( nFormat == MUSIK_LIBRARY_FORMAT_MP3 )
				success = CmusikMp3Info::WriteInfo( m_Params->m_UpdatedTags->at( i ) );
			else if ( nFormat == MUSIK_LIBRARY_FORMAT_OGG )
				success = CmusikOggInfo::WriteInfo( m_Params->m_UpdatedTags->at( i ) );
		}
		else
			success = true;

		if ( success )
		{
			if ( !m_Params->m_WriteToFile )
				m_Params->m_UpdatedTags->at( i ).SetDirtyFlag( "1" );
			else
				m_Params->m_UpdatedTags->at( i ).SetDirtyFlag( "0" );

			m_Params->m_Library->SetSongInfo( &m_Params->m_UpdatedTags->at( i ) );
		}

		// post progress to the functor
		curr_prog = ( 100 * i ) / m_Params->m_UpdatedTags->size();
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
