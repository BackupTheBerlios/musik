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
//   CmusikBatchAddFunctor
//
// Filename(s): 
//
//   musikBatchAddFunctor.h, musikBatchAddFunctor.cpp
//
// Information:
//
//   CmusikFunctor subclass that is used for CmusikBatchAddTasks
//   to communicate back to CMainFrame.
// 
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "../musikCore/include/musikFunctor.h"

///////////////////////////////////////////////////

using namespace musik;

///////////////////////////////////////////////////

class CmusikBatchAddFunctor : public CmusikFunctor
{
public:

	CmusikBatchAddFunctor( CWnd* parent)
	{ 
		m_Parent = parent; 
	}

	virtual void OnTaskStart()
	{
		TRACE0( "CmusikBatchAdd task started\n" );
	}

	virtual void OnTaskEnd( CmusikTask* task_addr = NULL )
	{
		TRACE0( "CmusikBatchAdd task complete\n" );

		int WM_BATCHADD_END = RegisterWindowMessage( "BATCHADD_END" );
		m_Parent->PostMessage( WM_BATCHADD_END, (WPARAM)task_addr );	
	}

	virtual void OnTaskProgress( size_t progress )
	{
		CmusikString s;
		s.Format( "CmusikBatchAdd %d%% complete.\n", progress );
		TRACE0( s.c_str() );
	}

	virtual bool VerifyPlaylist( void* pl_addr = NULL )
	{
		if ( !pl_addr )
			return false;

		int WM_BATCHADD_VERIFY_PLAYLIST = RegisterWindowMessage( "BATCHADD_VERIFY_PLAYLIST" );
		LRESULT res = m_Parent->SendMessage( WM_BATCHADD_VERIFY_PLAYLIST, (WPARAM)pl_addr );

		if ( res == 1L )
			return true;
		else
			return false;
	}

private:

	CWnd* m_Parent;
};

///////////////////////////////////////////////////