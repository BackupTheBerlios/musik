///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "MusikPlaylistCtrl.h"

#include "../Musik.Core/include/MusikFunctor.h"

///////////////////////////////////////////////////

class CMusikLibrary;
class CMusikPlayer;
class CMusikPrefs;
class CMusikPlaylist;
class CMusikBatchAdd;

class CMusikBatchAddFunctor;

///////////////////////////////////////////////////

class CMusikPlaylistView : public CWnd
{
public:

	// construct and destruct
	CMusikPlaylistView( CFrameWnd* parent, CMusikLibrary* library, CMusikPlayer* player, CMusikPrefs* prefs );
	virtual ~CMusikPlaylistView();

	// get the playlist control
	CMusikPlaylistCtrl* GetCtrl(){ return m_Playlist; }

protected:

	// playlist GUI object
	CMusikPlaylistCtrl* m_Playlist;

	// a pointer to the library and player,
	// so we can use it in file dropped operations
	CMusikLibrary* m_Library;
	CMusikPlayer* m_Player;
	CMusikPrefs* m_Prefs;

	// pointer to main window so we can
	// messages to it, such as updating the UI
	// after files have been dropped
	CWnd* m_Parent;
	
	// mfc message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDropFiles(HDROP hDropInfo);

	// custom message maps
	afx_msg LRESULT OnBatchAddProgress( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnBatchAddEnd( WPARAM wParam, LPARAM lParam );

	// macros
	DECLARE_DYNAMIC(CMusikPlaylistView)
	DECLARE_MESSAGE_MAP()

private:

	// batch add files thread
	CMusikBatchAdd* m_BatchAddThr;
	CMusikBatchAddFunctor* m_BatchAddFnct;
};

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