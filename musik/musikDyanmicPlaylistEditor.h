///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CmusikDyanmicPlaylistEditor : public CDialog
{
public:

	// construct / destruct
	CmusikDyanmicPlaylistEditor(CWnd* pParent = NULL);   // standard constructor
	virtual ~CmusikDyanmicPlaylistEditor();

	// dialog data
	enum { IDD = IDD_DYNPLAYLISTEDITOR };

protected:

	// misc
	virtual void DoDataExchange(CDataExchange* pDX);

	// macros
	DECLARE_DYNAMIC(CmusikDyanmicPlaylistEditor)
	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

