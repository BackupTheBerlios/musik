/*
*  MetaDataHandler.cpp
*
*  Object which handles metadata read and write.
*
*  Uses: id3lib, and ogg/vorbis
*  Information about id3lib is available at http://www.id3lib.org
*  Information about ogg/vorbis is available at http://www.vorbis.com/
*  
*  Copyright (c) 2004 Gunnar Roth (gunnar.roth@gmx.de)
*	Contributors: Simon Windmill, Dustin Carter, Casey Langen, Wade Brainerd
*
*  See the file "license.txt" for information on usage and redistribution
*  of this file, and for a DISCLAIMER OF ALL WARRANTIES.
*/
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
//--- mp3 / ogg helpers ---//
#include "../MusikUtils.h"
#include "CMP3Info.h"
#include "COggInfo.h"
#include "CFMODInfo.h"
#include "MetaDataHandler.h"

static CMP3Info mp3info;
static COggInfo ogginfo;
static CFMODInfo fmodinfo;

static const tSongClass valid_SongClasses[] = 
{
	{wxT("mp3"),MUSIK_FORMAT_MP3,&mp3info,&mp3info}
	,{wxT("ogg"),MUSIK_FORMAT_OGG,&ogginfo,&ogginfo}
	,{wxT("wav"),MUSIK_FORMAT_WAV,&fmodinfo,NULL}
	,{wxT("mp2"),MUSIK_FORMAT_MP2,&mp3info,&mp3info}
	,{wxT("aiff"),MUSIK_FORMAT_AIFF,&fmodinfo,NULL}
	#ifdef __WXMSW__
	,{wxT("wma"),MUSIK_FORMAT_WMA,&fmodinfo,NULL}
#endif
};

const tSongClass * CMetaDataHandler::GetSongClass(const wxString &ext)
{
	for(size_t i = 0; i < WXSIZEOF(valid_SongClasses);i++)
		if(ext.CompareTo(valid_SongClasses[i].szExtension ,wxString::ignoreCase) == 0)
			return &valid_SongClasses[i];
	return NULL;
}
const tSongClass * CMetaDataHandler::GetSongClass(EMUSIK_FORMAT_TYPE eFormat)
{
	for(size_t i = 0; i < WXSIZEOF(valid_SongClasses);i++)
		if(valid_SongClasses[i].eFormat == eFormat)
			return &valid_SongClasses[i];
	return NULL;
}

CMetaDataHandler::RetCode CMetaDataHandler::GetMetaData( CSongMetaData & MetaData  )
{

	//--- get format ---//
	wxString ext = MetaData.Filename.GetExt().MakeLower();
	
	const tSongClass *psc = GetSongClass(ext);
	wxASSERT(psc);
	if(psc == NULL)
		return fail;
	MetaData.eFormat = psc->eFormat;
	RetCode rc = success;
	if(psc->pInfoRead)
	{
		rc = psc->pInfoRead->ReadMetaData(MetaData) ? success:fail;
	}
	else
		rc = notsupported;

	if ( MetaData.Title.Length() == 0 )
		MetaData.Title = ConvToUTF8( MetaData.Filename.GetFullPath() );

	return rc;

}


CMetaDataHandler::RetCode CMetaDataHandler::WriteMetaData(const CSongMetaData &MetaData,bool bClearAll)
{
	const tSongClass *psc = GetSongClass(MetaData.eFormat);
	wxASSERT(psc);
	if(psc == NULL)
		return fail;
	RetCode rc = success;
	if(psc->pInfoWrite)
		rc  = psc->pInfoWrite->WriteMetaData(MetaData,bClearAll)? success:fail;
	else
		rc = notsupported;

	return rc;
}
