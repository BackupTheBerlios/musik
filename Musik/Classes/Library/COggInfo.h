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

#include <wx/filename.h>
//--- globals ---//
#include "../../MusikGlobals.h"
#include <vorbis/vorbisfile.h>
#include <map>

class COggInfo
{
public:
	COggInfo();
	bool ReadMetaData(CSongMetaData & MetaData) const;
	bool WriteMetaData(const CSongMetaData & MetaData,bool bClearAll = false);

protected:
	class CVCTagMap
	{
	public:
		void AddTagsFromVC( vorbis_comment *pComment );
		void CopyTagsToVC( vorbis_comment *pComment );
		void AddTag( const char *tag,const char *val, bool bUnique = true );
	protected:
	private:
		typedef std::multimap<wxString ,CSongMetaData::StringData> tagmap_t;
		tagmap_t m_mapTags;
	};
	
private:
};



#endif
