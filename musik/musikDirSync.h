///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CmusikDirSync : public CDialog
{
public:

	// construct / destruct
	CmusikDirSync( CWnd* pParent = NULL );
	virtual ~CmusikDirSync();

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);

	// macros
	DECLARE_DYNAMIC(CmusikDirSync)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
