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
// Class(s): 
//
//   CmusikPlaylistCtrl
//
// Filename(s): 
//
//   musikPlaylistCtrl.h, musikPlaylistCtrl.cpp
//
// Information:
//
//   musikCube's playlist control, based on a CmusikListCtrl
// 
// Usage: 
//
//   The control is virtual and fairly specialized, read
//   the header file for more information.
//
// Some code adapted from:
//
//   Michael Dunn: http://www.codeproject.com/shell/explorerdragdrop.asp?target=drag%7C%2Bdrop
//
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
	void UpdateV( bool redraw = true );

	// returns TRUE if user has deleted, inserted,
	// or otherwise modified a playlist that may
	// need to be saved...
	bool PlaylistNeedsSave(){ return m_PlaylistNeedsSave; }

	// get the playlist
	CmusikPlaylist* GetPlaylist(){ return m_Playlist; }

	// set the playlist
	void SetPlaylist( CmusikPlaylist* playlist, int type );

	// save the current playlist back to the library..
	// this will only work on standard playlists. will
	// return false if non standard playlist
	void SavePlaylist( bool check_prompt = true );

	// used by the main UI to trigger an "item activate"
	// event..
	bool PlayItem( int n = -1 );

protected:

	// is dnd active
	UINT m_DropID;

	// is a column being arranged?
	bool m_Arranging;

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

	// function the custom header control
	// will use as a callback after a column
	// has been moved
	virtual void OnDragColumn( int source, int dest );

	// fonts
	void InitFonts();
	CFont m_StarFont;
	CFont m_ItemFont;
	CFont m_BoldFont;

	// rating ***** text extent
	void GetRatingExtent();
	int m_RatingExtent;
	int m_TwoSpace;

	// colors
	COLORREF clrStripe;
	COLORREF clrStripText;

	// manipulating items..
	bool m_PlaylistNeedsSave;
	void GetSelectedItems( CIntArray* items );
	void GetItemIDs( const CIntArray& items, CIntArray* target );
	void DeleteItems( const CIntArray& items, bool update = true );
	void InsertItems( const CIntArray& items, int first_sel, int at, bool update = true );
	int GetFirstSelected();

	// misc
	bool m_Changed;

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
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLvnMarqueeBegin(NMHDR *pNMHDR, LRESULT *pResult);

	// CmusikPlaylistDropTarget calls
	// this function once files have
	// been dropped...
	bool m_DropArrange;
	void OnDropFiles(HDROP hDropInfo);

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
};

///////////////////////////////////////////////////
