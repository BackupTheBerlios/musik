///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a a cross platform, open source
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
//   CmusikDynDspInfoRange, 
//   CmusikDynDspInfo
//
// Filename(s): 
//
//   musikDynDspInfo.h,
//
// Information:
//
//   This is musik's dynamic display (playlist) view
//   cache. It is designed to be a generic way for any
//   UI to effectively manage memory when dealing with
//   CmusikPlaylist objects. 
// 
// Example: 
//
//   MFC's CListCtrl's Cache-Hint mechanism, which sends
//   a range of items in a control must be managed.
//
// Usage: 
//
//   Construct it with a pointer to a CmusikPlaylist object
//   to be managed, and a CmusikLary objecibrt to query.
//
//   When the range of managed songs is known, pass it via
//   SetRange. The CmusikDynDspInfo() class is now ready
//   to be queryed.
//
///////////////////////////////////////////////////

#ifndef C_MUSIK_SONG_INFO_CACHE
#define C_MUSIK_SONG_INFO_CACHE

///////////////////////////////////////////////////

#include "musikArrays.h"
#include "musikLibrary.h"

///////////////////////////////////////////////////

class CmusikSong;
class CmusikDynDspInfo;

///////////////////////////////////////////////////

class CmusikDynDspInfoRange
{
public:
	CmusikDynDspInfoRange(){ m_First = m_Last = 0; }
	~CmusikDynDspInfoRange(){}

	void Set( int first, int last ){ m_First = first; m_Last = last; }

	int GetDistance(){ return m_Last - m_First; }
	int GetFirst(){ return m_First; }
	int GetLast(){ return m_Last; }
	bool IsNull(){ return ( GetDistance() == 0 ? true : false ); }

	int m_First, m_Last;
};

///////////////////////////////////////////////////

class CmusikDynDspInfo
{
public:

	CmusikDynDspInfo( CmusikPlaylist* songs, CmusikLibrary* library )
	{
		m_Songs = songs;
		m_Library = library;

		m_LastItemRange.Set( -999, -999 );
	}

	~CmusikDynDspInfo()
	{	
	}

	void SetPlaylist( CmusikPlaylist* playlist )
	{
		m_Songs = playlist;
	}

	void Set( int from, int to, bool force_update )
	{ 
		// set the current item range
		m_ItemRange.Set( from, to + 1 );

		// see if our new item range is the same as our 
		// last item range... if so, return
		if ( !force_update )
		{
			if ( m_ItemRange.GetFirst() == m_LastItemRange.GetFirst() &&
				m_ItemRange.GetLast() == m_LastItemRange.GetLast() )
				return;
		}

		// we got a 0,0 range, so check to see if there
		// are any songs in the playlist. if there are
		// not then that means nothing has been selected,
		// and this cache hint was sent even though its not
		// needed. just return.
		if ( m_ItemRange.GetLast() > (int)m_Songs->GetCount() )
			return;

		// grab all the new items
		m_Items.clear();
		CmusikSongInfo item;
		int nID;
		for ( int i = m_ItemRange.GetFirst(); i < m_ItemRange.GetLast(); i++ )
		{
			nID = m_Songs->GetSongID( i );
			m_Library->GetSongInfoFromID( nID, &item );
			m_Items.push_back( item );
		}
		
		// store the last known range, so we
		// can later compare it to anew one
		m_LastItemRange = m_ItemRange;
	}	

	CStdString GetValue( int id, int field )
	{
		if ( id > (int)m_Items.size() || id < 0 )
		{
			#ifdef _DEBUG
				CStdString sErr;
				sErr.Format( "CmusikDynDspInfo failed. Item %d was requested, but there are only %d items\n", id, m_Items.size()-1 );
				TRACE0( sErr.c_str() );
			#endif
			
			return "[musik.cache]";
		}

		return 
			m_Items.at( id ).GetField( field );
	}	

	bool ResyncItem( int songid )
	{
		for ( size_t i = 0; i < m_Items.size(); i++ )
		{
			if ( m_Items.at( i ).GetID() == songid )
			{
				CmusikSongInfo item;
				m_Library->GetSongInfoFromID( songid, &item );
				m_Items.at( i ) = item;
				return true;
			}
		}

		return false;
	}
	
	CmusikSongInfoArray* items()
	{ 
		return &m_Items; 
	}

	int GetDistance()
	{
		return m_ItemRange.GetDistance();
	}

	int GetFirst()
	{
		return m_ItemRange.GetFirst();
	}

	int GetLast()
	{
		return m_ItemRange.GetLast();
	}

	CmusikDynDspInfoRange m_LastItemRange;

private:
	CmusikDynDspInfoRange m_ItemRange;
	CmusikSongInfoArray m_Items;
	CmusikPlaylist* m_Songs;
	CmusikLibrary* m_Library;
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
