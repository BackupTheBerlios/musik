#ifndef MUSIK_FADER_H
#define MUSIK_FADER_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif

//--- fmod ---//
#include "fmod.h"

class MusikCrossfaderThread;

class MusikFaderThread : public wxThread
{
public:
	MusikFaderThread();

	virtual void *Entry();
	virtual void OnExit();

	void CrossfaderAbort();
	void CrossfaderStop();

	bool IsCrossfaderActive(){ return m_CrossfaderActive; }
	void SetCrossfaderActive( bool active = true );

private:
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

	virtual void *Entry();
	virtual void OnExit();
private:
	MusikFaderThread *m_Parent;
	bool m_Aborted;
};

#endif
