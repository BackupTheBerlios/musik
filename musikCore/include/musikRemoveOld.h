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
//   CmusikBatchAdd
//
// Filename(s): 
//
//   CmusikRemoveOld.h
//
// Information:
//
//   A worker thread for removing missing files 
//   from the library
//
// Usage: 
//
//   Create a new CmusikRemoveOldTask and a CmusikRemoveOld,
//   set the specified paramters, then run the task
//   sending the CmusikRemoveOld object as the argument.
//
///////////////////////////////////////////////////

#ifndef C_MUSIK_REMOVEOLD_H
#define C_MUSIK_REMOVEOLD_H

///////////////////////////////////////////////////

#include "musikConfig.h"
#include "musikLibrary.h"
#include "musikFunctor.h"

///////////////////////////////////////////////////

namespace musik {

///////////////////////////////////////////////////

class CmusikRemoveOld
{
public: 

	// construct / destruct
	CmusikRemoveOld();
	CmusikRemoveOld( CmusikLibrary* pLibrary, CmusikFunctor* pFunctor );

	~CmusikRemoveOld(){}

	// we'll allow users to manually set
	// these variables.
	CmusikLibrary* m_Library;
	CmusikFunctor* m_Functor;
	bool m_CallFunctorOnAbort;
};

///////////////////////////////////////////////////

class CmusikRemoveOldTask : public CmusikTask
{

public:

	CmusikRemoveOldTask();
	~CmusikRemoveOldTask();

	int open( void* params );
	int close( u_long flags );
	int svc();

private:

	CmusikRemoveOld* m_Params;

};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////

