#pragma once


#include "stdafx.h"
#include "CIniReader/INI.h"

#define CStringToInt atoi

inline CString IntToCString( int n )
{
	char buffer[20];
	itoa( n, buffer, 10 );
	return (CString)buffer;
}

class CMusikPrefs
{
public:
	CMusikPrefs( CString filename );
	~CMusikPrefs();

	void LoadPrefs();
	void SavePrefs();

	int GetSelBoxCount(){ return m_SelectionBox_Count; }

protected:
	void VerifyFile( CString filename );
	CIniReader* config;

private:
	int m_SelectionBox_Count;
};