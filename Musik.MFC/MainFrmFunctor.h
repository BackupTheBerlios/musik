///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "MainFrm.h"

#include "../Musik.Core/include/MusikFunctor.h"

///////////////////////////////////////////////////

class CMainFrame;

///////////////////////////////////////////////////

class CMusikFrameFunctor : public CMusikFunctor
{
public:
	CMusikFrameFunctor( CWnd* parent)
	{ 
		m_Parent = parent; 
	}

	virtual void OnNewSong()
	{
		int WM_SONGCHANGE = RegisterWindowMessage( "SONGCHANGE" );
		m_Parent->PostMessage( WM_SONGCHANGE, NULL );
	}

	virtual void OnStop()
	{
		int WM_SONGSTOP = RegisterWindowMessage( "SONGSTOP" );
		m_Parent->PostMessage( WM_SONGSTOP, NULL );
	}

private:
	CWnd* m_Parent;
};

///////////////////////////////////////////////////