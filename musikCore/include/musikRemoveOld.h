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
//   CmusikRemoveOld.h
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

class CmusikRemoveOldTask : public CmusikTask
{

public:

	CmusikRemoveOldTask()
		: CmusikTask()
	{
		m_Type = MUSIK_TASK_TYPE_REMOVEOLD;
	}

	int open( void* params )
	{
		m_Params = (CmusikRemoveOld*)params;
		int ret_code = activate( THR_NEW_LWP | THR_JOINABLE | THR_USE_AFX );

		return ret_code;
	}

	int svc()
	{
		m_Stop = false;
		m_Finished = false;
		m_Active = true;

		size_t curr_prog = 0;
		size_t last_prog = 0;

		CmusikStringArray all_files;
		m_Params->m_Library->GetAllDistinct( MUSIK_LIBRARY_TYPE_FILENAME, all_files, false );

		bool verify_failed = false;

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

		// clean up
		CmusikFunctor* functor = m_Params->m_Functor;
		bool call_functor_abort = m_Params->m_CallFunctorOnAbort;

		delete m_Params;
		m_Finished = true;

		if ( ( functor && !m_Stop ) || ( m_Stop && call_functor_abort ) )
			functor->OnTaskEnd( this );

		return 0;
	}

private:

	CmusikRemoveOld* m_Params;

};

///////////////////////////////////////////////////
