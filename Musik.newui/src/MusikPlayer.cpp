#include "wx/wxprec.h"
#include "MusikPlayer.h"

CMusikPlayer::CMusikPlayer( wxEvtHandler* parent )
	: wxThread( wxTHREAD_JOINABLE )
{
	m_Parent = parent;
}

CMusikPlayer::~CMusikPlayer()
{
}