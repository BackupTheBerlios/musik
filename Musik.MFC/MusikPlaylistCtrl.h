#pragma once

#include "MusikListCtrl.h"

class CMusikLibrary;
class CMusikPrefs;
class CMusikPlaylist;
class CMusikDynDspInfo;

class CMusikPlaylistCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CMusikPlaylistCtrl)

//-------------------------------------------------//
//--- implementation							---//
//-------------------------------------------------//
public:
	CMusikPlaylistCtrl( CMusikLibrary* library, CMusikPrefs* prefs, CMusikPlaylist* playlist );
	virtual ~CMusikPlaylistCtrl();

	void UpdateV();

protected:
	DECLARE_MESSAGE_MAP()

//-------------------------------------------------//
//--- variables									---//
//-------------------------------------------------//
protected:
	CMusikLibrary* m_Library;
	CMusikPrefs* m_Prefs;
	CMusikPlaylist* m_Playlist;
	CFont m_Bullets;
	CFont m_Items;

	CMusikDynDspInfo* m_SongInfoCache;

//-------------------------------------------------//
//--- operations								---//
//-------------------------------------------------//
protected:
	void InitFonts();

private:
	void ResetColumns();
	void SaveColumns();
	CString GetRating( int item );
	
//-------------------------------------------------//
//--- message maps								---//
//-------------------------------------------------//
public:
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnOdcachehint(NMHDR *pNMHDR, LRESULT *pResult);
};


