/*
 *  COggInfo.h
 *
 *  Helper functions to retrieve oggvorbis comments from a given file.
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef OGGINFO_H
#define OGGINFO_H

//--- globals ---//
#include "../../MusikGlobals.h"

class COggInfo
{
public:
	bool loadInfo( const wxString &filename );

	wxString GetArtist()	{ return m_Song.Artist; }
	wxString GetTitle()		{ return m_Song.Title; }
	wxString GetAlbum()		{ return m_Song.Album; }
	wxString GetGenre()		{ return m_Song.Genre; }
	wxString GetYear()		{ return m_Song.Year; }
	int		 GetTrackNum()	{ return m_Song.TrackNum; }
	int		 GetBitrate()	{ return m_Song.Bitrate; }
private:
	CMusikSong m_Song;
};

#endif
