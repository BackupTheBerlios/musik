///////////////////////////////////////////////////

#include "stdafx.h"

#include "musikVolumeCtrl.h"
#include "musikPrefs.h"

#include "../musikCore/include/musikPlayer.h"

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

