#pragma once


#include "stdafx.h"
#include "CIniReader/INI.h"

class CMusikPrefs
{
public:
	CMusikPrefs( CString filename );
	~CMusikPrefs();

protected:
	CIniReader* config;

private:
};