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
//   CmusikDockBar
//
// Filename(s): 
//
//   musikDockBar.h, musikDockBar.cpp
//
// Information:
//
//   A CSizingControlBar specifically adpated for the
//   musikCube UI
// 
// Usage: 
//
//   Same as a CSizingControlBar... see URL below
//
// Some code adapted from:
//
//   Cristi Posea: http://www.datamekanix.com/sizecbar/
//
///////////////////////////////////////////////////

#ifndef C_MUSIK_DOCKBAR_H
#define C_MUSIK_DOCKBAR_H

///////////////////////////////////////////////////

class CmusikPrefs;
class CmusikDockbarBtn;

///////////////////////////////////////////////////

#ifndef baseCmusikDockBar
	#define basemusikDockBar CSizingControlBar
#endif

///////////////////////////////////////////////////

#define HTOPTIONS ( HTCLOSE + 56 )

///////////////////////////////////////////////////

class CmusikDockBar : public basemusikDockBar
{
public:

	// construct / destruct
    CmusikDockBar( CmusikPrefs* prefs = NULL, CMenu* menu = NULL  );
	~CmusikDockBar();

	// override default create method
    virtual BOOL Create( LPCTSTR lpszWindowName, CWnd* pParentWnd,
        UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP );

	// setting size
	void ForceDockedSize( const CSize& size, int orientation, bool layout = false );
	void ForceFloatSize( const CSize& size );
	void Layout(){ CalcDynamicLayout( NULL, LM_VERTDOCK | LM_HORZDOCK ); }

	// show / hide gripper
    virtual BOOL HasGripper() const;
	void ShowGripper( bool show = true );

protected:

	// mfc message maps and overrides
	virtual void NcPaintGripper(CDC* pDC, CRect rcClient);
	virtual void NcCalcClient(LPRECT pRc, UINT nDockBarID);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
    afx_msg UINT OnNcHitTest(CPoint point);
    afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	// custom message maps
    afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);

	// virtual function to call when the 
	// option button is pressed
	virtual void OnOptions();

	// appearance
    int     m_cyGripper;
	bool	m_ShowGripper;
	BOOL    m_bActive;
    CString m_sFontFace;

	// gripper icons
	CmusikDockbarBtn* m_biHide;
	CmusikDockbarBtn* m_biOptions;

	// musik stuff
	CmusikPrefs* m_Prefs;
	CMenu* m_Menu;
	
	// macros
    DECLARE_DYNAMIC(CmusikDockBar)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

class CmusikDockbarBtn 
{
public:
    CmusikDockbarBtn( CmusikPrefs* prefs, char btn = 'r' );
	~CmusikDockbarBtn(){}

    void Move(CPoint ptTo) {ptOrg = ptTo; };
    CRect GetRect() { return CRect(ptOrg, CSize(11, 11)); };
    void Paint(CDC* pDC);

    BOOL    bPushed;
    BOOL    bRaised;

protected:

	CmusikPrefs* m_Prefs;

    CPoint  ptOrg;
	char	m_Btn;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////