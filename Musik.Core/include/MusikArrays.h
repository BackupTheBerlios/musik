///////////////////////////////////////////////////
// 
// Info:
//
//   Musik is a a cross platform, open source
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
//   CStdStringArray,
//   CMusikSongArray,
//   CMusikSongInfoArray,
//   CIntArray,
//   CMusikStreamPtrArray
//
// Filename(s): 
//
//   MusikArrays.h,
//
// Information:
//
//   Some stl vector arrays for objects commonly used.
//   Using dynamic arrays helps to keep things easily
//   maintained.
//
// Usage: 
//
//   Use just as standard std vector arrays. 
//
///////////////////////////////////////////////////

#ifndef C_MUSIK_ARRAYS_H
#define C_MUSIK_ARRAYS_H

///////////////////////////////////////////////////

#include "StdString.h"
#include "fmod.h"
#include <vector>

///////////////////////////////////////////////////

class CMusikSong;
class CMusikSongInfo;
class CMusikPlaylist;
class CMusikPlaylistInfo;

///////////////////////////////////////////////////

typedef std::vector<CStdString> CStdStringArray;
typedef std::vector<CMusikSong> CMusikSongArray;
typedef std::vector<CMusikSongInfo> CMusikSongInfoArray;
typedef std::vector<CMusikPlaylist> CMusikPlaylistArray;
typedef std::vector<CMusikPlaylistInfo> CMusikPlaylistInfoArray;
typedef std::vector<int> CIntArray;
typedef std::vector<FSOUND_STREAM*> CMusikStreamPtrArray;

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
