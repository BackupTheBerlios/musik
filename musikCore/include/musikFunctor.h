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
//   CmusikFunctor
//
// Filename(s): 
//
//   musikFunctor.h,
//
// Information:
//
//   This class provides a generic way for CmusikPlayer
//   to pass events to a UI.
// 
// Usage: 
//
//   This can be used to notify a UI when the currently 
//   playing song has changed.
//
//   This object is pure virtual, so instantiate a new
//   object from it, overriding the Call() function. When
//   constructing a CmusikPlayer object, send this new 
//   object as the Functor.
//
///////////////////////////////////////////////////

#ifndef C_MUSIK_FUNCTOR
#define C_MUSIK_FUNCTOR

///////////////////////////////////////////////////

class CmusikFunctor
{
public:
	CmusikFunctor()
	{
		// pure virtual
	};

	virtual ~CmusikFunctor()
	{
		// pure virtual
	}

	virtual void OnNewSong()
	{
		// pure virtual
	}

	virtual void OnPause()
	{
		// pure virtual
	}

	virtual void OnResume()
	{
		// pure virtual
	}

	virtual void OnStop()
	{
		// pure virtual
	}

	virtual void OnThreadStart()
	{
		// pure virtual
	}

	virtual void OnThreadEnd( void* thr_addr = NULL )
	{
		// pure virtual
	}

	virtual void OnThreadProgress( size_t progress )
	{
		// pure virtual
	}

	virtual void OnThreadNewFile( size_t currfile, size_t allfiles )
	{
		// pure virtual
	}

	virtual bool VerifyPlaylist( void* pl_addr = NULL )
	{
		return true;
	}
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
