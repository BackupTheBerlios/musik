///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CmusikDynamicPlaylistEditor : public CDialog
{
public:

	// construct and destruct
	CmusikDynamicPlaylistEditor(CWnd* pParent = NULL);
	virtual ~CmusikDynamicPlaylistEditor();

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);

	// macros
	DECLARE_DYNAMIC(CmusikDynamicPlaylistEditor)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
