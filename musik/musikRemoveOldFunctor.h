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
//   CmusikFunctor subclass that is used for CmusikRemoveOldTasks
//   to communicate back to CMainFrame.
// 
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "../musikCore/include/musikFunctor.h"

///////////////////////////////////////////////////

using namespace musik;

///////////////////////////////////////////////////

class CmusikRemoveOldFunctor : public CmusikFunctor
{
public:

	CmusikRemoveOldFunctor( CWnd* parent)
	{ 
		m_Parent = parent; 
	}

	virtual void OnTaskStart()
	{
		TRACE0( "CmusikRemoveOldFunctor task started\n" );
	}

	virtual void OnTaskEnd( CmusikTask* task_addr = NULL )
	{
		TRACE0( "CmusikRemoveOldFunctor task complete\n" );

		int WM_REMOVEOLD_END = RegisterWindowMessage( "REMOVEOLD_END" );
		m_Parent->PostMessage( WM_REMOVEOLD_END, (WPARAM)task_addr );		
	}

	virtual void OnTaskProgress( size_t progress )
	{
		CmusikString s;
		s.Format( "CmusikRemoveOld %d%% complete.\n", progress );
		TRACE0( s.c_str() );
	}

private:

	CWnd* m_Parent;
};

///////////////////////////////////////////////////