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

class CMusikSongInfo
{
public:
	CMusikSongInfo();
	~CMusikSongInfo();

	void SetID( int id ){ m_ID = id; }
	void Update();
	
	int			GetID()	
	{ return m_ID; }
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
