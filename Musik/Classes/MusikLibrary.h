/*
 *  MusikLibrary.h
 *
 *  Object which controls database manipulation.
 *
 *  Uses: SQLite, id3lib, and oggvorbis
 *  Information about SQLite - An Embeddable SQL Database Engine is available at http://www.hwaci.com/sw/sqlite/
 *  Information about id3lib is available at http://www.id3lib.org
 *  Information about oggvorbis is available at http://www.vorbis.com/
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef MUSIK_LIBRARY_H
#define MUSIK_LIBRARY_H

//--- sql ---//
#include "sqlite.h"

//--- wx ---//
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif
#include <wx/dynarray.h>
#include <wx/datetime.h>
#include <wx/regex.h>
#include <wx/filename.h>

#include "../MusikUtils.h"
enum EMUSIK_LIB_TYPE
{
	MUSIK_LIB_INVALID = -1,
    MUSIK_LIB_ARTIST = 0,
	MUSIK_LIB_ALBUM,
	MUSIK_LIB_GENRE,
	MUSIK_LIB_SONG,
	MUSIK_LIB_YEAR,
	MUSIK_LIB_DECADE
};

enum EMUSIK_FORMAT_TYPE
{
	MUSIK_FORMAT_INVALID = -1,
	MUSIK_FORMAT_MP3 = 0,
	MUSIK_FORMAT_OGG,
	MUSIK_FORMAT_MOD,
	MUSIK_FORMAT_WAV,
	MUSIK_FORMAT_WMA,
	MUSIK_FORMAT_AIFF,
	MUSIK_FORMAT_NETSTREAM
};

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
		const char * _StrDup(const char * sz)
		{
			if(!sz)
				return NULL;
			char *buf = new char[strlen(sz) + 1];
			if(buf)
			{
				strcpy(buf,sz);
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

class CMusikSong
{
public:
	CMusikSong();

public:
	int			songid;
	wxString	Filename;
	wxString	Title;
	int			TrackNum;
	wxString	Artist;
	wxString	Album;
	wxString	Genre;
	wxString	Year;
	wxString	LastPlayed;
	int			Format;
	int			Duration;
	int			Rating;
	int			TimesPlayed;
	int			Bitrate;
	bool		VBR;
	wxString	Notes;
	wxString	TimeAdded;
	int			Filesize;
	int			Check1;		//--- for tag dlg stuff, checks to see if it needs to be written to file / db ---//
};

WX_DECLARE_OBJARRAY( CMusikSong, CMusikSongArray );

class CMusikLibrary : public wxEvtHandler
{
public:
	CMusikLibrary();
	~CMusikLibrary();

	//---------------//
	//--- loading ---//
	//---------------//
	bool Load		();
	void Shutdown	();
	void AddSongDataFromFile	( const wxString & filename );
	void UpdateSongDataFromFile	( const wxString & filename );
	void BeginTransaction()
	{ 	wxCriticalSectionLocker lock( m_csDBAccess );
		sqlite_exec_printf( m_pDB, "begin transaction;", NULL, NULL, NULL );
	}
	void EndTransaction()
	{ 		
		wxCriticalSectionLocker lock( m_csDBAccess );
		sqlite_exec_printf( m_pDB, "end transaction;", NULL, NULL, NULL );
	}
	//----------------//
	//--- updating ---//
	//----------------//
	void UpdateItem				( CMusikSong &newsonginfo, bool bDirty );
	void UpdateItemLastPlayed	( const CMusikSong & song  );
	void UpdateItemResetDirty	( const CMusikSong & song );
	void SetRating				( int songid, int nVal );
	void InitTimeAdded			( );

	//--------------------//
	//--- writing tags ---//
	//--------------------//
	bool RenameFile			( CMusikSong* song, bool bClearCheck = false );
	bool RetagFile			( const CMusikTagger & tagger, CMusikSong* song );
	bool ReplaceMask		( wxString *sSrc, wxString sMask, wxString sTarget, bool bReplaceAll = true  );
	int  ClearDirtyTags		( );
	void WriteTag			(  CMusikSong & song, bool ClearAll, bool bUpdateDB = true );
	
	//----------------//
	//--- removing ---//
	//----------------//
	bool CheckAndPurge	( const wxString & sFilename	);
	void RemoveSongDir	( const wxString & sDir			);
	void RemoveSong		( const wxString & sSong		);
	void RemoveSong		( int songid					);
	void RemoveAll		( );
	
	//---------------------//
	//--- getting items ---//
	//---------------------//
	void GetFilelistSongs		( const wxArrayString & aFiles, CMusikSongArray & aReturn );
	bool GetSongFromSongid	( int songid, CMusikSong *pSong );

	void SetSortOrderField( int nField, bool descending = false);
	double GetTotalPlaylistSize();

	//--------------------//
	//--- getting info ---//
	//--------------------//
	int	GetTimesPlayed	( const CMusikSong & song );
	int	GetSongCount	();
	int	GetSongDirCount ( wxString sDir );

	//------------//
	//--- misc ---//
	//------------//
	bool FileInLibrary		( const wxString & filename, bool fullpath );

	void RecordSongHistory( const CMusikSong & song ,int playedtime,bool bSelectedByUser);
	//------------------------//
	//--- generic querying ---//
	//------------------------//
	void Query				( const wxString & query, wxArrayString & aReturn );
	void QuerySongsWhere	( const wxString & query, CMusikSongArray & aReturn ,bool bSorted = false);	  //if bSorted ==true,  use SetSortOrderField to set sorting string
	void RedoLastQuerySongsWhere( CMusikSongArray & aReturn ,bool bSorted = false);
	//-----------------------------------//
	//--- pre-defined queries to make ---//
	//---   life a little bit easier  ---//
	//-----------------------------------//
	void GetAllSongs		( CMusikSongArray & aReturn	,bool bSorted = true);
	void GetAllArtists		( wxArrayString & aReturn );
	void GetAllAlbums		( wxArrayString & aReturn );
	void GetAllGenres		( wxArrayString & aReturn );
	void GetAllYears		( wxArrayString & aReturn );
	void GetArtistAlbums	( const wxArrayString & aArtists, wxArrayString & aReturn );

	void GetInfo			( const wxArrayString & aInfo, int nInType, int nOutType, wxArrayString & aReturn );
	void GetSongs			( const wxArrayString & aInfo, int nInType, CMusikSongArray & aReturn );
private:

	sqlite		  *m_pDB;
	void CreateDB();
	int QueryCount			(const char * szQuery );
	bool GetMetaData		( CSongMetaData & MetaData  );
	bool GetMP3MetaData		( CSongMetaData & MetaData );
	void AddMod				( const wxString & filename );
	bool GetOggMetaData		( CSongMetaData & MetaData );
	void AddWav				( const wxString & filename );
	void AddWMA				( const wxString & filename );
	void AddAIFF			( const wxString & filename );

	void WriteMP3Tag		( const CMusikSong & song, bool ClearAll );
	bool WriteOGGTag		( const CMusikSong & song, bool ClearAll );

	wxString m_TimeAdded;

	wxString  m_sSortAllSongsQuery;
	wxString m_lastQueryWhere;
	int m_nCachedSongCount;

	void VerifyYearList ( const wxArrayString & aList,wxArrayString & aVerifiedList );

	wxCriticalSection m_csDBAccess; // to lock all accesses to m_pDB. 
									// used instead of wxMutex, because this is faster on windows. on linux 
									// a wxMutex is used automatically instead
	void CreateDBFuncs();
	static void remprefixFunc(sqlite_func *context, int argc, const char **argv);
	static void wxjuliandayFunc(sqlite_func *context, int argc, const char **argv);
};

#endif
