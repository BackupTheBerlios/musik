#include "stdafx.h"

#include "MusikPrefs.h"

CMusikPrefs::CMusikPrefs( CString filename )
{
	config = new CIniReader( filename );
	VerifyFile( filename );
	LoadPrefs();
}

CMusikPrefs::~CMusikPrefs()
{
    delete config;
}

void CMusikPrefs::LoadPrefs()
{
	m_SelectionBox_Count = CStringToInt( config->getKeyValue( "Selection Area", "Count", "4" ) );
}

void CMusikPrefs::SavePrefs()
{
	config->setKey( "Selection Area", "Count", IntToCString( GetSelBoxCount() ) );
}

void CMusikPrefs::VerifyFile( CString filename )
{
	CStdioFile file;
	file.Open( filename, CFile::modeCreate | CFile::modeRead, NULL );
	file.Close();
}