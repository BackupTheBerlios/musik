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

#define HTOPTIONS ( HTCLOSE + 1 )

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

	// custom message maps
    afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);

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
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
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