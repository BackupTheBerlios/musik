/*
 *  MusikUtils.cpp
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

//--- For compilers that support precompilation, includes "wx/wx.h" ---//
#include "wx/wxprec.h"

//--- globals ---//
#include "MusikGlobals.h"
#include "MusikUtils.h"

//--- mp3 helper ---//
#include "Classes/Library/CMP3Info.h"
#include <id3/tag.h>

//--- wx ---//
#include <wx/textfile.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <wx/dir.h>



class wxPlaylistTraverser : public wxDirTraverser
{
public:
    wxPlaylistTraverser(wxArrayString& files) : m_files(files) { }

    virtual wxDirTraverseResult OnFile(const wxString& filename)
    {
		wxFileName fn( filename );
		wxString ext = fn.GetExt();
		ext.MakeLower();

		if ( ext.Left(2) == wxT( "mp" ))
		{
			wxString add = fn.GetFullName();
			m_files.Add( add );
		}
        
        return wxDIR_CONTINUE;
    }

    virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname))
    {
        return wxDIR_CONTINUE;
    }

private:
    wxArrayString& m_files;
};



void  DelimitStr( wxString sStr, wxString sDel, wxArrayString &aReturn, bool bRemoveDelimiter )
{
	aReturn.Clear();
	wxStringTokenizer toke( sStr, sDel );

	while( toke.HasMoreTokens() )
	{
		//--- if we remove the delimiter, do nothing ---//
		if ( bRemoveDelimiter )
			aReturn.Add( toke.GetNextToken() );
		
		//--- otherwise tack it to the end ---//
		else
			aReturn.Add( toke.GetNextToken() + sDel );
	}

	//--- strip last delimiter ---//
	aReturn.Item( aReturn.GetCount()-1 ).Replace( sDel, wxT( "" ), true );

	return;
}

void SortArrayByLength ( wxArrayString* pArray )
{
	wxString sTemp;

	for ( size_t i = 0; i < pArray->GetCount(); i++ )
	{
		for ( size_t j = 0; j < pArray->GetCount()-1; j++ )
		{	
			if( pArray->Item( j ).Length() < pArray->Item( j + 1 ).Length() )
			{
				sTemp = pArray->Item( j );
				pArray->Item( j + 0 ) = pArray->Item( j + 1 );
				pArray->Item( j + 1 ) = sTemp;
			}
		}
	}
}

wxString GetGenre ( const wxString & sGenre ) 
{ 
	// if sGenre is a number, the name will be returned
	// if sGenre is something like (nnn) XXXXX , the name of the number nnn will be returned
	// if nothing matches the sGenre will be returned, but if sGenre is an empty string, wxT("<unknown>") will be returned
	wxString aReturn(sGenre);
	if ( sGenre.Length() > 0 )
	{
		int nGenreID = -1;
		if(wxIsdigit(sGenre[0]))
			nGenreID = wxStringToInt( sGenre );
		else if (sGenre.StartsWith(wxT("("),&aReturn))
		{	
			aReturn = aReturn.BeforeFirst(')');
			nGenreID = wxStringToInt( aReturn );
		}
		if (nGenreID >=0 && nGenreID < ID3_NR_OF_V1_GENRES )
			aReturn = ConvA2W( ID3_v1_genre_description[nGenreID] );
	}

	return aReturn;
}

int GetGenreID( const wxString & sGenre )
{		
	for ( int i = 0; i < ID3_NR_OF_V1_GENRES; i++ )
	{
		wxString sCur = ConvA2W( ID3_v1_genre_description[i] );
		if ( sGenre.CmpNoCase( sCur ) == 0)
			return i;
	}
	return -1; //--- return -1 if unknown ---//
}

wxString MStoStr( int timems )
{
	wxString result;

	int ms = timems;
	int hours = ms / 1000 / 60 / 60;
	ms -= hours * 1000 * 60 * 60;
	int minutes = ms / 1000 / 60;
	ms -= minutes * 1000 * 60;
	int seconds = ms / 1000;

	if ( hours > 0 )
		result.sprintf( wxT("%d:%02d:%02d"), hours, minutes, seconds );
	else
		result.sprintf( wxT("%d:%02d"), minutes, seconds );
	
	return result;
}

wxString GetJustFilename( const wxString & filename )
{
	wxArrayString paths;
	DelimitStr( filename, wxString( MUSIK_PATH_SEPARATOR ), paths, false );
	return paths.Item( paths.GetCount() - 1 );	
}

void GetPlaylistDir( wxArrayString & aFiles )
{
	aFiles.Clear();

	wxDir dir( MUSIK_PLAYLIST_DIR );
	if ( dir.IsOpened() )
	{
		wxPlaylistTraverser traverser( aFiles );
		dir.Traverse( traverser );
	}

	return;
}

wxArrayString FileToStringArray(  const wxString & sName )
{
	wxArrayString aReturn;

	wxTextFile In( sName );

	if ( !wxFileExists( sName ) )
		return aReturn;

	else
	{
		if ( In.Open() )
		{
			for ( int i = 0; i < ( int )In.GetLineCount(); i++ )
			{
				wxString sCheck = In.GetLine( i );
				sCheck.Replace( wxT(" "), wxT(""), TRUE );
				if ( !sCheck.IsEmpty())
					aReturn.Add( In.GetLine( i ) );
			}
			In.Close();
		}
	}
	return aReturn;
}

int MusikRound(float x)
{
	//--- as far as I can tell this is equivalent to (int)(x+0.5) ;) ---//
    float i;
    i = x - (float)(int)x;
    if ((i < 0.5) && (i > -0.5))
		return (int)x;
    else 
		if (i >= 0.5)
			return (int)(x + 1.0);
		else return (int)(x - 1.0);
}

int	FindStrInArray( wxArrayString* array, wxString pattern )
{
	for ( size_t i = 0; i < array->GetCount(); i++ )
	{
		if ( array->Item( i ) == pattern )
			return i;
	}
	return -1;
}

void ReplaceNoCase( wxString& str, const wxString& oldstr, const wxString& newstr )
{
	int pos = 0;
	wxString strlow = str.Lower();
	wxString oldstrlow = oldstr.Lower();
	
	while ( ( pos = strlow.Find( oldstrlow ) ) != -1 )
	{
		str.Remove( pos, oldstr.Len() );
		strlow.Remove( pos, oldstr.Len() );
		str.insert( pos, newstr );
	}
}
wxString MoveArtistPrefixToEnd( const wxString & str )
{
	wxString Artist = str;
	wxString Prefix = Artist.Left(4);
	if(Prefix.IsSameAs(wxT("The "),false))
	{
		Artist = Artist.Right(Artist.Len()-Prefix.Len());
		Artist += wxT(", ");
		Artist += Prefix.Left(Prefix.Len()-1);
	}
	return Artist;
}
wxString SanitizedString( const wxString & str )
{
	if ( !g_Prefs.nBlankSwears )
		return str;

	wxString outstr( str );
	ReplaceNoCase( outstr, _("fuck"), 		wxT("****") );
	ReplaceNoCase( outstr, _("shit"), 		wxT("****") );
	ReplaceNoCase( outstr, _("tits"), 		wxT("****") );
	ReplaceNoCase( outstr, _("dick"), 		wxT("****") );
	ReplaceNoCase( outstr, _("bitch"), 		wxT("*****") );
	ReplaceNoCase( outstr, _("whore"), 		wxT("*****") );
	ReplaceNoCase( outstr, _("slut"), 		wxT("****") );
	ReplaceNoCase( outstr, _("wank"),		wxT("****") );
	ReplaceNoCase( outstr, _("cunt"), 		wxT("****") );
	ReplaceNoCase( outstr, _("manchester"), wxT("**********") );
	ReplaceNoCase( outstr, _("dildo"), 		wxT("*****") );

	return outstr;
}

void wxListCtrlSelAll( wxListCtrl *pList )
{
	#if defined __WXMSW__
		pList->SetItemState( -1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
	#elif defined __WXGTK__
		for( int i = 0; i < pList->GetItemCount(); i++ )
			pList->SetItemState( i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
	#endif
}

void wxListCtrlSelNone( wxListCtrl *pList )
{
	#if defined __WXMSW__
		pList->SetItemState( -1, 0, wxLIST_STATE_SELECTED );
	#elif defined __WXGTK__
		for( int i = 0; i < pList->GetItemCount(); i++ )
			pList->SetItemState( i, 0, wxLIST_STATE_SELECTED );
	#endif
}

wxString GenTempFilename( wxString filename, int nsize )
{
	wxFileName temp_name( filename );
	wxString s;
	int num; 
	srand( wxGetLocalTime() );
	for ( int i = 0; i < nsize; i++ )
	{
		num = rand() % 99;
		s += IntTowxString( num );
	}
	temp_name.SetName( s );
	return temp_name.GetFullPath();
}

wxString ColourToString ( const wxColour& color )
{
	return wxString::Format( wxT("%d, %d, %d"), color.Red(), color.Green(), color.Blue() );
}

wxColour StringToColour ( const wxString& string )
{
	wxStringTokenizer st( string, wxT(",") );

	unsigned long r = 0, g = 0, b = 0;
	bool ok;
	ok = st.GetNextToken().ToULong( &r );
	ok = ok && st.GetNextToken().ToULong( &g );
	ok = ok && st.GetNextToken().ToULong( &b );

	if ( ok )
		return wxColour( r, g, b );
	else
		return wxColour( 0, 0, 0 );
}

wxString GetFramePlacement( wxFrame* frame )
{
	wxPoint pos = frame->GetPosition();
	wxSize size = frame->GetSize();
	bool maximized = frame->IsMaximized();
	bool iconized = frame->IsIconized();
	wxString s = wxString::Format( wxT("%d,%d,%d,%d,%d,%d"), pos.x, pos.y, size.x, size.y, maximized, iconized );
	wxLogDebug( s );
	return s;
}

bool SetFramePlacement( wxFrame* frame, wxString place )
{
	wxStringTokenizer st( place, wxT(",") );

	unsigned long x=0, y=0, w=0, h= 0, maximized, iconized;
	bool ok;
	ok = st.GetNextToken().ToULong( &x );
	ok = ok && st.GetNextToken().ToULong( &y );
	ok = ok && st.GetNextToken().ToULong( &w );
	ok = ok && st.GetNextToken().ToULong( &h );
	ok = ok && st.GetNextToken().ToULong( &maximized );
	ok = ok && st.GetNextToken().ToULong( &iconized );

	ok = ok && (x+w <= (unsigned long)wxSystemSettings::GetMetric( wxSYS_SCREEN_X )) && (y+h <= (unsigned long)wxSystemSettings::GetMetric( wxSYS_SCREEN_Y ));
	if ( ok )
	{
		frame->SetSize( wxRect( x, y, w, h ) );
#if 0 //--- still working out the issues with this, lots of nasty event order probs ---//
		if ( maximized ) 
			frame->Maximize();
		else if ( iconized )
			frame->Iconize();
#endif
	}
	return ok;
}

//------------------------------------------- Random numbers support -------------------------------------//

/* 
A C-program for MT19937, with initialization improved 2002/2/10.
Coded by Takuji Nishimura and Makoto Matsumoto.
This is a faster version by taking Shawn Cokus's optimization,
Matthe Bellew's simplification, Isaku Wada's real version.

Before using, initialize the state by using init_genrand(seed) 
or init_by_array(init_key, key_length).

Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
All rights reserved.                          

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

3. The names of its contributors may not be used to endorse or promote 
products derived from this software without specific prior written 
permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


Any feedback is very welcome.
http://www.math.keio.ac.jp/matumoto/emt.html
email: matumoto@math.keio.ac.jp
*/

