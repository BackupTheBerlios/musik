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
	MUSIK_FORMAT_MP3 = 0,
	MUSIK_FORMAT_OGG,
	MUSIK_FORMAT_MOD,
	MUSIK_FORMAT_WAV,
	MUSIK_FORMAT_WMA,
	MUSIK_FORMAT_AIFF
};

class CMusikSong
{
public:
	CMusikSong();

public:
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
	void AddItem		( const wxString & filename );
	void BeginTransaction(){ sqlite_exec_printf( m_pDB, "begin transaction;", NULL, NULL, NULL );}
	void EndTransaction(){ sqlite_exec_printf( m_pDB, "end transaction;", NULL, NULL, NULL );}
	//----------------//
	//--- updating ---//
	//----------------//
	void UpdateItem				( const wxString & filename, CMusikSong &newsonginfo, bool bDirty );
	void UpdateItemLastPlayed	( const wxString & filename  );
	void SetRating				( const wxString & sFile, int nVal );
	void InitTimeAdded			( );

	//--------------------//
	//--- writing tags ---//
	//--------------------//
	bool RenameFile			( CMusikSong* song, bool bClearCheck = false );
	bool RetagFile			( CMusikSong* song );
	bool ReplaceMask		( wxString *sSrc, wxString sMask, wxString sTarget, bool bReplaceAll = true  );
	void WriteMP3Tag		( const wxString & filename, bool ClearAll );
	bool WriteOGGTag		( const wxString & filename, bool ClearAll );
	void ClearDirtyTags		( bool bInform );
	
	//----------------//
	//--- removing ---//
	//----------------//
	void CheckAndPurge	( const wxString & sFilename	);
	void DeleteItem		( const wxString & filename		);
	void RemoveSongDir	( const wxString & sDir			);
	void RemoveSong		( const wxString & sSong		);
	void RemoveAll		( );
	
	//---------------------//
	//--- getting items ---//
	//---------------------//
	void GetStdPlaylistSongs	( const wxArrayString & aFiles, CMusikSongArray & aReturn );
	bool 			GetSongFromFilename	( const wxString& filename, CMusikSong *pSong );

	//--------------------//
	//--- getting info ---//
	//--------------------//
	int	GetTimesPlayed	( const wxString & filename );
	int	GetSongCount	();
	int	GetSongDirCount ( wxString sDir );

	//------------//
	//--- misc ---//
	//------------//
	bool FileInLibrary		( const wxString & filename, bool fullpath );

	//------------------------//
	//--- generic querying ---//
	//------------------------//
	void Query				( const wxString & query, wxArrayString & aReturn );
	void QuerySongs			( const wxString & query, CMusikSongArray & aReturn );

	//-----------------------------------//
	//--- pre-defined queries to make ---//
	//---   life a little bit easier  ---//
	//-----------------------------------//
	void GetAllSongs		( CMusikSongArray & aReturn	);
	void GetAllArtists	( wxArrayString & aReturn );
	void GetAllAlbums	( wxArrayString & aReturn );
	void GetAllGenres	( wxArrayString & aReturn );
	void GetAllYears		( wxArrayString & aReturn );
	void GetArtistAlbums	( const wxArrayString & aArtists, wxArrayString & aReturn );
	void GetArtistGenres	( const wxArrayString & aArtists, wxArrayString & aReturn );
	void GetArtistYears	( const wxArrayString & aArtists, wxArrayString & aReturn );
	void GetArtistSongs	( const wxArrayString & aArtists, CMusikSongArray & aReturn );
	void GetAlbumArtists	( const wxArrayString & aAlbums, wxArrayString & aReturn );
	void GetAlbumGenres	( const wxArrayString & aAlbums, wxArrayString & aReturn );
	void GetAlbumYears	( const wxArrayString & aAlbums, wxArrayString & aReturn );
	void GetAlbumSongs	( const wxArrayString & aAlbums, CMusikSongArray & aReturn );
	void GetGenreArtists	( const wxArrayString & aGenres, wxArrayString & aReturn );
	void GetGenreAlbums	( const wxArrayString & aGenres, wxArrayString & aReturn );
	void GetGenreYears	( const wxArrayString & aGenres, wxArrayString & aReturn );
	void GetGenreSongs	( const wxArrayString & aGenres, CMusikSongArray & aReturn );
	void GetYearArtists	( const wxArrayString & aYears, wxArrayString & aReturn );
	void GetYearAlbums	( const wxArrayString & aYears, wxArrayString & aReturn );
	void GetYearGenres	( const wxArrayString & aYears, wxArrayString & aReturn );
	void GetYearSongs	( const wxArrayString & aYears, CMusikSongArray & aReturn );
	void GetInfo		( const wxArrayString & aInfo, int nInType, int nOutType, wxArrayString & aReturn );
	
private:
	void GetSongs		( const wxArrayString & aInfo, int nInType, CMusikSongArray & aReturn );
	sqlite		  *m_pDB;
	void CreateDB();

	
	void AddMP3				( const wxString & filename );
	void AddMod				( const wxString & filename );
	void AddOgg				( const wxString & filename );
	void AddWav				( const wxString & filename );
	void AddWMA				( const wxString & filename );
	void AddAIFF			( const wxString & filename );

	wxString m_TimeAdded;

	void VerifyYearList ( const wxArrayString & aList,wxArrayString & aVerifiedList );

	wxMutex LibMutex;
};

#endif
