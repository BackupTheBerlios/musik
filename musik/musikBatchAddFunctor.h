///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "MainFrm.h"

#include "../musikCore/include/musikFunctor.h"

///////////////////////////////////////////////////

class CmusikBatchAddFunctor : public CmusikFunctor
{
public:

	CmusikBatchAddFunctor( CWnd* parent)
	{ 
		m_Parent = parent; 
	}

	virtual void OnThreadStart()
	{
		TRACE0( "CmusikBatchAdd thread started...\n" );
	}

	virtual void OnThreadEnd()
	{
		TRACE0( "CmusikBatchAdd thread complete...\n" );

		int WM_BATCHADD_END = RegisterWindowMessage( "BATCHADD_END" );
		m_Parent->PostMessage( WM_BATCHADD_END );		
	}

	virtual void OnThreadProgress( size_t progress )
	{
		CStdString s;
		s.Format( "CmusikBatchAdd %d%% complete.\n", progress );
		TRACE0( s.c_str() );
	}

private:
	CWnd* m_Parent;
};

///////////////////////////////////////////////////