///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CmusikPrefs;

///////////////////////////////////////////////////

class CmusikTrackCtrl : public CSliderCtrl
{
public:

	// construct / destruct
	CmusikTrackCtrl( CmusikPrefs* prefs = NULL );
	virtual ~CmusikTrackCtrl();

	// public mfc message maps
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);

protected:	
	
	// use this to get colors...
	CmusikPrefs* m_Prefs;

	// width / height of the channel
	int m_Channel;

	// drawing...
	void DrawChannel( CDC* pDC, const CRect& rect, BOOL bHoriz );
	void DrawHorizontalThumb( CDC* pDC, const CRect &rect );
	void DrawVerticalThumb( CDC* pDC, const CRect &rect );

	// message maps
	DECLARE_DYNAMIC(CmusikTrackCtrl)
	DECLARE_MESSAGE_MAP()
public:

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

///////////////////////////////////////////////////

