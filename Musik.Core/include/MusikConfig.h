#ifndef MUSIK_CONFIG_H
#define MUSIK_CONFIG_H

#define MUSIK_USE_UNICODE 1

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
