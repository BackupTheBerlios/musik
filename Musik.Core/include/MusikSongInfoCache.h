#ifndef C_MUSIK_SONG_INFO_CACHE
#define C_MUSIK_SONG_INFO_CACHE

#include "MusikArrays.h"
#include "MusikLibrary.h"

class CMusikSongInfoCache
{
public:

	CMusikSongInfoCache( CMusikPlaylist* songs, CMusikLibrary* library)
	{
		m_Songs = songs;
		m_Library = library;
	}
	~CMusikSongInfoCache(){}

	void Set( int from, int to )
	{ 
		if ( from == m_From && to == m_To )
			return;

		m_From = from; 
		m_To = to;
	}

	CMusikSongInfoArray* items()
	{ 
		return &m_Items; 
	}

private:
	int m_From;
	int m_To;
	CMusikSongInfoArray m_Items;
	CMusikPlaylist* m_Songs;
	CMusikLibrary* m_Library;
};

#endif