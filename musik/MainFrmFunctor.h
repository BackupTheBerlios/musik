///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "MainFrm.h"

#include "../musikCore/include/musikFunctor.h"

///////////////////////////////////////////////////

class CMainFrame;

///////////////////////////////////////////////////

class CmusikFrameFunctor : public CmusikFunctor
{
public:
	CmusikFrameFunctor( CWnd* parent)
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

	virtual void OnPause()
	{
		int WM_SONGPAUSERESUME = RegisterWindowMessage( "SONGPAUSERESUME" );
		m_Parent->PostMessage( WM_SONGPAUSERESUME, NULL );
	}

	virtual void OnResume()
	{
		int WM_SONGPAUSERESUME = RegisterWindowMessage( "SONGPAUSERESUME" );
		m_Parent->PostMessage( WM_SONGPAUSERESUME, NULL );
	}

	virtual void OnPlaybackFailed()
	{
		MessageBox( NULL, "Playback failed.\n\nThis needs to post an event to the main frame.", "musikCube", MB_ICONINFORMATION );
	}

private:
	CWnd* m_Parent;
};

///////////////////////////////////////////////////