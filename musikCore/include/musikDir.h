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
//   CmusikDir
//
// Filename(s): 
//
//   musikDir.h
//
// Information:
//
//   Cross platform (win32 / linux) directory traverser
// 
// Usage: 
//
//   Construct then call Run()
//
///////////////////////////////////////////////////

#ifndef C_MUSIK_DIR_H
#define C_MUSIK_DIR_H

///////////////////////////////////////////////////

#include "musikConfig.h"
#include "musikFunctor.h"

#ifndef WIN32
	#include <dirent.h>
#endif

///////////////////////////////////////////////////

class CmusikDir
{
public:

	// construct and destruct
	CmusikDir();
	CmusikDir( CmusikString dir, CmusikStringArray* target = NULL );
	~CmusikDir();

	// the few functions we have
	void SetTarget( CmusikStringArray* target ){ m_Target = target;	}
	void Run();

	// values we need
	CmusikStringArray* m_Target;
	CmusikString m_Dir;

private:

	void OpenDir( CmusikString dir, CmusikStringArray* target = NULL );

};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
