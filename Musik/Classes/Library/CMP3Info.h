#ifndef CMP3INFO_H
#define CMP3INFO_H

//--- globals ---//
#include "../../MusikGlobals.h"


class CMP3Info
{
public:
	CMP3Info();
	bool ReadMetaData(CSongMetaData & MetaData) const;
	bool WriteMetaData(const CSongMetaData & MetaData,bool bClearAll = false);

protected:

private:
};
#endif