/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UMASK 0x80000000UL /* most significant w-r bits */
#define LMASK 0x7fffffffUL /* least significant r bits */
#define MIXBITS(u,v) ( ((u) & UMASK) | ((v) & LMASK) )
#define TWIST(u,v) ((MIXBITS(u,v) >> 1) ^ ((v)&1UL ? MATRIX_A : 0UL))

static unsigned long state[N]; /* the array for the state vector  */
static int left = 1;
static int initf = 0;
static unsigned long *next;

/* initializes state[N] with a seed */
void SeedRandom(unsigned long s)
{
	int j;
	state[0]= s & 0xffffffffUL;
	for (j=1; j<N; j++) {
		state[j] = (1812433253UL * (state[j-1] ^ (state[j-1] >> 30)) + j); 
		/* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
		/* In the previous versions, MSBs of the seed affect   */
		/* only MSBs of the array state[].                        */
		/* 2002/01/09 modified by Makoto Matsumoto             */
		state[j] &= 0xffffffffUL;  /* for >32 bit machines */
	}
	left = 1; initf = 1;
}

static void next_state(void)
{
	unsigned long *p=state;
	int j;

	/* if init_genrand() has not been called, */
	/* a default initial seed is used         */
	if (initf==0) SeedRandom(5489UL);

	left = N;
	next = state;

	for (j=N-M+1; --j; p++) 
		*p = p[M] ^ TWIST(p[0], p[1]);

	for (j=M; --j; p++) 
		*p = p[M-N] ^ TWIST(p[0], p[1]);

	*p = p[M-N] ^ TWIST(p[0], state[0]);
}

