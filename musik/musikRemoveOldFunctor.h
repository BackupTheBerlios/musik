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
		TRACE0( "CmusikRemoveOldFunctor thread started...\n" );
	}

	virtual void OnThreadEnd( void* thr_addr = NULL )
	{
		TRACE0( "CmusikRemoveOldFunctor thread complete...\n" );

		int WM_REMOVEOLD_END = RegisterWindowMessage( "REMOVEOLD_END" );
		m_Parent->PostMessage( WM_REMOVEOLD_END, (WPARAM)thr_addr );		
	}

	virtual void OnThreadProgress( size_t progress )
	{
		CStdString s;
		s.Format( "CmusikRemoveOld %d%% complete.\n", progress );
		TRACE0( s.c_str() );
	}

private:

	CWnd* m_Parent;
};

///////////////////////////////////////////////////