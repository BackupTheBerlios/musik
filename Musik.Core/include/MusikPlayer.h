#ifndef C_MUSIK_PLAYER_H
#define C_MUSIK_PLAYER_H

#include "boost/thread/mutex.hpp"
#include "boost/thread/thread.hpp"

#include "MusikPlaylist.h"
#include "fmod.h"

class CMusikLibrary;

int thread_main();

class CMusikPlayer : public boost::thread
{
public:
	CMusikPlayer( CMusikLibrary* library );
	~CMusikPlayer();

private:
	CMusikLibrary* m_Library;
};

#endif