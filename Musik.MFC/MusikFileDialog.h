///////////////////////////////////////////////////
// 
// Info:
//
//   Musik is a a cross platform, open source
//   multimedia library. More information at:
//
//     http://musik.berlios.de
//     http://musik.sourceforge.net
//
// Copyright and Credits:
//
//   Copyright      : Casey Langen, 2003
//   Casey Langen   : Lead Developer, Project Manager. E-Mail: casey@bak.rr.com
//   Dustin Carter  : Developer, pain in the ass
//   Simon Windmill : Developer, the most helpful person thats immediately available ;)
//
// License:
//
//   See license.txt included with this distribution
//
///////////////////////////////////////////////////
//
// Class(s): 
//
//   CMusikFileDialog
//
// Filename(s): 
//
//   MusikFileDialog.h, MusikFileDialog.cpp
//
// Information:
//
//   Class basically stolen from P.J Arends, see
//   http://www.codeproject.com/dialog/PJA_MultiSelect.asp
//
//   CFileDialog requires a static buffer size to retrieve
//   a list of files. This is not good, becuase we will
//   get an error if we select too many files. This dynamically
//   allocates memory for us so we don't have to worry about it.
//
//   Much thanks to the original author, P.J Arends.
// 
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CMusikFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CMusikFileDialog)

public:
	CMusikFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

	virtual ~CMusikFileDialog();

	// overrides
	virtual int DoModal();
	CString GetNextPathName( POSITION &pos ) const;
	POSITION GetStartPosition();

protected:
	BOOL m_bParsed;
	TCHAR *m_pstrFolder;
	TCHAR *m_pstrFiles;

	virtual void OnFileNameChange();
	void CleanStrings();

	DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////


