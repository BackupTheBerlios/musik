///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CNowPlayingStatic : public CStatic
{
public:

	// construct and destruct
	CNowPlayingStatic();
	virtual ~CNowPlayingStatic();

	// function to set the text, etc
	void SetDynText( const CString& str );
	void UpdateDynSize();


protected:

	// macros
	DECLARE_DYNAMIC(CNowPlayingStatic)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////


