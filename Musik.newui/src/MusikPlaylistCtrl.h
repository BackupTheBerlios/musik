#ifndef C_MUSIK_PLAYLIST_CTRL_H
#define C_MUSIK_PLAYLIST_CTRL_H

#include "wx/wxprec.h"
#include "MusikListCtrl.h"

#include "images/ratings/rating0.xpm"
#include "images/ratings/rating1.xpm"
#include "images/ratings/rating2.xpm"
#include "images/ratings/rating3.xpm"	
#include "images/ratings/rating4.xpm"
#include "images/ratings/rating5.xpm"

enum E_MUSIK_PLAYLIST_COLUMN_ID
{
	PLAYLISTCOLUMN_RATING = 0,
	PLAYLISTCOLUMN_TRACK,
	PLAYLISTCOLUMN_TITLE,
	PLAYLISTCOLUMN_ARTIST,
	PLAYLISTCOLUMN_ALBUM,
	PLAYLISTCOLUMN_YEAR,
	PLAYLISTCOLUMN_GENRE,
	PLAYLISTCOLUMN_TIMES_PLAYED,
	PLAYLISTCOLUMN_LAST_PLAYED,
	PLAYLISTCOLUMN_TIME,
	PLAYLISTCOLUMN_BITRATE,
	PLAYLISTCOLUMN_FILENAME,
	NPLAYLISTCOLUMNS
};

const int E_MUSIK_PLAYLIST_COLUMN_ALIGN[NPLAYLISTCOLUMNS] =
{
	wxLIST_FORMAT_CENTER,
	wxLIST_FORMAT_LEFT,
	wxLIST_FORMAT_LEFT,
	wxLIST_FORMAT_LEFT,
	wxLIST_FORMAT_LEFT,
	wxLIST_FORMAT_CENTER,
	wxLIST_FORMAT_LEFT,
	wxLIST_FORMAT_RIGHT,
	wxLIST_FORMAT_RIGHT,
	wxLIST_FORMAT_RIGHT,
	wxLIST_FORMAT_RIGHT,
	wxLIST_FORMAT_LEFT	
};

const wxString E_MUSIK_PLAYLIST_COLUMN_LABELS[NPLAYLISTCOLUMNS] =
{
	wxT( "Rating"		),
	wxT( "Track"		),
	wxT( "Title"		),
	wxT( "Artist"		),
	wxT( "Album"		),
	wxT( "Year"			),
	wxT( "Genre"		),
	wxT( "Times Played"	),
	wxT( "Last Played"	),
	wxT( "Time"			),
	wxT( "Bitrate"		),
	wxT( "Filename"		)
};

const wxString E_MUSIK_PLAYLIST_DB_NAMES[NPLAYLISTCOLUMNS] =
{
	wxT( "rating"		),
	wxT( "tracknum"		),
	wxT( "title"		),
	wxT( "artist"		),
	wxT( "album"		),
	wxT( "year"			),
	wxT( "genre"		),
	wxT( "timesplayed"	),
	wxT( "lastplayed"	),
	wxT( "duration"		),
	wxT( "bitrate"		),
	wxT( "filename"		)
};

class CMusikPlaylistCtrl : public CMusikListCtrl
{
public:
	CMusikPlaylistCtrl( wxWindow* parent, wxWindowID id );
	~CMusikPlaylistCtrl();

};

#endif