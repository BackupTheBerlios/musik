//--- For compilers that support precompilation, includes "wx/wx.h". ---//
#include "wx/wxprec.h"

#include <fstream>
#include <stdio.h>
#include <iostream>
#include <iomanip>

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 

#ifndef WIN32
	#include <streambuf>	// i hate windows
#endif

using namespace std;

#include "CMP3Info.h"
#include "../../MusikUtils.h"
#include "../../MusikGlobals.h"

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

/*
 *  Modified by Casey Langen
 *	If getting variable bitrate fails, fall back to fmod.
 *	This is a Musik only mod.
*/


#define ERR_FILEOPEN    0x0001
#define ERR_NOSUCHFILE  0x0002
#define ERR_NOMP3FILE   0x0004
#define ERR_ID3TAG      0x0008

int CMP3Info::loadInfo( wxString srcMP3 ) {
    
    // open input-file stream to the specified file, name
    ifstream* ifile = new ifstream( ( const char* )ConvFNToFieldMB( srcMP3 ), ios::in | ios::binary /*| ios::nocreate*/);

    if (ifile) { // if the file was opened correctly

        // get file size, by setting the pointer in the end and tell the position
        ifile->seekg(0,ios::end);
        fileSize = ifile->tellg();

        // get srcMP3 into fileName variable
		fileName = srcMP3;
        
        int pos = 0; // current position in file...


        //---************************************************---//
        /* search and load the first frame-header in the file */
        //---************************************************---//
        
        char headerchars[4]; // char variable used for header-loading

        do {
            // if no header has been found after 200kB
            // or the end of the file has been reached
            // then there's probably no mp3-file
            if ( pos>(1024*200) || ifile->eof() ) {
                ifile->close();
                delete ifile;
                return ERR_NOMP3FILE;
            }

            // read in four characters
            ifile->seekg(pos);
            ifile->read (headerchars, 4);

            // move file-position forward
            pos++;
            
            // convert four chars to CFrameHeader structure
            header.loadHeader(headerchars);

        }
        while ( !header.isValidHeader() );  // test for correct header

        // to correct the position to be right after the frame-header
        // we'll need to add another 3 to the current position
        pos += 3;


        //---************************************************---//
        /* check for an vbr-header, to ensure the info from a */
        /* vbr-mp3 is correct                                 */
        //---************************************************---//

        char vbrchars[12];
        
        // determine offset from first frame-header
        // it depends on two things, the mpeg-version
        // and the mode(stereo/mono)

        if( header.getVersionIndex()==3 ) {  // mpeg version 1

            if( header.getModeIndex()==3 ) pos += 17; // Single Channel
            else                           pos += 32;

        } else {                             // mpeg version 2 or 2.5

            if( header.getModeIndex()==3 ) pos +=  9; // Single Channel
            else                           pos += 17;

        }

        // read next twelve bits in
        ifile->seekg(pos);
        ifile->read (vbrchars, 12);

        // turn 12 chars into a CVBitRate class structure
        VBitRate = vbr.loadHeader(vbrchars);        


        //---************************************************---//
        /* get tag from the last 128 bytes in an .mp3-file    */
        //---************************************************---//
        
        char tagchars[128]; // load tag as string

        // get last 128 bytes
        ifile->seekg ( -128, ios::end );
        ifile->read ( tagchars, 128 );

    }
    else {
        ifile->close();
        delete ifile;
        return ERR_NOSUCHFILE;
    }

    ifile->close();
    delete ifile;
    return 0;

}




int CMP3Info::getBitrate() {

    if (VBitRate) {

        // get average frame size by deviding fileSize by the number of frames
        float medFrameSize = (float)fileSize / (float)getNumberOfFrames();
        
        /* Now using the formula for FrameSizes which looks different,
           depending on which mpeg version we're using, for mpeg v1:
        
           FrameSize = 12 * BitRate / SampleRate + Padding (if there is padding)

           for mpeg v2 the same thing is:

           FrameSize = 144 * BitRate / SampleRate + Padding (if there is padding)

           remember that bitrate is in kbps and sample rate in Hz, so we need to
           multiply our BitRate with 1000.

           For our purpose, just getting the average frame size, will make the
           padding obsolete, so our formula looks like:

           FrameSize = (mpeg1?12:144) * 1000 * BitRate / SampleRate;
        */

        return (int)( 
                     ( medFrameSize * (float)header.getFrequency() ) / 
                     ( 1000.0 * ( (header.getLayerIndex()==3) ? 12.0 : 144.0))
                    );

    }
    else return header.getBitrate();

}

int CMP3Info::getLengthInSeconds() {

    // kiloBitFileSize to match kiloBitPerSecond in bitrate...
    int kiloBitFileSize = (8 * fileSize) / 1000;
	int mp3BitRate = getBitrate();

	if ( mp3BitRate )
		return (int)(kiloBitFileSize/getBitrate());
	else
		return g_Player.GetFileDuration( fileName, FMOD_SEC );
}



int CMP3Info::getNumberOfFrames() {

    if (!VBitRate) {

        /* Now using the formula for FrameSizes which looks different,
           depending on which mpeg version we're using, for layer 1:
        
           FrameSize = 12 * BitRate / SampleRate + Padding (if there is padding)

           for layer 2 & 3 the same thing is:

           FrameSize = 144 * BitRate / SampleRate + Padding (if there is padding)

           remember that bitrate is in kbps and sample rate in Hz, so we need to
           multiply our BitRate with 1000.

           For our purpose, just getting the average frame size, will make the
           padding obsolete, so our formula looks like:

           FrameSize = (layer1?12:144) * 1000 * BitRate / SampleRate;
        */
           
        float medFrameSize = (float)( 
                                     ( (header.getLayerIndex()==3) ? 12 : 144 ) *
                                     (
                                      (1000.0 * (float)header.getBitrate() ) /
                                      (float)header.getFrequency()
                                     )
                                    );
        
        return (int)(fileSize/medFrameSize);

    }
    else return vbr.getNumberOfFrames();

}

void CMP3Info::getVersion(char* input) 
{

    char versionchar[32]; // temporary string
    char tempchar2[4]; // layer

    // call CFrameHeader member function
    float ver = header.getVersion();

    // create the layer information with the amounts of I
    for( int n = 0; n < header.getLayer(); n++ ) 
    {
		tempchar2[n] = 'I';
		if ( n == header.getLayer() - 1 )
			tempchar2[n+1] = '\0';
    }
    // combine strings
    sprintf(versionchar,"MPEG %g Layer %s", (double)ver, tempchar2);

    // copy result into inputstring
    strcpy(input, versionchar);

}

void CMP3Info::getMode(char* input) {

    char modechar[32]; // temporary string

    // call CFrameHeader member function
    header.getMode(modechar);

    // copy result into inputstring
    strcpy(input, modechar);

}
