///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "MusikListCtrl.h"

#include "../Musik.Core/include/StdString.h"
#include "../Musik.Core/include/MusikFunctor.h"

///////////////////////////////////////////////////

class CMusikLibrary;
class CMusikPlayer;
class CMusikPrefs;
class CMusikPlaylist;
class CMusikDynDspInfo;
class CMusikBatchAdd;
class CMusikBatchAddFunctor;

///////////////////////////////////////////////////

class CMusikPlaylistCtrl : public CMusikListCtrl
{

	friend class CMusikPlaylistDropTarget;

public:

	// construct and destruct
	CMusikPlaylistCtrl( CFrameWnd* mainwnd, CMusikLibrary* library, CMusikPlayer* player, CMusikPrefs* prefs, UINT dropid );
	virtual ~CMusikPlaylistCtrl();

	// update the virtual list control
	void InitColors();
	void UpdateV();

	// get the playlist
	CMusikPlaylist* GetPlaylist(){ return m_Playlist; }

	// set the playlist
	void SetPlaylist( CMusikPlaylist* playlist, int type );

protected:

	// is dnd active
	UINT m_DropID;

	// pointer to main window
	CFrameWnd* m_MainWnd;

	// core stuff
	CMusikLibrary* m_Library;
	CMusikPlayer* m_Player;
	CMusikPrefs* m_Prefs;

	// playlist stuff
	void CleanNowPlaying();
	CMusikPlaylist* m_NowPlaying;
	CMusikDynDspInfo* m_SongInfoCache;

	CMusikPlaylist* m_Playlist;
	int m_PlaylistType;

	// fonts
	void InitFonts();
	CFont m_StarFont;
	CFont m_ItemFont;
	CFont m_BoldFont;

	// colors
	COLORREF clrStripe;
	COLORREF clrStripText;

	// misc
	bool m_Changed;
	int m_RatingWidth;

	// mfc message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnOdcachehint(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);

	// CMusikPlaylistDropTarget calls
	// this function once files have
	// been dropped...
	void OnDropFiles(HDROP hDropInfo);

	// custom message maps
	afx_msg LRESULT OnBatchAddProgress( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnBatchAddEnd( WPARAM wParam, LPARAM lParam );

	// macros
	DECLARE_DYNAMIC(CMusikPlaylistCtrl)
	DECLARE_MESSAGE_MAP()

private:

	// playlist column stuff
	void ResetColumns();
	void SaveColumns();
	CString GetRating( int item );
	CString GetTimeStr( int item );

	// dnd stuff
	bool m_IsWinNT;
	size_t m_ClipboardFormat;

	// batch add files thread
	CMusikBatchAdd* m_BatchAddThr;
	CMusikBatchAddFunctor* m_BatchAddFnct;

	// gives the player ownership of
	// the current playlist, then
	// creates a blank one in its place
	// for the UI to add to
	void GivePlaylistToPlayer();
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