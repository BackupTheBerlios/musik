#ifndef MUSIKFX_H
#define MUSIKFX_H

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
	#include "wx/wx.h"
#endif 

#include <wx/config.h> 
#include <wx/confbase.h>
#include <wx/fileconf.h> 

class CMusikFX
{
public:
	void	SetLeftBands( float data[18] );
	void	SetRightBands( float data[18] );
	void	GetLeftBands( float *data );
	void	GetRightBands( float *data );
	void	MakeTable( float samplerate );
	void	ResetBands();
	void	InitEQ();
	void	EndEQ();
	void	ProcessSamples( void *pBuffer, int length, int channels, int bitspersample );

	void	SaveBands( wxFileConfig *pConfig );
	void	LoadBands( wxFileConfig *pConfig );
private:
	float m_LeftBands[18];
	float m_RightBands[18];
};

#endif