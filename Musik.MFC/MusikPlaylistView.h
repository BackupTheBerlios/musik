#pragma once



// CMusikPlaylistView form view

class CMusikPlaylistView : public CFormView
{
	DECLARE_DYNCREATE(CMusikPlaylistView)

protected:
	CMusikPlaylistView();           // protected constructor used by dynamic creation
	virtual ~CMusikPlaylistView();

public:
	enum { IDD = IDD_PLAYLIST_VIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};


