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
//   CMusikFunctor
//
// Filename(s): 
//
//   MusikFunctor.h,
//
// Information:
//
//   This class provides a generic way for CMusikPlayer
//   to pass events to a UI.
// 
// Usage: 
//
//   This can be used to notify a UI when the currently 
//   playing song has changed.
//
//   This object is pure virtual, so instantiate a new
//   object from it, overriding the Call() function. When
//   constructing a CMusikPlayer object, send this new 
//   object as the Functor.
//
///////////////////////////////////////////////////

#ifndef C_MUSIK_FUNCTOR
#define C_MUSIK_FUNCTOR

///////////////////////////////////////////////////

class CMusikFunctor
{
public:
	CMusikFunctor()
	{
		// pure virtual
	};

	virtual ~CMusikFunctor()
	{
		// pure virtual
	}

	virtual void OnNewSong()
	{
		// pure virtual
	}

	virtual void OnThreadStart()
	{
		// pure virtual
	}

	virtual void OnThreadEnd()
	{
		// pure virtual
	}

	virtual void OnThreadProgress()
	{
		// pure virtual
	}

	virtual void OnThreadNewFile()
	{
		// pure virtual
	}
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