/* generates a random number on [0,0x7fffffff]-interval */
long GetRandomNumber(void)
{
	unsigned long y;

	if (--left == 0) next_state();
	y = *next++;

	/* Tempering */
	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680UL;
	y ^= (y << 15) & 0xefc60000UL;
	y ^= (y >> 18);

	return (long)(y>>1);
}

///////////////////////////////////////////////////////////////////////////////

CNiceFilesize::CNiceFilesize()
{
	m_Bytes = 0;
	m_Kilobytes = 0;
	m_Megabytes = 0;
	m_Gigabytes = 0;
	m_Terabytes = 0;
}

CNiceFilesize::~CNiceFilesize()
{
}

void CNiceFilesize::AddB( long b )
{
	if ( ( m_Bytes + b ) > 1023 )
	{
		long kb = ( m_Bytes + b ) / 1024;
		long by = ((m_Bytes + b) * 100 / 1024) % 100;
		AddK( kb );
		m_Bytes = by;
	}
	else
		m_Bytes += b;

}

void CNiceFilesize::AddK( long k )
{
	if ( ( m_Kilobytes + k ) > 1023 )
	{
		long mb = ( m_Kilobytes + k ) / 1024;
		long kb = ((m_Kilobytes + k) * 100 / 1024) % 100;
		AddM( mb );
		m_Kilobytes = kb;
	}
	else
		m_Kilobytes += k;

}

