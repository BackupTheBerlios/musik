///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CMusikNowPlayingCtrl : public CWnd
{


public:

	// construct and destruct
	CMusikNowPlayingCtrl();
	virtual ~CMusikNowPlayingCtrl();

protected:

	// message maps
	afx_msg void OnPaint();

	// macros
	DECLARE_DYNAMIC(CMusikNowPlayingCtrl)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
