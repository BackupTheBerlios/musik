#ifndef C_MUSIK_ARRAYS_H
#define C_MUSIK_ARRAYS_H

#include "StdString.h"
#include "fmod.h"
#include <vector>

class CMusikSong;
class CMusikSongInfo;

typedef std::vector<CStdString> CStdStringArray;
typedef std::vector<CMusikSong> CMusikSongArray;
typedef std::vector<CMusikSongInfo> CMusikSongInfoArray;
typedef std::vector<int> CIntArray;
typedef std::vector<FSOUND_STREAM*> CMusikStreamPtrArray;

#endif