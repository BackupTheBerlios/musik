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
 *	Contributors: Simon Windmill, Dustin Carter
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

	//----------------//
	//--- updating ---//
	//----------------//
	void UpdateItem				( const wxString & filename, CMusikSong &newsonginfo, bool bDirty );
	void UpdateItemLastPlayed	( const wxString & filename  );
	void SetRating				( const wxString & sFile, int nVal );

	//--------------------//
	//--- writing tags ---//
	//--------------------//
	bool RenameFile			( CMusikSong* song, bool bClearCheck = false );

	bool RetagFile			( CMusikSong* song );
	bool CheckTokenForInt	( const wxArrayString aTokens, size_t nStart, size_t nEnd );

	void WriteMP3Tag		( const wxString & filename, bool ClearAll );
	bool WriteOGGTag		( const wxString & filename, bool ClearAll );
	void ClearDirtyTags		( bool bInform );
	
	//----------------//
	//--- removing ---//
	//----------------//
	void CheckAndPurge	( const wxString & sFilename	);
	void DeleteItem		( const wxString &  filename	);
	void RemoveSongDir	( const wxString &  sDir		);
	void RemoveSong		( const wxString &  sSong		);
	void RemoveAll		( );
	
	//---------------------//
	//--- getting items ---//
	//---------------------//
	CMusikSongArray GetStdPlaylistSongs	( wxArrayString *aFiles );
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
	wxArrayString	Query				( const wxString & query );
	CMusikSongArray QuerySongs			( const wxString & query );

	//-----------------------------------//
	//--- pre-defined queries to make ---//
	//---   life a little bit easier  ---//
	//-----------------------------------//
	CMusikSongArray	GetAllSongs		(							);
	wxArrayString	GetAllArtists	(							);
	wxArrayString	GetAllAlbums	(							);
	wxArrayString	GetAllGenres	(							);
	wxArrayString	GetAllYears		(							);
	wxArrayString	GetArtistAlbums	( wxArrayString *aArtists	);
	wxArrayString	GetArtistGenres	( wxArrayString *aArtists	);
	wxArrayString	GetArtistYears	( wxArrayString *aArtists	);
	CMusikSongArray GetArtistSongs	( wxArrayString *aArtists	);
	wxArrayString	GetAlbumArtists	( wxArrayString *aAlbums	);
	wxArrayString	GetAlbumGenres	( wxArrayString *aAlbums	);
	wxArrayString	GetAlbumYears	( wxArrayString *aAlbums	);
	CMusikSongArray GetAlbumSongs	( wxArrayString *aAlbums	);
	wxArrayString	GetGenreArtists	( wxArrayString *aGenres	);
	wxArrayString	GetGenreAlbums	( wxArrayString *aGenres	);
	wxArrayString	GetGenreYears	( wxArrayString *aGenres	);
	CMusikSongArray GetGenreSongs	( wxArrayString *aGenres	);
	wxArrayString	GetYearArtists	( wxArrayString *aYears		);
	wxArrayString	GetYearAlbums	( wxArrayString *aYears		);
	wxArrayString	GetYearGenres	( wxArrayString *aYears		);
	CMusikSongArray GetYearSongs	( wxArrayString *aYears		);
	
private:
	wxArrayString GetInfo			( wxArrayString *aInfo, int nInType, int nOutType );
	CMusikSongArray GetSongs		( wxArrayString *aInfo, int nInType );
	sqlite		  *m_pDB;
	void CreateDB();

	void AddMP3				( const wxString & filename );
	void AddMod				( const wxString & filename );
	void AddOgg				( const wxString & filename );
	void AddWav				( const wxString & filename );
	void AddWMA				( const wxString & filename );
	void AddAIFF			( const wxString & filename );

	wxArrayString VerifyYearList ( wxArrayString *aList );

	wxMutex LibMutex;
};

#endif
