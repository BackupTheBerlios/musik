#pragma once

#include "CPropTree/PropTree.h"

// CMusikSourcesCtrl

class CMusikSourcesCtrl : public CPropTree
{
	DECLARE_DYNAMIC(CMusikSourcesCtrl)

public:
	CMusikSourcesCtrl();
	virtual ~CMusikSourcesCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:


	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


