#pragma once


// CMusikPlaylistCtrl
class CMusikLibrary;

class CMusikPlaylistCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CMusikPlaylistCtrl)

public:
	CMusikPlaylistCtrl( CMusikLibrary* library );
	virtual ~CMusikPlaylistCtrl();

protected:
	CMusikLibrary* m_Library;
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnNcPaint();
};


