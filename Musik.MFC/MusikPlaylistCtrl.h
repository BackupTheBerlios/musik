#pragma once


// CMusikPlaylistCtrl
class CMusikLibrary;
class CMusikPrefs;
class CMusikPlaylist;

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

//-------------------------------------------------//
//--- operations								---//
//-------------------------------------------------//
private:
	void EnableHighlighting( HWND hWnd, int row, bool bHighlight );
	bool IsRowSelected( HWND hWnd, int row );
	void ResetColumns();
	void SaveColumns();
	
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
};


