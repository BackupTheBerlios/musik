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
	CMusikPlaylistCtrl( CMusikLibrary* library, CMusikPlayer* player, CMusikPrefs* prefs, CMusikPlaylist* playlist );
	virtual ~CMusikPlaylistCtrl();

	// update the virtual list control
	void UpdateV();

	// set the playlist
	void SetPlaylist( CMusikPlaylist* playlist );

protected:

	// core stuff
	CMusikLibrary* m_Library;
	CMusikPlayer* m_Player;
	CMusikPrefs* m_Prefs;
	CMusikPlaylist* m_Playlist;
	CMusikDynDspInfo* m_SongInfoCache;

	// fonts
	void InitFonts();
	CFont m_Bullets;
	CFont m_Items;

	// misc
	bool m_Changed;
	int m_RatingWidth;

	// message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnOdcachehint(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult);

	// macros
	DECLARE_DYNAMIC(CMusikPlaylistCtrl)
	DECLARE_MESSAGE_MAP()

private:

	// playlist column stuff
	void ResetColumns();
	void SaveColumns();
	CString GetRating( int item );
	CString GetTimeStr( int item );


};

///////////////////////////////////////////////////

