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
// Class(es)
//
//   CmusikString
//
// Filename(s): 
//
//   musikConfig.h
//
// Information:
//
//   Some stl vector arrays for objects commonly used.
//   Using dynamic arrays helps to keep things easily
//   maintained.
//
//   A CmusikString is a CStdString, renamed becuase 
//   I think it looks prettier.
//
///////////////////////////////////////////////////

#ifndef MUSIK_CONFIG_H
#define MUSIK_CONFIG_H

///////////////////////////////////////////////////

#include "StdString.h"
#include <vector>

///////////////////////////////////////////////////

namespace musik {

///////////////////////////////////////////////////

typedef CStdString CmusikString;	
typedef std::vector<CmusikString> CmusikStringArray;
typedef std::vector<int> CIntArray;

///////////////////////////////////////////////////

#define MUSIK_USE_UNICODE 1

///////////////////////////////////////////////////

// stuff that windows uses but 
// may not be exactly the same
// with gcc
#ifndef WIN32
	#if !MUSIK_USE_UNICODE
		#define _T(x) x
	#else
		#define _T(x) L ## x
	#endif

	#ifndef TRACE0
		#define TRACE0(x) cout << x
	#endif

	#ifndef Sleep
		#define Sleep(x) sleep(x)
	#endif

	#ifndef ASSERT
		#define ASSERT(x) assert(x)
	#endif
#endif

///////////////////////////////////////////////////

inline static char *ItoA( char *buff, unsigned int i )
{
	if ( i > 9 ) 
		buff = ItoA( buff, i / 10 );
	
	*(buff++) = '0' + (i % 10);
	return buff;
}

///////////////////////////////////////////////////

inline char *int2str( char *buff, int i )
{
	if ( i < 0 )
	{
		*buff = '-';
		*ItoA(buff+1,-i) = 0;
	}
	else    
		*ItoA(buff,i) = 0;
	
	return buff;
}

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
