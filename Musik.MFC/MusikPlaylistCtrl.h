///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "MusikListCtrl.h"

#include "../Musik.Core/include/StdString.h"
#include "../Musik.Core/include/MusikFunctor.h"
#include "../Musik.Core/include/MusikDir.h"

///////////////////////////////////////////////////

class CMusikLibrary;
class CMusikPlayer;
class CMusikPrefs;
class CMusikPlaylist;
class CMusikDynDspInfo;
class CMusikBatchAdd;
class CMusikBatchAddFunctor;
class CMusikDir;

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

	// for some reason creating a CMusikDir
	// object during a drop operation causes
	// it to fail. i can't imagine why, its 
	// just initializing a few variables to 
	// NULL in the constructor. at any rate,
	// its created here...
	CMusikDir m_Dir;

	// pointer to main window
	CFrameWnd* m_MainWnd;

	// core stuff
	CMusikLibrary* m_Library;
	CMusikPlayer* m_Player;
	CMusikPrefs* m_Prefs;

	// playlist stuff
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
