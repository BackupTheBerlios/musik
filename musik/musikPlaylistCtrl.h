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

#include "musikPrefs.h"

#include "../musikCore/include/musikLibrary.h"
#include "../musikCore/include/musikPlayer.h"
#include "../musikCore/include/musikBatchAdd.h"
#include "../musikCore/include/musikFunctor.h"
#include "../musikCore/include/musikDir.h"
#include "../musikCore/include/musikDynDspInfo.h"

///////////////////////////////////////////////////

using namespace musik;

///////////////////////////////////////////////////

class CmusikPlaylistInfoCtrl;

///////////////////////////////////////////////////

class CmusikPlaylistCtrl : public CmusikListCtrl
{
	friend class CmusikPlaylistDropTarget;

public:

	// construct and destruct
	CmusikPlaylistCtrl(){}
	CmusikPlaylistCtrl( CFrameWnd* mainwnd, CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs, UINT dropid_l, UINT dropid_r );
	virtual ~CmusikPlaylistCtrl();

	// update the virtual list control
	void InitColors();
	void UpdateV( bool redraw = true, bool select_none = false );
	void ScrollToCurr();
	void ResyncItem( size_t songid, int item = -1 );

	// stuff dealing with the playlist header
	void ShowContextMenu();
	void ShowHeaderMenu();
	int GetColumnPos( int field );
	void AddColumn( int field, bool refresh_cols = true );
	void RemoveColumn( int at, bool refresh_cols = true );
	void ToggleColumn ( int field );

	// call when new playlist is set becuase the
	// previous playlist's sorting should not apply
	// to the current one's...
	void HideSortArrow();
    
	// returns TRUE if user has deleted, inserted,
	// or otherwise modified a playlist that may
	// need to be saved...
	bool PlaylistNeedsSave(){ return m_PlaylistNeedsSave; }
	CmusikPlaylist* GetPlaylist(){ return m_Playlist; }
	void SetPlaylist( CmusikPlaylist* playlist );
	void SavePlaylist( bool check_prompt = true );

	// used by the main UI to trigger an "item activate"
	// event..
	bool PlayItem( int n = -1 );

	void SetInfoCtrl( CmusikPlaylistInfoCtrl *ctrl );

protected:

	// is dnd active
	UINT m_DropID_L;
	UINT m_DropID_R;

	// internal play routines
	bool PlayItem_AddNP( int n = -1 );
	bool PlayItem_ReplaceNP( int n = -1 );

	// for some reason creating a CmusikDir
	// object during a drop operation causes
	// it to fail. i can't imagine why, its 
	// just initializing a few variables to 
	// NULL in the constructor. at any rate,
	// its created here...
	CmusikDir m_Dir;

	// pointer to main window
	CFrameWnd* m_Parent;

	// pointer to info ctrl
	CmusikPlaylistInfoCtrl* m_InfoCtrl;

	// core stuff
	CmusikLibrary* m_Library;
	CmusikPlayer* m_Player;
	CmusikPrefs* m_Prefs;

	// playlist stuff
	CmusikPlaylist* m_NowPlaying;
	CmusikDynDspInfo* m_SongInfoCache;

	CmusikPlaylist* m_Playlist;

	// drag and drop functions
	virtual void OnDragColumn( int source, int dest );
	void BeginDrag( NMHDR* pNMHDR, bool right_button );
	bool m_Arranging;

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
	void GetSelectedItems( CIntArray& items );
	void GetSelectedSongs( CmusikPlaylist& songs );
	void GetItemIDs( const CIntArray& items, CIntArray* target );
	void DeleteItems( const CIntArray& items, bool update = true );
	void InsertItems( CmusikPlaylist& items, int first_sel, int at, bool update = true );
	int GetFirstSelected();
	void DeleteSelectedItems( bool from_library = false, bool from_computer = false );

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
	afx_msg void OnLvnMarqueeBegin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPlaylistcolumnsArtist();
	afx_msg void OnLvnBeginrdrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPlaylistcolumnsAlbum();
	afx_msg void OnPlaylistcolumnsYear();
	afx_msg void OnPlaylistcolumnsGenre();
	afx_msg void OnPlaylistcolumnsTitle();
	afx_msg void OnPlaylistcolumnsTracknumber();
	afx_msg void OnPlaylistcolumnsTimeadded();
	afx_msg void OnPlaylistcolumnsLastplayed();
	afx_msg void OnPlaylistcolumnsFilesize();
	afx_msg void OnPlaylistcolumnsFormat();
	afx_msg void OnPlaylistcolumnsDuration();
	afx_msg void OnPlaylistcolumnsRating();
	afx_msg void OnPlaylistcolumnsTimesplayed();
	afx_msg void OnPlaylistcolumnsBitrate();
	afx_msg void OnPlaylistcolumnsFilename();
	afx_msg void OnLvnKeydown(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPlcDeleteFromplaylist();
	afx_msg void OnPlcDeleteFromlibrary();
	afx_msg void OnPlcDeleteFromcomputer();
	afx_msg void OnPlaylistcontextmenuShufflenowplaying();

	// CmusikPlaylistDropTarget calls
	// this function once files have
	// been dropped...
	bool m_DropArrange;
	void OnDropFiles(HDROP hDropInfo);

	// macros
	DECLARE_DYNAMIC(CmusikPlaylistCtrl)
	DECLARE_MESSAGE_MAP()

private:

	// misc
	bool UseTempTable();

	// playlist column stuff
	void ResetColumns( bool update = true );
	void SaveColumns();
	CString GetRating( int item );
	CString GetTimeStr( int item );

	// dnd stuff
	bool m_IsWinNT;
	size_t m_ClipboardFormat;

	// current sorted column
	void DrawSortArrow( int col );
	int m_LastCol;
	bool m_Ascend;
};

///////////////////////////////////////////////////
