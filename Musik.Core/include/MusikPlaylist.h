#ifndef C_MUSIK_PLAYLIST_H
#define C_MUSIK_PLAYLIST_H

#include "sqlite.h"
#include "StdString.h"
#include <vector>
#include <iostream>

#include "MusikArrays.h"

#ifndef BOOST_HAS_THREADS
#define BOOST_HAS_THREADS 1
#endif

class CMusikLibrary;

//-----------------------------------------------------//
//--- CMusik Song: a single song ID					---//
//-----------------------------------------------------//
class CMusikSong
{
	static CMusikLibrary* m_MusikLibrary;

public:
	CMusikSong();
	~CMusikSong();

	//-----------------------------------------------------//
	//--- gets											---//
	//-----------------------------------------------------//
	int GetID(){ return m_ID; }
	CStdString GetField( int field );	

	//-----------------------------------------------------//
	//--- sets											---//
	//-----------------------------------------------------//
	void SetID( int id ){ m_ID = id; }
	void SetField( int field, CStdString value );	
	static void SetLibrary( CMusikLibrary* library ){ m_MusikLibrary = library; }

private:
	int m_ID;
};

//-----------------------------------------------------//
//--- CMusikPlaylist: a class that manages an array	---//
//--- of CMusikSongs (CMusikSongArray)				---//
//-----------------------------------------------------//
class CMusikPlaylist
{
public:
	CMusikPlaylist();
	~CMusikPlaylist();

	//-----------------------------------------------------//
	//--- gets											---//
	//-----------------------------------------------------//
	CStdString GetField( int index, int field );

	//-----------------------------------------------------//
	//--- sets											---//
	//-----------------------------------------------------//
	void SetField( int index, int field );

	//-----------------------------------------------------//
	//--- stl wrapper functions							---//
	//-----------------------------------------------------//
	void push_back( const CMusikSong& song ){ m_Songs.push_back( song ); }
	void clear(){ m_Songs.clear(); }
	size_t size(){ return m_Songs.size(); }
	CMusikSongArray* items(){ return &m_Songs; }

private:
	CMusikSongArray m_Songs;
};

class CMusikSongInfo
{
	//-----------------------------------------------------//
	//--- this function, found in CMusikLibrary, is		---//
	//--- allowed to set the protected items			---//
	//-----------------------------------------------------//
	friend static int sqlite_GetSongInfoFromID( void *args, int numCols, char **results, char ** columnNames );

public:
	CMusikSongInfo();
	~CMusikSongInfo();

	void SetID( int id ){ m_ID = id; }
	void Update();
	
	const int&			GetID()				{ return m_ID; }
	const CStdString&	GetFilename()		{ return m_Filename; }
	const CStdString&	GetTitle()			{ return m_Title; }
	const int&			GetTrackNum()		{ return m_TrackNum; }
	const CStdString&	GetArtist()			{ return m_Artist; }
	const CStdString&	GetAlbum()			{ return m_Album; }
	const CStdString&	GetGenre()			{ return m_Genre; }
	const int&			GetDuration()		{ return m_Duration; }
	const int&			GetFormat()			{ return m_Format; }
	const int&			GetVBR()			{ return m_VBR; }
	const CStdString&	GetYear()			{ return m_Year; }
	const int&			GetRating()			{ return m_Rating; }
	const int&			GetBitrate()		{ return m_Bitrate; }
	const CStdString&	GetLastPlayed()		{ return m_LastPlayed; }
	const CStdString&	GetNotes()			{ return m_Notes; }
	const int&			GetTimesPlayed()	{ return m_TimesPlayed; }
	const CStdString&	GetTimeAdded()		{ return m_TimeAdded; }
	const int&			GetFilesize()		{ return m_Filesize; }
	const int&			GetDirtyFlag()		{ return m_Dirty; }

protected:

	int			m_ID, m_Rating, m_Bitrate, m_Duration, m_Dirty, m_Filesize,
				m_TimesPlayed, m_TrackNum, m_Format, m_VBR;

	CStdString	m_Filename, m_Title, m_Artist, m_Album, m_Year, m_Genre,
				m_LastPlayed, m_TimeAdded, m_Notes;
};


#endif