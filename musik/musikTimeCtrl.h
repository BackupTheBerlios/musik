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

	// misc
	void OnNewSong();

	// size
	void SetSize( const CSize& size );

	// message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnTrackChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnTrackBegin( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnTrackFinish( WPARAM wParam, LPARAM lParam );

protected:

	// 'core' stuff
	CmusikPrefs* m_Prefs;
	CmusikPlayer* m_Player;

	// our children...
	CmusikTrackCtrl* m_TimeCtrl;
	CmusikDynamicText* m_CurTime;
	CmusikDynamicText* m_TotalTime;
	void RescaleInfo( int cx );
	size_t m_CurChars;
	size_t m_TotalChars;

	// size
	CSize m_Size;
	CSize m_CapSize;

	// we seeking playback time?
	bool m_TimeDrag;

	// timer
	UINT m_TimerID;

	// macros
	DECLARE_DYNAMIC(CmusikTimeCtrl)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

