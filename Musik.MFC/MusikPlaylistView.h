#pragma once


// CMusikPlaylistView view

class CMusikPlaylistView : public CView
{
	DECLARE_DYNCREATE(CMusikPlaylistView)

protected:
	CMusikPlaylistView();           // protected constructor used by dynamic creation
	virtual ~CMusikPlaylistView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


