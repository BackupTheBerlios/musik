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

	// custom message maps
	afx_msg LRESULT OnClosePresets( WPARAM wParam, LPARAM lParam );

	// macros
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

// CmusikEqualizerCtrl

///////////////////////////////////////////////////

class CmusikEqualizerCtrl : public CWnd
{
	friend class CmusikEqualizerSets;

public:

	// construct and destruct
	CmusikEqualizerCtrl( CmusikLibrary* library, CmusikPlayer* player, CmusikPrefs* prefs );
	virtual ~CmusikEqualizerCtrl();

	// message maps
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

protected:

	// core
	CmusikLibrary* m_Library;
	CmusikPlayer* m_Player;
	CmusikPrefs* m_Prefs;

	// controls
	CmusikTrackCtrl m_LeftBands[16];
	CmusikTrackCtrl m_RightBands[16];

	// macros
	DECLARE_DYNAMIC(CmusikEqualizerCtrl)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////


