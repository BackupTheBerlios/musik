#pragma once


#include "stdafx.h"
#include "CIniReader/INI.h"

class CMusikPrefs
{
public:
	CMusikPrefs( CString filename );
	~CMusikPrefs();

	void LoadPrefs();
	void SavePrefs();

protected:
	CIniReader* config;

private:


};