void CNiceFilesize::AddM( long m )
{
	if ( ( m_Megabytes + m ) > 1023 )
	{
		long gb = (m_Megabytes + m) / 1024;
		long mb = ((m_Megabytes + m) * 100 / 1024) % 100;
		AddG( gb );
		m_Megabytes = mb;
	}
	else
		m_Megabytes += m;

}

void CNiceFilesize::AddG( long g )
{	
	if ( ( m_Gigabytes + g ) > 1023 )
	{
		long tb = ( m_Gigabytes + g ) / 1024;
		long gb = ((m_Gigabytes + g) * 100 / 1024) % 100;
		AddT( tb );
		m_Gigabytes = gb;
	}
	else
		m_Gigabytes += g;
}


void CNiceFilesize::AddT( long t )
{
	m_Terabytes += t;
}

wxString CNiceFilesize::GetFormatted()
{
	if ( m_Terabytes > 0 )
		return wxString::Format( _("%d.%02d TB"), m_Terabytes, m_Gigabytes );
	
	if ( m_Gigabytes > 0 )
		return wxString::Format( _("%d.%02d GB"), m_Gigabytes, m_Megabytes );

	if ( m_Megabytes > 0 )
		return wxString::Format( _("%d.%02d MB"), m_Megabytes, m_Kilobytes );

	if ( m_Kilobytes > 0 )
		return wxString::Format( _("%d.%02d KB"), m_Kilobytes, m_Bytes );

	return wxString( _("Empty") );
}

