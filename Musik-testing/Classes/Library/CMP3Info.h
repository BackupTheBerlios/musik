#ifndef CMP3INFO_H
#define CMP3INFO_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 

#include "CFrameHeader.h"
//#include "CId3Tag.h"
#include "CVBitRate.h"

/* ----------------------------------------------------------
   CMP3Info class is your complete guide to the 
   MP3 file format in the C++ language. It's a large class
   with three different, quite large "sub-classes" so it's
   a fair amount of code to look into.

   This code will be well commented, so that everyone can
   understand, as it's made for the public and not for
   private use, although private use is allowed. :)

   all functions specified both in the header and .cpp file
   will have explanations in both locations.

   everything here by: Gustav "Grim Reaper" Munkby
                       http://home.swipnet.se/grd/
                       grd@swipnet.se
   ---------------------------------------------------------- */


class CMP3Info {

    public:

    // function to load a file into this structure
    // the argument passed is the path to a MP3 file
    int   loadInfo( wxString srcMP3 );

    // functions used to get information about the "file"
    int   getFileSize() { return fileSize; };
    void  getFileName(char* input);

    // information that is avaliable in FrameHeader & VBR header
    void  getVersion(char* input);
    int   getBitrate();
    int   getFrequency() { return header.getFrequency(); };
    void  getMode(char* input);
    
    int   getNumberOfFrames();

    int   getLengthInSeconds();

    // just to know what kind of file it is.
    bool  isVBitRate() { return VBitRate; };
    bool  isTagged() { return Tagged; };
    
    private:

    // these are the "sub-classes"
    CFrameHeader header;
    CVBitRate    vbr;

    // just to know what kind of file it is
    bool VBitRate;
    bool Tagged;

    // the file information can not be found elsewhere
	wxString fileName;
    int fileSize;

};

#endif
