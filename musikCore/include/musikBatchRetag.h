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
//   CmusikBatchRetag
//
// Filename(s): 
//
//   CmusikBatchRetag.h
//
// Information:
//
//   A worker thread for updating tags 
//
// Usage: 
//
//   Create a new CmusikBatchRetagTask and a CmusikBatchRetag,
//   set the specified paramters, then run the task
//   sending the CmusikBatchRetag object as the argument.
//
///////////////////////////////////////////////////

#ifndef C_MUSIK_BATCHRETAG_H
#define C_MUSIK_BATCHRETAG_H

///////////////////////////////////////////////////

#include "musikConfig.h"
#include "musikLibrary.h"
#include "musikFunctor.h"

///////////////////////////////////////////////////

namespace musik {

///////////////////////////////////////////////////

class CmusikBatchRetag
{
public: 

	// construct / destruct
	CmusikBatchRetag();
	CmusikBatchRetag( CmusikLibrary* pLibrary, CmusikFunctor* pFunctor, CmusikSongInfoArray* pUpdatedTags );

	~CmusikBatchRetag(){}

	// we'll allow users to manually set
	// these variables.
	CmusikLibrary* m_Library;
	CmusikFunctor* m_Functor;
	CmusikSongInfoArray* m_UpdatedTags;
	bool m_CallFunctorOnAbort;
	bool m_DeleteUpdatedTags;
	bool m_WriteToFile;
};

///////////////////////////////////////////////////

class CmusikBatchRetagTask : public CmusikTask
{

public:

	// construct / destruct
	CmusikBatchRetagTask();
	~CmusikBatchRetagTask();

	// virtual overrides
	int open( void* params );
	int close( u_long flags );
	int svc();

private:

	CmusikBatchRetag* m_Params;

};

///////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////