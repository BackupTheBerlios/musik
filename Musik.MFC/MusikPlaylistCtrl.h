#pragma once


// CMusikPlaylistCtrl

class CMusikPlaylistCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CMusikPlaylistCtrl)

public:
	CMusikPlaylistCtrl();
	virtual ~CMusikPlaylistCtrl();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnNcPaint();
};


