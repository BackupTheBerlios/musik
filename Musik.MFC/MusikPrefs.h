#pragma once

#include "stdafx.h"
#include "CIniReader/iniFile.h"

using namespace std;

inline int StringToInt( const string str )
{
	return atoi( str.c_str() );	
}


inline string IntToString( int n )
{
	char buffer[20];
	itoa( n, buffer, 10 );
	string str = buffer;
    return str;	
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
	CIniFile* config;

private:
	CString m_Filename;
	int m_SelectionBox_Count;
};