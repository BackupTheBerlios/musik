///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikListCtrl.h"

#include "../musikCore/include/StdString.h"
#include "../musikCore/include/musikFunctor.h"
#include "../musikCore/include/musikDir.h"

///////////////////////////////////////////////////

class CmusikLibrary;
class CmusikPlayer;
class CmusikPrefs;
class CmusikPlaylist;
class CmusikDynDspInfo;
class CmusikBatchAdd;
class CmusikBatchAddFunctor;
class CmusikDir;

///////////////////////////////////////////////////

class CmusikPlaylistCtrl : public CmusikListCtrl
{

	friend class CmusikPlaylistDropTarget;

public:

	// construct and destruct
	CmusikPlaylistCtrl( CFrameWnd* mainwnd, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs, UINT dropid );
	virtual ~CmusikPlaylistCtrl();

	// update the virtual list control
	void InitColors();
	void UpdateV();

	// get the playlist
	CmusikPlaylist* GetPlaylist(){ return m_Playlist; }

	// set the playlist
	void SetPlaylist( CmusikPlaylist* playlist, int type );

protected:

	// is dnd active
	UINT m_DropID;

	// for some reason creating a CmusikDir
	// object during a drop operation causes
	// it to fail. i can't imagine why, its 
	// just initializing a few variables to 
	// NULL in the constructor. at any rate,
	// its created here...
	CmusikDir m_Dir;

	// pointer to main window
	CFrameWnd* m_MainWnd;

	// core stuff
	CmusikLibrary* m_Library;
	CmusikPlayer* m_Player;
	CmusikPrefs* m_Prefs;

	// playlist stuff
	CmusikPlaylist* m_NowPlaying;
	CmusikDynDspInfo* m_SongInfoCache;

	CmusikPlaylist* m_Playlist;
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

	// CmusikPlaylistDropTarget calls
	// this function once files have
	// been dropped...
	void OnDropFiles(HDROP hDropInfo);

	// custom message maps
	afx_msg LRESULT OnBatchAddProgress( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnBatchAddEnd( WPARAM wParam, LPARAM lParam );

	// macros
	DECLARE_DYNAMIC(CmusikPlaylistCtrl)
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
	CmusikBatchAdd* m_BatchAddThr;
	CmusikBatchAddFunctor* m_BatchAddFnct;
public:
	afx_msg void OnLvnMarqueeBegin(NMHDR *pNMHDR, LRESULT *pResult);
};

///////////////////////////////////////////////////
