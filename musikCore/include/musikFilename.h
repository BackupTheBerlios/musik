///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
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
// Class(es): 
//
//   CmusikFilename
//
// Filename(s): 
//
//   musikFilename.h, musikFilename.cpp
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

#include "musikConfig.h"

///////////////////////////////////////////////////

#ifdef WIN32
	#define MUSIK_PATH_SEPARATOR "\\"
#else
	#define MUSIK_PATH_SEPARATOR "/"
#endif

///////////////////////////////////////////////////

class CmusikFilename
{
public:

	// construct and destruct
	CmusikFilename();
	CmusikFilename( const CmusikString& fn );
	~CmusikFilename();

	// gets
	CmusikString GetPathSeparator();
	CmusikString GetJustFilename();
	CmusikString GetPath();
	CmusikString GetFullFilename();
	CmusikString GetExtension( bool make_lower = true );

	// sets
	void SetFilename( const CmusikString& fn );

	// misc
	static bool FileExists( const CmusikString& fn );
	static CmusikString GetTempFilename( CmusikString fn, bool check_exists );

private:

	CmusikString m_Filename;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
