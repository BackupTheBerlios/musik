#pragma once

#define LCSB_CLIENTDATA 1
#define LCSB_NCOVERRIDE 2

//-------------------------------------------------//
//--- Definision of which is used as default.	---//
//-------------------------------------------------//
#define SB_HORZ             0
#define SB_VERT             1
#define SB_BOTH             3

class CMusikListCtrl : public CListCtrl
{
//-------------------------------------------------//
//--- implementation							---//
//-------------------------------------------------//
public:
	CMusikListCtrl();
	virtual ~CMusikListCtrl();
	void HideScrollBars(int Type, int Which=SB_BOTH);

protected:
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	DECLARE_MESSAGE_MAP()

private:
	BOOL NCOverride;
	int Who;
};