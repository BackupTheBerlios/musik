#ifndef C_MUSIK_PLAYLIST_H
#define C_MUSIK_PLAYLIST_H

#include "wx/wxprec.h"
#include "wx/dynarray.h"
#include "wx/datetime.h"

class CMusikSong
{
public:
	CMusikSong();

public:
	wxString	sFilename;
	wxString	sTitle;
	int			nTrackNum;
	wxString	sArtist;
	wxString	sAlbum;
	wxString	sGenre;
	wxString	sYear;
	wxString	sLastPlayed;
	int			nFormat;
	int			nDuration;
	int			nRating;
	int			nTimesPlayed;
	int			nBitrate;
	bool		bVBR;
	wxString	sNotes;
	wxString	sTimeAdded;
	int			nFilesize;
};

WX_DECLARE_OBJARRAY( CMusikSong, CMusikPlaylist );

#endif