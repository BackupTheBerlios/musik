///////////////////////////////////////////////////

#include "stdafx.h"

#include "../include/MusikPlayer.h"
#include "../include/MusikLibrary.h"
#include "../include/MusikFunctor.h"

///////////////////////////////////////////////////

CMusikPlayer::CMusikPlayer( CMusikFunctor* functor )
{
	m_IsPlaying = false;
	m_IsPaused	= false;
	m_Functor	= functor; 
}

///////////////////////////////////////////////////

CMusikPlayer::~CMusikPlayer()
{
}

///////////////////////////////////////////////////

bool CMusikPlayer::Play( int index )
{

	m_Functor->Call();

	return true;
}

///////////////////////////////////////////////////