///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
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
// Class(es): 
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
//   MFC's CListCtrl's Cache-Hint mechanism sends
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

#include "musikConfig.h"
#include "musikLibrary.h"

///////////////////////////////////////////////////

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

	// construct and destruct
	CmusikDynDspInfo( CmusikPlaylist* songs, CmusikLibrary* library );
	~CmusikDynDspInfo(){}

	// sets
	void SetPlaylist( CmusikPlaylist* playlist );
	void Set( int from, int to, bool force_update );

	// gets
	CmusikString GetValue( int id, int field );	
	CmusikSongInfoArray* items();
	int GetDistance();
	int GetFirst();
	int GetLast();

	// actions
	bool ResyncItem( int songid );

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
