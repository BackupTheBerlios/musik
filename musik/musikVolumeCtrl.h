///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikTrackCtrl.h"

///////////////////////////////////////////////////

class CmusikPrefs;
class CmusikPlayer;

///////////////////////////////////////////////////

class CmusikVolumeCtrl : public CmusikTrackCtrl
{
public:

	// construct and destruct
	CmusikVolumeCtrl( CmusikPrefs* prefs, CmusikPlayer* player );
	~CmusikVolumeCtrl();

private:

	// core related stuff
	CmusikPlayer* m_Player;

	// position changed
	virtual void OnPosChanged();

	// message maps
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	// macros
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////