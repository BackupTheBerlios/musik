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

		// we only have new items at the bottom of 
		// the list
		if ( from == m_From && to != m_To )
		{
			// we have more items to ADD to the
			// bottom of the list
			if ( to > m_To )
			{
				
			}

			// we have items to REMOVE from the
			// bottom of the list
			else if ( to < m_To )
			{

			}

			m_To = to;
			return;
		}

		// we have new items in the top of the
		// list, but not at the bottom
		else if ( to == m_To && from != m_From )
		{
			// we have more items to ADD to the 
			// top
			if ( from > m_From )
			{

			}

			// have have items to REMOVE from 
			// the top
			else if ( from < m_From )
			{
				
			}

			m_From = from;
			return;
		}

		// we have a completely new set of data, so
		else
		{


		}

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