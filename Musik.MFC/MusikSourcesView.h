#pragma once



// CMusikSourcesView form view

class CMusikSourcesView : public CFormView
{
	DECLARE_DYNCREATE(CMusikSourcesView)

protected:
	CMusikSourcesView();           // protected constructor used by dynamic creation
	virtual ~CMusikSourcesView();

public:
	enum { IDD = IDD_SOURCES_VIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};