CMusikTagger::CMusikTagger(const wxString &sTheMask, bool bConvertUnderscoresToSpaces)
:m_bConvertUnderscoresToSpaces(bConvertUnderscoresToSpaces)
{
	wxString sMask(sTheMask);
	sMask.Trim();
	if(sMask.Left(1) != wxString(MUSIK_PATH_SEPARATOR))
	{
		sMask = MUSIK_PATH_SEPARATOR + sMask;
	}
	wxRegEx reMatchRegexSpecialChars(wxT("([{}\\:\\^\\*\\.\\+\\$\\(\\)\\|\\?\\\\]|\\[|\\])"));
	reMatchRegexSpecialChars.ReplaceAll(&sMask,wxT("\\\\\\1"));// replace all special regex chars by \char

	wxRegEx reMatchPlaceHolder(wxT("%([[:alnum:]])"));
	wxString sMaskParse = sMask;
	while(reMatchPlaceHolder.Matches(sMaskParse))
	{
		size_t start=0, len=0;
		reMatchPlaceHolder.GetMatch(&start,&len,1);
		int nPlaceHolder = sMaskParse.Mid(start,len).c_str()[0];
		m_PlaceHolderArray.Add(nPlaceHolder);
		sMaskParse = sMaskParse.Right(sMaskParse.Length() - (start + len));
	}
	wxRegEx reMatchPlaceHolderRemoveTracknum(wxT("(%6|%n)"));// tracknum
	reMatchPlaceHolderRemoveTracknum.ReplaceAll(&sMask,wxT("([[:digit:]]+)"));

	wxRegEx reMatchPlaceHolderRemoveTracknumUC(wxT("(%N)"));// tracknum ( eat up surrounding spaces)
	reMatchPlaceHolderRemoveTracknumUC.ReplaceAll(&sMask,wxT("\\ *([[:digit:]]+)\\ *"));
	// replace all other %x by ([^\\/]+)
	wxRegEx reMatchPlaceHolderRemove(wxT("(%[a-z0-9])"));
	reMatchPlaceHolderRemove.ReplaceAll(&sMask,wxT("([^\\\\/]+)"));
	// replace all other %X by "\ *([^\\\\/]+)\ *" this eats up all surrounding spaces
	wxRegEx reMatchPlaceHolderRemoveUC(wxT("(%[A-Z])"));
	reMatchPlaceHolderRemoveUC.ReplaceAll(&sMask,wxT("\\ *([^\\\\/]+)\\ *"));

	sMask += '$';
	m_reMask.Compile(sMask);
}

bool CMusikTagger::Retag(CMusikSong * Song) const
{
	wxString sFile	= Song->Filename;
	//-------------------------------------------------//
	//--- get rid of the file extension.		---//
	//-------------------------------------------------//
	wxStripExtension(sFile);
	if(m_bConvertUnderscoresToSpaces)
		sFile.Replace(wxT( "_" ), wxT( " " ));

	if(m_reMask.Matches(sFile))
	{
		for(size_t i = 0;i < m_PlaceHolderArray.GetCount(); i++ )
		{
			wxString sField;
			size_t start = 0,len = 0;
			if(!m_reMask.GetMatch(&start,&len,i + 1))
				break;
			sField = sFile.Mid(start, len);
			switch ( m_PlaceHolderArray[i] )
			{
			case '1':
			case 't':
			case 'T':
				Song->Title = sField;
				break;
			case '2':
			case 'a':
			case 'A':
				Song->Artist = sField;
				break;
			case '3':
			case 'b':
			case 'B':
				Song->Album = sField;
				break;
			case '4':
			case 'g':
			case 'G':
				Song->Genre = sField;
				break;
			case '5':
			case 'y':
			case 'Y':
				Song->Year = sField;
				break;
			case '6':
			case 'n':
			case 'N':
				Song->TrackNum = wxStringToInt( sField);
				break;
			default:
				// skip
				break;
			}

		}
		return true;
	}
	else
	{
		return false;
	}
}
