///////////////////////////////////////////////////
// 
// Info:
//
//   Musik is a a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2003
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey@bak.rr.com
//   Dustin Carter  : Developer, pain in the ass
//   Simon Windmill : Developer, the most helpful person thats immediately available ;)
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Class(s): 
//
//   CMusikFilename
//
// Filename(s): 
//
//   MusikFilename.h, MusikFilename.cpp
//
// Information:
//
//   A crossplatform (Win32/Linux) way to extract certain
//   parts of a filename.
// 
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