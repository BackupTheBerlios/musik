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
//   CmusikEqualizerBar, CmusikEqualizerCtrl
//
// Filename(s): 
//
//   musikEqualizerCtrl.h, musikEqualizertrl.cpp
//
// Information:
//
//   musikCube's equalizer control.
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikDockBar.h"
#include "musikTrackCtrl.h"
#include "musikPrefs.h"
#include "musikEqualizerSets.h"
#include "musikTrackCtrl.h"

#include "../musikCore/include/musikLibrary.h"
#include "../musikCore/include/musikPlayer.h"

///////////////////////////////////////////////////

class CmusikEqualizerCtrl;

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

	// getting and setting equalizer
	void GetActiveEqualizer( CmusikEQSettings* settings );
	void SetActiveEqualizer( const CmusikEQSettings& settings );

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
	afx_msg void OnEqualizerResetdefault();
	afx_msg void OnEqualizerAutomaticallySave();
	afx_msg void OnEqualizerEnabled();

	// custom message maps
	afx_msg LRESULT OnClosePresets( WPARAM wParam, LPARAM lParam );

	// macros
	DECLARE_DYNAMIC(CmusikEqualizerBar)
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEqualizerCloseView();
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
	friend class CmusikEqualizerBar;

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
	void ResetDefault();

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
	void SetBandsFrom( const CmusikEQSettings& settings );
	void BandsToEQSettings( CmusikEQSettings* settings );

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


