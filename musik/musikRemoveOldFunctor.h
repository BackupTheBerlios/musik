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

#include "MainFrm.h"

#include "../musikCore/include/musikFunctor.h"

///////////////////////////////////////////////////

class CmusikRemoveOldFunctor : public CmusikFunctor
{
public:

	CmusikRemoveOldFunctor( CWnd* parent)
	{ 
		m_Parent = parent; 
	}

	virtual void OnThreadStart()
	{
		TRACE0( "CmusikRemoveOldFunctor thread started\n" );
	}

	virtual void OnThreadEnd( void* thr_addr = NULL )
	{
		TRACE0( "CmusikRemoveOldFunctor thread complete\n" );

		int WM_REMOVEOLD_END = RegisterWindowMessage( "REMOVEOLD_END" );
		m_Parent->PostMessage( WM_REMOVEOLD_END, (WPARAM)thr_addr );		
	}

	virtual void OnThreadProgress( size_t progress )
	{
		CmusikString s;
		s.Format( "CmusikRemoveOld %d%% complete.\n", progress );
		TRACE0( s.c_str() );
	}

private:

	CWnd* m_Parent;
};

///////////////////////////////////////////////////