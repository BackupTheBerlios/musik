#pragma once


// CMusikPlaylistCtrl
class CMusikLibrary;
class CMusikPrefs;

class CMusikPlaylistCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CMusikPlaylistCtrl)

//-------------------------------------------------//
//--- implementation							---//
//-------------------------------------------------//
public:
	CMusikPlaylistCtrl( CMusikLibrary* library, CMusikPrefs* prefs );
	virtual ~CMusikPlaylistCtrl();

protected:
	DECLARE_MESSAGE_MAP()

//-------------------------------------------------//
//--- variables									---//
//-------------------------------------------------//
protected:
	CMusikLibrary* m_Library;
	CMusikPrefs* m_Prefs;

//-------------------------------------------------//
//--- operations								---//
//-------------------------------------------------//
private:
	void ResetColumns();
	
//-------------------------------------------------//
//--- message maps								---//
//-------------------------------------------------//
public:
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


