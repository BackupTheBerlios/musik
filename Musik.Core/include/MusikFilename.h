///////////////////////////////////////////////////

#ifndef C_MUSIK_FILENAME_H
#define C_MUSIK_FILENAME_H

///////////////////////////////////////////////////

#include "StdString.h"

///////////////////////////////////////////////////

#ifdef WIN32
	#define MUSIK_PATH_SEPARATOR "\\"
#else
	#define MUSIK_PATH_SEPARATOR "/"
#endif

///////////////////////////////////////////////////

class CMusikFilename
{
public:

	// construct and destruct
	CMusikFilename();
	CMusikFilename( const CStdString& fn );
	~CMusikFilename();

	// gets
	CStdString GetPathSeparator();
	CStdString GetJustFilename();
	CStdString GetPath();
	CStdString GetFullFilename();
	CStdString GetExtension( bool make_lower = true );

	// sets
	void SetFilename( const CStdString& fn );

	// misc
	static bool FileExists( const CStdString& fn );
	static CStdString GetTempFilename( CStdString fn, bool check_exists );

private:

	CStdString m_Filename;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////