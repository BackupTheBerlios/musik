/*
 *  MusikUtils.h
 *
 *  Global functions that don't really belong 
 *  anywhere but here.
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
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
#include <wx/regex.h>
#include <wx/listctrl.h>
#include "MusikDefines.h"

class CSongMetaData
{
public:
	class StringData
	{
	public:
		StringData()
		{
			m_szData = NULL;
			m_bOwner = false;
		}
		StringData(const char* sz , size_t len = (size_t) -1)
		{
			Assign(sz,len);
		}

		void Assign(const char* sz , size_t len = (size_t) -1)
		{
			m_szData = _StrDup(sz,len);
			m_bOwner = true;
		}
		off_t Find(int ch)   const
		{
			if(m_szData)
			{
				char *pos = strchr(m_szData,ch);
				return pos - m_szData;
			}
			return -1;
		}
		const char * Substr(off_t start)
		{
			return IsEmpty() ? "" : m_szData + start;
		}
		void Attach(const char* sz ,bool bOwner = true)
		{
			Empty();
			m_bOwner = bOwner;
			m_szData =  sz ;
		}
		StringData(const StringData & rhs)
		{
			m_bOwner = true;
			m_szData =  _StrDup(rhs.m_szData);
		}
		StringData & operator=(const StringData & rhs)
		{
			if(this != &rhs)
			{
				Empty();
				m_bOwner = true;
				m_szData =  _StrDup(rhs.m_szData);
			}
			return *this;
		}
		StringData & operator=(const char * sz)
		{
			Empty();
			m_bOwner = true;
			m_szData =  _StrDup(sz);
			return *this;
		}

		operator const char *()	const 
		{
			return IsEmpty() ? "" : m_szData;
		}
		bool IsEmpty()	const 
		{
			return (m_szData == NULL) || (Length() == 0);
		}
		void Empty()
		{
			if(m_szData && m_bOwner)
				delete [] (m_szData);
			m_szData = NULL;
		}
		const char * Detach()
		{
			const char * tmp = m_szData;
			m_szData = NULL;
			return tmp;
		}
		size_t Length()	const
		{
			return (m_szData == NULL) ? 0 : strlen(m_szData);
		}
		~StringData()
		{
			Empty();
		}
	protected:
		const char * _StrDup(const char * sz, size_t len=(size_t) -1)
		{
			if(!sz)
				return NULL;
			len = (len == (size_t) -1) ? strlen(sz) : len;
			char *buf = new char[len + 1];
			if(buf)
			{
				strncpy(buf,sz,len);
				buf[len] = 0;
			}
			return buf;
		}
	private:
		const char *m_szData;
		bool m_bOwner;
	};
public:
	CSongMetaData()
	{
		eFormat = MUSIK_FORMAT_INVALID;
		bVBR = false;
		nDuration_ms = nBitrate  = nFilesize   =	nTracknum = 0;
	}

	wxFileName			Filename;
	StringData			Title;
	StringData			Artist;
	StringData			Album;
	StringData			Genre;
	StringData			Year;
	StringData			Notes;
	int				    nTracknum;
	EMUSIK_FORMAT_TYPE  eFormat;
	int					nDuration_ms;
	bool				bVBR;
	int					nBitrate;
	int					nFilesize;

};

inline wxString FilenameAsUrl(const wxFileName &fn)
{
	wxString url = fn.GetFullPath(wxPATH_NATIVE);
  	url.Replace(wxT("\\"), wxT("/"));
	url.Replace(wxT("//http/"),wxT("http://"));
	return url;
}
//------------------------------------------------------------------//
//--- misc utility functions, see implementatio		n for description ---//
//------------------------------------------------------------------//
// wxString		GetGenre			(  const wxString & sGenre );
int				GetGenreID			(  const CSongMetaData::StringData & sGenre  );

void			DelimitStr			( wxString sStr, wxString sDel, wxArrayString &aReturn, bool bRemoveDelimiter = true );

void			GetPlaylistDir		( wxArrayString & aFiles );
wxString		MStoStr				( int timems );
wxArrayString	FileToStringArray	(  const wxString &  sFilename );
int				MusikRound			( float x );
int				FindStrInArray		( wxArrayString* array, wxString pattern );
wxString		MoveArtistPrefixToEnd( const wxString & str );
wxString		SanitizedString		( const wxString & str );
void			wxListCtrlSelAll	( wxListCtrl* listctrl );
void			wxListCtrlSelNone	( wxListCtrl* listctrl );
wxString		GenTempFilename		( wxString filename, int nsize );
wxString		ColourToString		( const wxColour& color );
wxColour		StringToColour		( const wxString& string );


void			SortArrayByLength	( wxArrayString* pArray );

wxString		GetJustFilename		( const wxString & filename );

wxString		GetFramePlacement	( wxFrame* frame );
bool			SetFramePlacement	( wxFrame* frame, wxString place );

void			SeedRandom			( unsigned long s );
long			GetRandomNumber		();

//-------------------------//
//--- Unicode functions ---//
//-------------------------//
inline wxString ConvA2W( const char *pChar )
{
	wxString s(wxConvCurrent->cMB2WX( pChar ) );
	return s;
}
inline const wxCharBuffer ConvW2A( const wxString &s )
{
	return wxConvCurrent->cWX2MB( s );
}


inline const wxCharBuffer ConvToUTF8( const wxString &s )
{
#if wxUSE_UNICODE
	return wxConvUTF8.cWC2MB(s);
#else
	return wxConvUTF8.cWC2WX(wxConvCurrent->cMB2WC(s));
#endif
}
inline const wxCharBuffer ConvFromISO8859_1ToUTF8( const char *s )
{
	return wxConvUTF8.cWC2MB(wxConvISO8859_1.cMB2WC(s));
}


#if wxUSE_UNICODE
inline const wxWCharBuffer ConvFromUTF8( const char *s )
{
	return wxConvUTF8.cMB2WC(s);
}
#else
inline const wxCharBuffer ConvFromUTF8( const char *s )
{
	return wxConvCurrent->cWC2WX(wxConvUTF8.cMB2WC(s));
}
#endif

inline const wxCharBuffer ConvFromUTF8ToISO8859_1( const char *s )
{
#if wxUSE_UNICODE
	return wxConvISO8859_1.cWX2MB(	ConvFromUTF8( s )  );
#else
	return wxConvISO8859_1.cWC2WX(wxConvUTF8.cMB2WC(s));
#endif

}
inline const wxCharBuffer ConvQueryToMB( const wxString &s )
{
	return ConvToUTF8(s);
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
inline double StringToDouble( wxString str )
{
	double ret;
	str.ToDouble( &ret );
	return ret;
}
inline void InternalErrorMessageBox( const wxString &sText)
{
	wxString sMessage = wxString::Format(wxT("An internal error has occured.\n%s,\n\nPlease contact the ") MUSIKAPPNAME wxT(" development team with this error."),(const wxChar *)sText);
	wxMessageBox( sMessage,MUSIKAPPNAME_VERSION, wxOK|wxICON_ERROR );

}

double CharStringToDouble(const char *z);
void DoubleToCharString(double r, char *z);
wxString GetForbiddenChars(wxPathFormat format = wxPATH_NATIVE);

class CNiceFilesize
{
public:
	CNiceFilesize();
	~CNiceFilesize();

public:
	long	m_Bytes;
	long	m_Kilobytes;
	long	m_Megabytes;
	long	m_Gigabytes;
	long	m_Terabytes;

	void	AddB( long b );
	void	AddK( long k );
	void	AddM( long m );
	void	AddG( long g );
	void	AddT( long t );

	wxString	GetFormatted();
};

class CMusikSong;

class CMusikTagger
{

	wxArrayInt 	m_PlaceHolderArray;
	wxRegEx 	m_reMask;
	bool		m_bConvertUnderscoresToSpaces;
public:
	CMusikTagger(const wxString &sMask, bool bConvertUnderscoresToSpaces);
	bool Retag(CMusikSong * Song) const;
};

#endif
