#ifndef C_MUSIK_UTF8_H
#define C_MUSIK_UTF8_H

#include "wx/wxprec.h"

inline wxCharBuffer wxStringToMB( const wxString& s )
{
	return wxConvLocal.cWX2MB( s );
}

inline wxString MBTowxString( const char* pchar )
{
	return wxConvLocal.cMB2WX( pchar );
}

inline int wxStringToInt( const wxString &str )
{
	long lRet;
	str.ToLong( &lRet );
	return (int)lRet;
}

inline int StringToInt( const char* pChar )
{
	return atoi( pChar );
}

inline wxString IntTowxString( int n )
{
	wxString ret;
	ret << n;
	return ret;
}

#endif