#include "MusikPlaylist.h"

#include "wx/wxprec.h"
#include "wx/arrimpl.cpp"

WX_DEFINE_OBJARRAY( CMusikPlaylist );

CMusikSong::CMusikSong()
{	
	sFilename 		= wxT("");
	sTitle 			= _("<unknown>");
	nTrackNum 		= 0;
	sArtist 		= _("<unknown>");
	sAlbum 			= _("<unknown>");
	sGenre 			= _("<unknown>");
	sYear 			= wxT("");
	sLastPlayed		= wxT("");
	sTimeAdded		= wxT("");
	nFilesize		= 0;
	nFormat 		= 0;
	nDuration		= 0;
	nRating 		= 0;
	nTimesPlayed	= 0;
	nBitrate 		= 0;
	bVBR 			= false;
	sNotes 			= wxT("");
}

