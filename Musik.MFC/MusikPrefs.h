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
	CSize	GetDlgSize	()						{ return m_DlgSize; }
	CPoint	GetDlgPos	()						{ return m_DlgPos; }

	void	SetDlgSize	( const CSize size )	{ m_DlgSize = size; }
	void	SetDlgPos	( const CPoint pt )		{ m_DlgPos = pt; }

	//-----------------------------------------------------//
	//--- selection area								---//
	//-----------------------------------------------------//
	int GetSelBoxCount(){ return m_SelectionBox_Count; }

	//-----------------------------------------------------//
	//--- playlist										---//
	//-----------------------------------------------------//

	//-----------------------------------------------------//
	//--- sources										---//
	//-----------------------------------------------------//

protected:
	CIniFile* config;

private:
	int m_SelectionBox_Count;
	CSize m_DlgSize;
	CPoint m_DlgPos;
};