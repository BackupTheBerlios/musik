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
//   CmusikVolumeCtrl
//
// Filename(s): 
//
//   musikVolumeCtrl.h, musikVolumeCtrl.cpp
//
// Information:
//
//   The volume adjustment slider in the "Now Playing" control.
// 
// Usage: 
//
//    Void.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikTrackCtrl.h"
#include "musikPrefs.h"

#include "../musikCore/include/musikPlayer.h"

///////////////////////////////////////////////////

using namespace musik;

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