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

	// sets
	void SetFilename( const CStdString& fn );

private:

	CStdString m_Filename;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////