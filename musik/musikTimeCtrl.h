///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CmusikTrackCtrl;
class CmusikPrefs;
class CmusikPlayer;
class CmusikDynamicText;

///////////////////////////////////////////////////

class CmusikTimeCtrl : public CWnd
{
public:

	// construct and destruct
	CmusikTimeCtrl( CmusikPrefs* prefs = NULL, CmusikPlayer* player = NULL );
	virtual ~CmusikTimeCtrl();

	// message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);

protected:

	// 'core' stuff
	CmusikPrefs* m_Prefs;
	CmusikPlayer* m_Player;

	// our children...
	CmusikTrackCtrl* m_TimeCtrl;
	CmusikDynamicText* m_CurTime;
	CmusikDynamicText* m_TotalTime;
	void RescaleInfo( int cx );

	// timer
	UINT m_TimerID;

	// macros
	DECLARE_DYNAMIC(CmusikTimeCtrl)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

