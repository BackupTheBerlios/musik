#include "stdafx.h"

#include "MusikPrefs.h"

CMusikPrefs::CMusikPrefs( CString filename )
{
	config = new CIniReader( filename );
}

CMusikPrefs::~CMusikPrefs()
{
    delete config;
}