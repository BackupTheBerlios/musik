#include "stdafx.h"

#include "MusikPrefs.h"

CMusikPrefs::CMusikPrefs( CString filename )
{
	config = new CIniFile( filename.GetBuffer() );

	if ( !config->ReadFile() )
		config->WriteFile();

	m_Filename = filename;

	LoadPrefs();
}

CMusikPrefs::~CMusikPrefs()
{
	delete config;
}

void CMusikPrefs::LoadPrefs()
{
	m_SelectionBox_Count = StringToInt( config->GetValue( "Selection Area", "Count", "2" ) );

	SavePrefs();
}

void CMusikPrefs::SavePrefs()
{
	config->SetValue( "Selection Area", "Count", IntToString( m_SelectionBox_Count ) );

	config->WriteFile();
}
