#include "stdafx.h"

#include "MusikPrefs.h"

CMusikPrefs::CMusikPrefs( CString filename )
{
	config = new CIniFile( filename.GetBuffer() );

	if ( !config->ReadFile() )
		config->WriteFile();

	LoadPrefs();
}

CMusikPrefs::~CMusikPrefs()
{
	delete config;
}

void CMusikPrefs::LoadPrefs()
{
	//-----------------------------------------------------//
	//--- selection area								---//
	//-----------------------------------------------------//
	m_SelectionBox_Count = StringToInt( config->GetValue( "Selection Area", "Count", "2" ) );

	//-----------------------------------------------------//
	//--- playlist										---//
	//-----------------------------------------------------//

	//-----------------------------------------------------//
	//--- sources										---//
	//-----------------------------------------------------//
	SavePrefs();
}

void CMusikPrefs::SavePrefs()
{
	//-----------------------------------------------------//
	//--- selection area								---//
	//-----------------------------------------------------//
	config->SetValue( "Selection Area", "Count", IntToString( m_SelectionBox_Count ) );

	//-----------------------------------------------------//
	//--- playlist										---//
	//-----------------------------------------------------//

	//-----------------------------------------------------//
	//--- sources										---//
	//-----------------------------------------------------//

	config->WriteFile();
}
