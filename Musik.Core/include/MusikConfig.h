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
// Filename(s): 
//
//   MusikConfig.h,
//
// Information:
//
//   Some stl vector arrays for objects commonly used.
//   Using dynamic arrays helps to keep things easily
//   maintained.
//
///////////////////////////////////////////////////

#ifndef MUSIK_CONFIG_H
#define MUSIK_CONFIG_H

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
		#define TRACE0(x) printf(x)
	#endif

	#ifndef Sleep
		#define Sleep(x) sleep(x)
	#endif
#endif

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
