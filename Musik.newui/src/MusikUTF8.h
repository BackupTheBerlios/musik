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

#endif