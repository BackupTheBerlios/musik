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
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

protected:	

	// mouse tracking
	CPoint m_Cursor;
	int m_LastPos;
	bool m_LeftDown;
	bool m_IsCapturing;
	void SetPosFromMouse();
	virtual void OnPosChanged();

	// use this to get colors...
	CmusikPrefs* m_Prefs;

	// drawing...
	void DrawChannel( CDC* pDC, const CRect& rect, BOOL bHoriz );
	void DrawHorizontalThumb( CDC* pDC, const CRect &rect );
	void DrawVerticalThumb( CDC* pDC, const CRect &rect );

	// message maps
	DECLARE_DYNAMIC(CmusikTrackCtrl)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

///////////////////////////////////////////////////

