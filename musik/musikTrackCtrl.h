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
//   CmusikTrackCtrl
//
// Filename(s): 
//
//   musikTrackCtrl.h, musikTrackCtrl.cpp
//
// Information:
//
//   musikCube's custom slider control, based off of
//   CSliderCtrl
// 
// Usage: 
//
//   See CmusikVolumeCtrl, CmusikTimeCtrl
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CmusikPrefs;
class CmusikPlayer;

///////////////////////////////////////////////////

class CmusikTrackCtrl : public CSliderCtrl
{
public:

	// construct / destruct
	CmusikTrackCtrl( CmusikPrefs* prefs = NULL, CmusikPlayer* player = NULL );
	virtual ~CmusikTrackCtrl();

	// misc
	void SetLockIfNotPlaying( bool lock = true ){ m_LockIfNotPlaying = lock; }
	bool IsLockSet(){ return m_LockIfNotPlaying; }

	// public mfc message maps
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	// use this to get colors
	CmusikPrefs* m_Prefs;

protected:	

	// mouse tracking
	CPoint m_Cursor;
	int m_LastPos;
	bool m_LeftDown;
	bool m_IsCapturing;
	void SetPosFromMouse();
	virtual void OnPosChanged();
	virtual void OnBeginDrag();
	virtual void OnFinishDrag();

	// lock movement if no playback
	bool m_LockIfNotPlaying;

	// use this to determine if 
	// the slider should be locked
	CmusikPlayer* m_Player;

	// drawing...
	void DrawChannel( CDC* pDC, const CRect& rect, BOOL bHoriz );
	void DrawHorizontalThumb( CDC* pDC, const CRect &rect );
	void DrawVerticalThumb( CDC* pDC, const CRect &rect );

	// message maps
	DECLARE_DYNAMIC(CmusikTrackCtrl)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

