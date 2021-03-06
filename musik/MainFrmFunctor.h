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
//   CmusikFrameFunctor
//
// Filename(s): 
//
//   MainFrmFunctor.h, MainFrmFunctor.cpp
//
// Information:
//
//   CmusikFunctor subclass that is used for communication between
//   the CmusikPlayer and CMainFrame.
// 
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "../musikCore/include/musikFunctor.h"

///////////////////////////////////////////////////

using namespace musik;

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

	virtual void OnNewEqualizer()
	{
		int WM_EQUALIZERCHANGE = RegisterWindowMessage( "EQUALIZERCHANGE" );
		m_Parent->PostMessage( WM_EQUALIZERCHANGE, NULL );
	}

	virtual void OnStop()
	{
		int WM_SONGSTOP = RegisterWindowMessage( "SONGSTOP" );
		m_Parent->PostMessage( WM_SONGSTOP, NULL );
	}

	virtual void OnPause()
	{
		int WM_SONGPAUSE = RegisterWindowMessage( "SONGPAUSE" );
		m_Parent->SendMessage( WM_SONGPAUSE, NULL );
	}

	virtual void OnResume()
	{
		int WM_SONGRESUME = RegisterWindowMessage( "SONGRESUME" );
		m_Parent->SendMessage( WM_SONGRESUME, NULL );
	}

	virtual void OnPlaybackFailed()
	{
		MessageBox( NULL, "Playback failed.\n\nThis needs to post an event to the main frame.", "musikCube", MB_ICONINFORMATION );
	}

private:
	CWnd* m_Parent;
};

///////////////////////////////////////////////////