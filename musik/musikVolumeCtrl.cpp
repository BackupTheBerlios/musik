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

#include "stdafx.h"

#include "musikVolumeCtrl.h"

///////////////////////////////////////////////////

CmusikVolumeCtrl::CmusikVolumeCtrl( CmusikPrefs* prefs, CmusikPlayer* player )
	: CmusikTrackCtrl( prefs )
{
	m_Player = player;
}

///////////////////////////////////////////////////

CmusikVolumeCtrl::~CmusikVolumeCtrl()
{

}

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CmusikVolumeCtrl, CmusikTrackCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()

///////////////////////////////////////////////////

int CmusikVolumeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ( CmusikTrackCtrl::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	SetRange( 0, 255 );

	if ( m_Prefs )
		SetPos( 255 - m_Prefs->GetPlayerVolume() );

	return 0;
}

///////////////////////////////////////////////////

void CmusikVolumeCtrl::OnPosChanged()
{
	if ( m_Player )
	{
		m_Player->SetVolume( m_Player->GetCurrChannel(), 255 - GetPos() );
		m_Player->SetMaxVolume( 255 - GetPos() );
	}

	if ( m_Prefs )
		m_Prefs->SetPlayerVolume( 255 - GetPos() );
}

///////////////////////////////////////////////////

