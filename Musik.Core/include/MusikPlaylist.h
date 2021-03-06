///////////////////////////////////////////////////
// 
// Info:
//
//   Musik is a a cross platform, open source
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
// Class(s): 
//
//   CMusikPlaylist,
//   CMusikPlaylistInfo,
//   CMusikSong,
//   CMusikSongInfo
//
// Filename(s): 
//
//   MusikPlaylist.h, MusikPlaylist.cpp
//
// Information:
//
//   CMusikPlaylist:
//     a CMusikSongArray manager. It allows easy
//     adding, removing, and querying of songs.
//
//   CMusikPlaylistInfo:
//     a class that contains info about a CMusikPlaylist
//
//   CMusikSong:
//     a small class that only contains the (unique)
//     ID of the song as it exists in the loaded database, as
//     well as a few utility functions to access it's respective
//     information. The library must be setup using
//     CMusikSong::SetLibrary(), which is a static function.
//
//   CMusikSongInfo:
//     a larger class than CMusikSong, as it actually
//     stores all of the database information in memory,
//     rather than querying the database to retrieve it.
//
// Usage: 
//
//   CMusikPlaylist:
//     Create an instance and Add(), Remove(), etc.
//
//   CMusikSong:
//     Create and set the song with SetID()
//     Use GetField() to retrieve information
//
//   CMusikSongInfo:
//     Create, then use CMusikLibrary::GetSongInfoFromID()
//     to populate. GetField() or various other Get() functions
//     to query.
//
///////////////////////////////////////////////////

#ifndef C_MUSIK_PLAYLIST_H
#define C_MUSIK_PLAYLIST_H

///////////////////////////////////////////////////

#include "sqlite.h"

#include "StdString.h"

#include <vector>
#include <iostream>

#include "MusikArrays.h"

///////////////////////////////////////////////////

class CMusikLibrary;

///////////////////////////////////////////////////

class CMusikSong
{
	static CMusikLibrary* m_MusikLibrary;

public:
	CMusikSong();
	~CMusikSong();

	// gets
	int GetID(){ return m_ID; }
	CStdString GetField( int field );	

	// sets
	void SetID( int id ){ m_ID = id; }
	void SetField( int field, CStdString value );	
	static void SetLibrary( CMusikLibrary* library ){ m_MusikLibrary = library; }

private:
	int m_ID;
};

///////////////////////////////////////////////////

class CMusikPlaylist
{
public:
	CMusikPlaylist();
	~CMusikPlaylist();

	// gets
	CStdString GetField( int index, int field );
	int GetSongID( int index );
	size_t GetCount();

	// sets
	void SetField( int index, int field );

	// misc
	void Add( const CMusikSong& song );
	void Clear();

private:
	CMusikSongArray m_Songs;
};

///////////////////////////////////////////////////

class CMusikPlaylistInfo
{
public:

	// construct and destruct
	CMusikPlaylistInfo();
	CMusikPlaylistInfo( CStdString name, int type, int id );
	~CMusikPlaylistInfo();
	
	// gets
	CStdString GetName(){ return m_Name; }
	int GetType(){ return m_Type; }
	int GetID(){ return m_ID; }

	// sets
	void Set( CStdString name, int type, int id );

	// misc
	void Rename( CStdString new_name );
	void SaveToLibrary( CMusikLibrary* library );

private:
	CStdString m_Name;
	int m_Type;
	int m_ID;
};

///////////////////////////////////////////////////

class CMusikSongInfo
{
public:
	CMusikSongInfo();
	~CMusikSongInfo();

	void		SetID( int id )		{ m_ID = id; }
	int			GetID()				{ return m_ID; }

	CStdString	GetFilename()		{ return m_Filename; }
	CStdString	GetTitle()			{ return m_Title; }
	CStdString	GetTrackNum()		{ return m_TrackNum; }
	CStdString	GetArtist()			{ return m_Artist; }
	CStdString	GetAlbum()			{ return m_Album; }
	CStdString	GetGenre()			{ return m_Genre; }
	CStdString	GetDuration()		{ return m_Duration; }
	CStdString	GetFormat()			{ return m_Format; }
	CStdString	GetVBR()			{ return m_VBR; }
	CStdString	GetYear()			{ return m_Year; }
	CStdString	GetRating()			{ return m_Rating; }
	CStdString	GetBitrate()		{ return m_Bitrate; }
	CStdString	GetLastPlayed()		{ return m_LastPlayed; }
	CStdString	GetNotes()			{ return m_Notes; }
	CStdString	GetTimesPlayed()	{ return m_TimesPlayed; }
	CStdString	GetTimeAdded()		{ return m_TimeAdded; }
	CStdString	GetFilesize()		{ return m_Filesize; }
	CStdString	GetDirtyFlag()		{ return m_Dirty; }
	
	CStdString	GetField( int field );
	
	void	SetFilename( CStdString s )		{ m_Filename = s; }
	void	SetTitle( CStdString s )		{ m_Title = s; }
	void	SetTrackNum( CStdString s )		{ m_TrackNum = s; }
	void	SetArtist( CStdString s )		{ m_Artist = s; }
	void	SetAlbum( CStdString s )		{ m_Album = s; }
	void	SetGenre( CStdString s )		{ m_Genre = s; }
	void	SetDuration( CStdString s )		{ m_Duration = s; }
	void	SetFormat( CStdString s )		{ m_Format = s; }
	void	SetVBR( CStdString s )			{ m_VBR = s; }
	void	SetYear( CStdString s )			{ m_Year = s; }
	void	SetRating( CStdString s )		{ m_Rating = s; }
	void	SetBitrate( CStdString s )		{ m_Bitrate = s; }
	void	SetLastPlayed( CStdString s )	{ m_LastPlayed = s; }
	void	SetNotes( CStdString s )		{ m_Notes = s; }
	void	SetTimesPlayed( CStdString s )	{ m_TimesPlayed = s; }
	void	SetTimeAdded( CStdString s )	{ m_TimeAdded = s; }
	void	SetFilesize( CStdString s )		{ m_Filesize = s; }
	void	SetDirtyFlag( CStdString s )	{ m_Dirty = s; }

protected:
	int m_ID;

	CStdString	m_Rating, m_Bitrate, m_Duration, m_Dirty, m_Filesize,
				m_TimesPlayed, m_TrackNum, m_Format, m_VBR, m_Filename,
				m_Title, m_Artist, m_Album, m_Year, m_Genre, m_LastPlayed,
				m_TimeAdded, m_Notes;

};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
