#pragma once


// CMusikPlaylistCtrl
class CMusikLibrary;
class CMusikPrefs;

class CMusikPlaylistCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CMusikPlaylistCtrl)

public:
	CMusikPlaylistCtrl( CMusikLibrary* library, CMusikPrefs* prefs );
	virtual ~CMusikPlaylistCtrl();

protected:
	CMusikLibrary* m_Library;
	CMusikPrefs* m_Prefs;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnNcPaint();
};


