///////////////////////////////////////////////////

#ifndef C_MUSIK_DOCKBAR_H
#define C_MUSIK_DOCKBAR_H

///////////////////////////////////////////////////

class CMusikPrefs;
class CMusikDockbarBtn;

///////////////////////////////////////////////////

#ifndef baseCMusikDockBar
	#define baseMusikDockBar CSizingControlBar
#endif

///////////////////////////////////////////////////

class CMusikDockBar : public baseMusikDockBar
{
public:

	// construct / destruct
    CMusikDockBar( CMusikPrefs* prefs = NULL, CMenu* menu = NULL  );
	~CMusikDockBar();

	// show / hide gripper
    virtual BOOL HasGripper() const;
	void ShowGripper( bool show = true );

protected:

	// overrides
	virtual void NcPaintGripper(CDC* pDC, CRect rcClient);
	virtual void NcCalcClient(LPRECT pRc, UINT nDockBarID);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
    afx_msg UINT OnNcHitTest(CPoint point);
    afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);

	// appearance
    int     m_cyGripper;
	bool	m_ShowGripper;
	BOOL    m_bActive;
    CString m_sFontFace;

    CMusikDockbarBtn* m_biHide;
	CMusikDockbarBtn* m_biOptions;

	// musik stuff
	CMusikPrefs* m_Prefs;
	CMenu* m_Menu;
	
	// mfc message maps
	afx_msg void OnNcPaint();

	// custom message maps
    afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);

	// macros
    DECLARE_DYNAMIC(CMusikDockBar)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

class CMusikDockbarBtn 
{
public:
    CMusikDockbarBtn( CMusikPrefs* prefs, char btn = 'r' );
	~CMusikDockbarBtn(){}

    void Move(CPoint ptTo) {ptOrg = ptTo; };
    CRect GetRect() { return CRect(ptOrg, CSize(11, 11)); };
    void Paint(CDC* pDC);

    BOOL    bPushed;
    BOOL    bRaised;

protected:

	CMusikPrefs* m_Prefs;

    CPoint  ptOrg;
	char	m_Btn;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////