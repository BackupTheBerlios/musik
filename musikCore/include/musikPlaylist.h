///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2003
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey@bak.rr.com
//   Dustin Carter  : Developer, pain in the ass
//   Simon Windmill : Developer, the most helpful person thats immediately available ;)
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Class(es): 
//
//   CmusikPlaylist,
//   CmusikPlaylistInfo,
//   CmusikSong,
//   CmusikSongInfo
//
// Filename(s): 
//
//   musikPlaylist.h, musikPlaylist.cpp
//
// Information:
//
//   CmusikPlaylist:
//     a CmusikSongArray manager. It allows easy
//     adding, removing, and querying of songs.
//
//   CmusikPlaylistInfo:
//     a class that contains info about a CmusikPlaylist
//
//   CmusikSong:
//     a small class that only contains the (unique)
//     ID of the song as it exists in the loaded database, as
//     well as a few utility functions to access it's respective
//     information. The library must be setup using
//     CmusikSong::SetLibrary(), which is a static function.
//
//   CmusikSongInfo:
//     a larger class than CmusikSong, as it actually
//     stores all of the database information in memory,
//     rather than querying the database to retrieve it.
//
// Usage: 
//
//   CmusikPlaylist:
//     Create an instance and Add(), Remove(), etc.
//
//   CmusikSong:
//     Create and set the song with SetID()
//     Use GetField() to retrieve information
//
//   CmusikSongInfo:
//     Create, then use CmusikLibrary::GetSongInfoFromID()
//     to populate. GetField() or various other Get() functions
//     to query.
//
///////////////////////////////////////////////////

#ifndef C_MUSIK_PLAYLIST_H
#define C_MUSIK_PLAYLIST_H

///////////////////////////////////////////////////

#include "sqlite.h"

#include "musikConfig.h"

#include <vector>
#include <iostream>

#include "musikArrays.h"

///////////////////////////////////////////////////

class CmusikLibrary;

///////////////////////////////////////////////////

class CmusikSong
{
	static CmusikLibrary* m_musikLibrary;

public:
	CmusikSong();
	~CmusikSong();

	// gets
	int GetID(){ return m_ID; }
	CmusikString GetField( int field );	

	// sets
	void SetID( int id ){ m_ID = id; }
	void SetField( int field, CmusikString value );	
	static void SetLibrary( CmusikLibrary* library ){ m_musikLibrary = library; }

private:
	int m_ID;
};

///////////////////////////////////////////////////

class CmusikPlaylist
{
public:
	CmusikPlaylist();
	~CmusikPlaylist();

	// gets
	CmusikString GetField( int index, int field );
	int GetSongID( int index );
	size_t GetCount();
	int GetPlaylistID(){ return m_ID; }

	// sets
	void SetField( int index, int field );
	void SetPlaylistID( int id ){ m_ID = id;} 

	// misc
	void Add( const CmusikSong& song );
	void DeleteAt( size_t pos );
	void InsertAt( int songid, int pos );
	void InsertAt( const CIntArray& songids, int pos );
	void Clear();

	int m_ID;
	CmusikSongArray m_Songs;
};

///////////////////////////////////////////////////

class CmusikPlaylistInfo
{
public:

	// construct and destruct
	CmusikPlaylistInfo();
	CmusikPlaylistInfo( CmusikString name, int type, int id );
	~CmusikPlaylistInfo();
	
	// gets
	CmusikString GetName(){ return m_Name; }
	int GetType(){ return m_Type; }
	int GetID(){ return m_ID; }

	// sets
	void Set( CmusikString name, int type, int id );

	// misc
	void Rename( CmusikString new_name );
	void SaveToLibrary( CmusikLibrary* library );

private:
	CmusikString m_Name;
	int m_Type;
	int m_ID;
};

///////////////////////////////////////////////////

class CmusikSongInfo
{
public:
	CmusikSongInfo();
	~CmusikSongInfo();

	CmusikString GetField( int field );
	void SetField( int field, const CmusikString& str );

	void SetID( int id )				{ m_ID = id; }
	int	GetID()							{ return m_ID; }

	CmusikString	GetFilename()			{ return m_Filename; }
	CmusikString	GetTitle()				{ return m_Title; }
	CmusikString	GetTrackNum()			{ return m_TrackNum; }
	CmusikString	GetArtist()				{ return m_Artist; }
	CmusikString	GetAlbum()				{ return m_Album; }
	CmusikString	GetGenre()				{ return m_Genre; }
	CmusikString	GetDuration()			{ return m_Duration; }
	CmusikString	GetFormat()				{ return m_Format; }
	CmusikString	GetVBR()				{ return m_VBR; }
	CmusikString	GetYear()				{ return m_Year; }
	CmusikString	GetRating()				{ return m_Rating; }
	CmusikString	GetBitrate()			{ return m_Bitrate; }
	CmusikString	GetLastPlayed()			{ return m_LastPlayed; }
	CmusikString	GetNotes()				{ return m_Notes; }
	CmusikString	GetTimesPlayed()		{ return m_TimesPlayed; }
	CmusikString	GetTimeAdded()			{ return m_TimeAdded; }
	CmusikString	GetFilesize()			{ return m_Filesize; }
	CmusikString	GetDirtyFlag()			{ return m_Dirty; }
	CmusikString	GetEqualizer()			{ return m_Equalizer; }
	
	void SetFilename( CmusikString s )		{ m_Filename = s; }
	void SetTitle( CmusikString s )			{ m_Title = s; }
	void SetTrackNum( CmusikString s )		{ m_TrackNum = s; }
	void SetArtist( CmusikString s )		{ m_Artist = s; }
	void SetAlbum( CmusikString s )			{ m_Album = s; }
	void SetGenre( CmusikString s )			{ m_Genre = s; }
	void SetDuration( CmusikString s )		{ m_Duration = s; }
	void SetFormat( CmusikString s )		{ m_Format = s; }
	void SetVBR( CmusikString s )			{ m_VBR = s; }
	void SetYear( CmusikString s )			{ m_Year = s; }
	void SetRating( CmusikString s )		{ m_Rating = s; }
	void SetBitrate( CmusikString s )		{ m_Bitrate = s; }
	void SetLastPlayed( CmusikString s )	{ m_LastPlayed = s; }
	void SetNotes( CmusikString s )			{ m_Notes = s; }
	void SetTimesPlayed( CmusikString s )	{ m_TimesPlayed = s; }
	void SetTimeAdded( CmusikString s )		{ m_TimeAdded = s; }
	void SetFilesize( CmusikString s )		{ m_Filesize = s; }
	void SetDirtyFlag( CmusikString s )		{ m_Dirty = s; }
	void SetEqualizer( CmusikString s )		{ m_Equalizer = s; }

protected:

	int m_ID;

	CmusikString	m_Rating, m_Bitrate, m_Duration, m_Dirty, m_Filesize,
					m_TimesPlayed, m_TrackNum, m_Format, m_VBR, m_Filename,
					m_Title, m_Artist, m_Album, m_Year, m_Genre, m_LastPlayed,
					m_TimeAdded, m_Notes, m_Equalizer;

};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
