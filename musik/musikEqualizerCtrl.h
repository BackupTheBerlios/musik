///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikDockBar.h"
#include "musikTrackCtrl.h"

///////////////////////////////////////////////////

class CmusikEqualizerCtrl;
class CmusikLibrary;
class CmusikPlayer;
class CmusikPrefs;
class CmusikDockBar;
class CmusikEQSettings;
class CmusikEqualizer;
class CmusikEqualizerSets;

///////////////////////////////////////////////////

#ifndef baseCmusikEqualizerBar
	#define baseCmusikEqualizerBar CmusikDockBar
#endif

///////////////////////////////////////////////////

// CmusikEqualizerBar

///////////////////////////////////////////////////

class CmusikEqualizerBar : public baseCmusikEqualizerBar
{
	friend class CmusikEqualizerSets;

public:

	// construct / destruct
	CmusikEqualizerBar( CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs );
	virtual ~CmusikEqualizerBar();

	// virtual
	virtual void OnOptions();

	// gets
	CmusikEqualizerCtrl* GetCtrl(){ return m_wndChild; }

protected:

	// children
	CmusikEqualizerCtrl* m_wndChild;

	// child's font
	CFont m_Font;

	// presets dialog
	CmusikEqualizerSets* m_Presets;

	// message maps
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEqualizerPresets();
	afx_msg void OnEqualizerLockchannels();
	afx_msg void OnEqualizerState18band();
	afx_msg void OnEqualizerState6band();
	afx_msg void OnEqualizerSetAsDefault();

	// custom message maps
	afx_msg LRESULT OnClosePresets( WPARAM wParam, LPARAM lParam );

	// macros
	DECLARE_DYNAMIC(CmusikEqualizerBar)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

// CmusikEqualizerCtrl

///////////////////////////////////////////////////

enum 
{
	MUSIK_EQUALIZER_CTRL_18BANDS = 18,
	MUSIK_EQUALIZER_CTRL_6BANDS = 6
};

///////////////////////////////////////////////////

class CmusikEqualizerCtrl : public CWnd
{
	friend class CmusikEqualizerSets;

public:

	// construct and destruct
	CmusikEqualizerCtrl( CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs );
	virtual ~CmusikEqualizerCtrl();

	// band state
	void SetBandState( int band_state );
	int GetBandState(){ return m_BandState; }
	void LayoutNewState();

	// channel lock
	void SetChannelsLocked( bool locked );
	bool IsChannelsLocked(){ return m_ChannelsLocked; }

	// actions
	void LoadCurrSong();
	void LoadDefault();
	void SetAsDefault();
	void DisplayChanged();

	// message maps
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	// custom message maps
	afx_msg LRESULT OnBandChange( WPARAM wParam, LPARAM lParam );

protected:

	// status
	bool m_ChannelsLocked;
	int m_BandState;

	// eq -> bands, bands -> eq
	void BandsFromEQSettings( const CmusikEQSettings& settings );
	void EQSettingsFromBands( CmusikEQSettings* settings );

	// core
	CmusikLibrary* m_Library;
	CmusikPlayer* m_Player;
	CmusikPrefs* m_Prefs;

	// controls
	CmusikTrackCtrl m_LeftBands[18];
	CmusikTrackCtrl m_RightBands[18];

	// macros
	DECLARE_DYNAMIC(CmusikEqualizerCtrl)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////


