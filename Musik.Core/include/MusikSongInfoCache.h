#ifndef C_MUSIK_SONG_INFO_CACHE
#define C_MUSIK_SONG_INFO_CACHE

#include "MusikArrays.h"

class CMusikSongInfoCache
{
public:

	CMusikSongInfoCache( int from, int to, CMusikSongArray* songs )
	{ 
		m_From = from;
		m_To = to;
		//m_Songs = songs;
	}

	~CMusikSongInfoCache();

	int GetFrom()
	{ 
		return m_From; 
	}

	int GetTo()
	{ 
		return m_To; 
	}

	void Set( int from, int to )
	{ 
		if ( from == m_From && to == m_To )
			return;

		m_From = from; 
		m_To = to;
	}

	CMusikSongInfoArray* items(){ return &m_Items; }

private:
	int m_From;
	int m_To;
	CMusikSongInfoArray m_Items;
	CMusikSongArray* m_Songs;
};

#endif