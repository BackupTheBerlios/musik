#pragma once

#include "stdafx.h"
#include "CIniEx/iniFile.h"

using namespace std;

class CMusikPrefs
{
public:
	CMusikPrefs( CString filename );
	~CMusikPrefs();

	void LoadPrefs();
	void SavePrefs();
	//-----------------------------------------------------//
	//--- whole dialog									---//
	//-----------------------------------------------------//
	CSize	GetDlgSize	()						{ return m_Dlg_Size; }
	CPoint	GetDlgPos	()						{ return m_Dlg_Pos; }

	void	SetDlgSize	( const CSize size )	{ m_Dlg_Size = size; }
	void	SetDlgPos	( const CPoint pt )		{ m_Dlg_Pos = pt; }

	//-----------------------------------------------------//
	//--- selection area								---//
	//-----------------------------------------------------//
	int		GetSelBoxCount()					{ return m_SelectionBox_Count; }

	void	SetSelBoxCount( size_t n )			{ m_SelectionBox_Count = n; }

	//-----------------------------------------------------//
	//--- playlist										---//
	//-----------------------------------------------------//

	//-----------------------------------------------------//
	//--- sources										---//
	//-----------------------------------------------------//

	//-----------------------------------------------------//
	//--- now playing									---//
	//-----------------------------------------------------//
	int		GetNowPlayingHeight()				{ return m_NowPlaying_Height; }

protected:
	CIniFile* config;

private:
	size_t m_SelectionBox_Count;

	CSize m_Dlg_Size;
	CPoint m_Dlg_Pos;

	int m_NowPlaying_Height;
};