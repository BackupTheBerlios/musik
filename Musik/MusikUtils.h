/*
 *  MusikUtils.h
 *
 *  Global functions that don't really belong 
 *  anywhere but here.
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef MUSIKUTILS_H
#define MUSIKUTILS_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

#include "wx/listctrl.h"
class MusikLibraryFrame;
//------------------------------------------------------------------//
//--- misc utility functions, see implementation for description ---//
//------------------------------------------------------------------//
wxString			DelimitedInfoAt		( wxString sStr, int nIndex );
wxString			GetGenre			( wxString sGenre );
int					GetGenreID			( wxString sGenre );
wxArrayString		DelimitStr			( wxString sStr, wxString sDel, bool bRemoveDelimiter = true );
wxArrayString		GetMusicDirs		( wxArrayString *aDirs );
wxArrayString		GetPlaylistDir		();
wxString			MStoStr				( int timems );
int					TimeToSec			( wxString sTime );
wxArrayString		FileToStringArray	( wxString sFilename );
bool				FileExists			( wxString sName );
int					MusikRound			( float x );
int					FindStrInArray		( wxArrayString* array, wxString pattern );
wxString			SanitizedString		( wxString str );
void				wxListCtrlSelAll	( wxListCtrl* listctrl );
void				wxListCtrlSelNone	( wxListCtrl* listctrl );
wxString			GenTempFilename		( wxString filename, int nsize );
wxString			ColourToString		( const wxColour& color );
wxColour			StringToColour		( const wxString& string );

wxString			GetFramePlacement	( wxFrame* frame );
bool				SetFramePlacement	( wxFrame* frame, wxString place );

void				SeedRandom			( unsigned long s );
long				GetRandomNumber		();

#if 0
//-------------------------//
//--- unicode functions ---//
//-------------------------//
int StringToInt( wxString str );
int StringToInt( const char* pChar );
wxString IntTowxString( int n);
double StringToDouble( wxString str );
#endif

//-------------------------//
//--- Unicode functions ---//
//-------------------------//
inline wxString ConvA2W(const char *pChar)
{
	wxString s(wxConvLocal.cMB2WX( pChar));
	return s;
}
inline wxCharBuffer ConvQueryToMB(const wxString &s)
{
	return wxConvLocal.cWX2MB(s);
}

inline wxCharBuffer ConvDBFieldToMB(const wxString &s)
{
	return wxConvLocal.cWX2MB(s);
}
inline wxCharBuffer ConvFNToFieldMB(const wxString &s)
{
	return wxConvLocal.cWX2MB(s);
}
inline wxString ConvFNToFieldWX(const char *pchar)
{
	return wxConvLocal.cMB2WX(pchar);
}
inline wxString ConvDBFieldToWX(const char *pchar)
{
	return wxConvLocal.cMB2WX(pchar);
}
inline int StringToInt( const wxString &str )
{
	long lRet;
	str.ToLong( &lRet );
	return (int)lRet;
}

inline int StringToInt( const char* pChar )
{
	return atoi(pChar);
}

inline wxString IntTowxString( int n)
{
	wxString ret;
	ret << n;
	return ret;
}
inline double StringToDouble( wxString str )
{
	double ret;
	str.ToDouble( &ret );
	return ret;
}

#define StringToANSI( str )	( strdup( (str).mb_str(wxConvLocal) ) ) //--- gunnar roth ---//

#endif
