///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CMusikNowPlayingCtrl : public CWnd
{
	DECLARE_DYNAMIC(CMusikNowPlayingCtrl)

public:
	CMusikNowPlayingCtrl();
	virtual ~CMusikNowPlayingCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};

///////////////////////////////////////////////////
