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
// Example: 
//
//   Notify a UI when the currently playing song has 
//   changed.
//
// Usage: 
//
//   This object is pure virtual, so instantiate a new
//   object from it, overriding the Call() function. When
//   constructing a CMusikPlayer object, send this new 
//   object as the Functor.
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

	~CMusikFunctor()
	{
		// pure virtual
	}

	virtual void Call()
	{
		// pure virtual
	}
};

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
