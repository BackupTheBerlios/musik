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
//   Create a new CmusikBatchRetagTask and a CmusikBatchRetag,
//   set the specified paramters, then run the task
//   sending the CmusikBatchRetag object as the argument.
//
///////////////////////////////////////////////////

#include "musikArrays.h"
#include "musikLibrary.h"
#include "musikFunctor.h"
#include "musikMP3Info.h"
#include "musikOggInfo.h"

#include "ace/Task.h"

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

class CmusikBatchRetagTask : public CmusikTask
{

public:

	CmusikBatchRetagTask()
		: CmusikTask()
	{
		m_Params = NULL;
		m_Type = MUSIK_TASK_TYPE_BATCHRETAG;
	}

	~CmusikBatchRetagTask()
	{
		if ( m_Params )
			delete m_Params;
	}

	int open( void* params )
	{
		m_Params = (CmusikBatchRetag*)params;
		
#ifdef WIN32
	int ret_code = activate( THR_NEW_LWP | THR_JOINABLE | THR_USE_AFX );
#else 
	int ret_code = activate( THR_NEW_LWP | THR_JOINABLE );
#endif

		return ret_code;
	}

	int close( u_long flags )
	{
		if ( m_Params->m_DeleteUpdatedTags )
			delete m_Params->m_UpdatedTags;

		m_Finished = true;

		if ( m_Params->m_Functor && ( !m_Stop || ( m_Stop && m_Params->m_CallFunctorOnAbort ) ) )
			m_Params->m_Functor->OnTaskEnd( this );

		return 0;
	}

	int svc()
	{
		m_Stop = false;
		m_Finished = false;
		m_Active = true;

		size_t curr_prog = 0;
		size_t last_prog = 0;

		// wait until previous transaction has finished
		ACE_Time_Value sleep_t;
		sleep_t.set( 0.5 );
		while ( m_Params->m_Library->GetOpenTransactions() )
			ACE_OS::sleep( sleep_t );

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

		return 0;
	}

private:

	CmusikBatchRetag* m_Params;

};

///////////////////////////////////////////////////
