/*
 *  MusikTagThreads.h
 *
 *  Threads related to MusikFrame and/or global threads,
 *	such as the crossfader.
 *  
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef MUSIK_THREADS_H
#define MUSIK_THREADS_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

//--- fmod ---//
#include "fmod.h"

class MusikFrame;

//---------------------------------------------------------//
//---- always running thread. it figures out when and 	---//
//---- how to que up the next song. also triggers fades	---//
//---------------------------------------------------------//
class MusikCrossfaderThread;

class MusikFaderThread : public wxThread
{
public:
	MusikFaderThread();

	virtual void *Entry();
	virtual void OnExit();

	void StartNew();

	void CrossfaderAbort		();
	void CrossfaderStop			();
	void WorkerInc				(){ m_Worker++; };
	void WorkerDec				(){ m_Worker--; };
	int  GetWorkerCount			(){ return m_Worker; }

	bool IsCrossfaderActive		(){ return m_CrossfaderActive; }
	void SetCrossfaderActive	( bool active = true );

private:

	int m_Worker;

	MusikCrossfaderThread *pCrossfader;

	int m_CrossfadersActive;
	bool m_CrossfaderActive;
};

//---------------------------------------------------------//
//---- this will fade in the new channel on top of the	---//
//---- the array, while fading *all* the others out		---//
//---------------------------------------------------------//
class MusikCrossfaderThread : public wxThread
{
public:
	MusikCrossfaderThread( MusikFaderThread* pParent );

	void Abort();
	void SetStopPlayer();

	virtual void *Entry();
	virtual void OnExit();
private:
	wxMutex protectingStreamArrays;
	MusikFaderThread *m_Parent;
	bool m_StopPlayer;
	bool m_Aborted;
	size_t m_FadeType;
};

//---------------------------------------------------------//
//--- this thread will write all dirty tags to file...	---//
//--- its placed here for lack of a better place		---//
//---------------------------------------------------------//
class MusikWriteDirtyThread : public wxThread
{
public:
	MusikWriteDirtyThread( bool bClear );

	virtual void *Entry();
	virtual void OnExit();
private:
	bool m_Clear;
};

#endif
