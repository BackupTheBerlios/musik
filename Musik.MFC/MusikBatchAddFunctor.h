///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "MainFrm.h"

#include "../Musik.Core/include/MusikFunctor.h"

///////////////////////////////////////////////////

class CMusikBatchAddFunctor : public CMusikFunctor
{
public:

	CMusikBatchAddFunctor( CWnd* parent)
	{ 
		m_Parent = parent; 
	}

	virtual void OnThreadStart()
	{
		TRACE0( "CMusikBatchAdd thread started...\n" );
	}

	virtual void OnThreadEnd()
	{
		TRACE0( "CMusikBatchAdd thread complete...\n" );

		int WM_BATCHADD_END = RegisterWindowMessage( "BATCHADD_END" );
		m_Parent->PostMessage( WM_BATCHADD_END );		
	}

	virtual void OnThreadProgress( size_t progress )
	{
		CStdString s;
		s.Format( "CMusikBatchAdd %d%% complete.\n", progress );
		TRACE0( s.c_str() );
	}

private:
	CWnd* m_Parent;
};

///////////////////////////////////////////////////