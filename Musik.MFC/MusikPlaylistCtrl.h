///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "MusikListCtrl.h"

///////////////////////////////////////////////////

class CMusikLibrary;
class CMusikPlayer;
class CMusikPrefs;
class CMusikPlaylist;
class CMusikDynDspInfo;

///////////////////////////////////////////////////

class CMusikPlaylistCtrl : public CListCtrl
{
public:

	// construct and destruct
	CMusikPlaylistCtrl( CFrameWnd* mainwnd, CMusikLibrary* library, CMusikPlayer* player, CMusikPrefs* prefs );
	virtual ~CMusikPlaylistCtrl();

	// update the virtual list control
	void InitColors();
	void UpdateV();

	// get the playlist
	CMusikPlaylist* GetPlaylist(){ return m_Playlist; }

	// set the playlist
	void SetPlaylist( CMusikPlaylist* playlist );

protected:

	// pointer to main window
	CFrameWnd* m_MainWnd;

	// core stuff
	CMusikLibrary* m_Library;
	CMusikPlayer* m_Player;
	CMusikPrefs* m_Prefs;

	// playlist stuff
	void CleanNowPlaying();
	CMusikPlaylist* m_Playlist;
	CMusikPlaylist* m_NowPlaying;
	CMusikDynDspInfo* m_SongInfoCache;

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
};

///////////////////////////////////////////////////