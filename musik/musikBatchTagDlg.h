///////////////////////////////////////////////////
// 
// Info:
//
//   musik is a cross platform, open source
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
// Class(es): 
//
//   CmusikBatchTagDlg
//
// Filename(s): 
//
//   musikBatchTagDlg.h, musikBatchTagDlg.cpp
//
// Information:
//
//   Dialog used for batch tag editing
// 
// Usage: 
//
//   Void.
//
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

class CmusikBatchTagDlg : public CDialog
{

public:

	// construct and destruct
	CmusikBatchTagDlg(CWnd* pParent = NULL);
	virtual ~CmusikBatchTagDlg();

	// misc
	enum { IDD = IDD_BATCH_TAG };
	virtual void DoDataExchange(CDataExchange* pDX);

protected:

	// macros
	DECLARE_DYNAMIC(CmusikBatchTagDlg)
	DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////
