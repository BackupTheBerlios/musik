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
// Example: 
//
//   If you don't know what an example of an array is,
//   you have issues.   
//
// Usage: 
//
//   Use just as standard std vector arrays. 
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2003
//
//   Casey Langen   : Lead Developer, Project Manager
//   Dustin Carter  : Developer, pain in the ass
//   Simon Windmill : Developer, the most helpful person thats immediately available ;)
// 
// License:
//
//   See license.txt included with this distribution
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

///////////////////////////////////////////////////

typedef std::vector<CStdString> CStdStringArray;
typedef std::vector<CMusikSong> CMusikSongArray;
typedef std::vector<CMusikSongInfo> CMusikSongInfoArray;
typedef std::vector<int> CIntArray;
typedef std::vector<FSOUND_STREAM*> CMusikStreamPtrArray;

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////