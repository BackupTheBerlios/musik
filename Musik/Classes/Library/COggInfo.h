/*
 *  COggInfo.h
 *
 *  Helper functions to retrieve oggvorbis comments from a given file.
 *
 *  Copyright (c) 2003 Casey Langen (casey@bak.rr.com)
 *	Contributors: Simon Windmill, Dustin Carter, Gunnar Roth, Wade Brainerd
 *
 *  See the file "license.txt" for information on usage and redistribution
 *  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/

#ifndef OGGINFO_H
#define OGGINFO_H

//--- globals ---//
#include "../../MusikGlobals.h"

bool loadOGGInfo( CSongMetaData & MetaData );


#endif
