#pragma once

class CSplayerButton : public CButton
{
public:
	CSplayerButton();
	virtual ~CSplayerButton();

	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void SetToolTipText(CString s);
	void SetToolTipColor(COLORREF crText, COLORREF crBkgnd);
	COLORREF SetTextColor(COLORREF new_color);
	void SetSkinBmp(UINT normal, UINT down, UINT over = 0, UINT disabled = 0, UINT focus = 0, UINT mask = 0,
		short drawmode = 0, short border = 0, short margin = 0);
	void SetSkinPng(UINT normal, UINT down, UINT over = 0, UINT disabled = 0, UINT focus = 0, UINT mask = 0,
		short drawmode = 0, short border = 0, short margin = 0);
	void SetSkinFromFile(CString normal, CString down, CString over = _T(""), CString disabled = _T(""), CString focus = _T(""), CString mask = _T(""),
		short drawmode = 0, short border = 0, short margin = 0);
	BOOL	LoadFromResource(CImage& img, LPCTSTR pName, LPCTSTR pType);
	HRGN	CreateRgnFromBitmap(HBITMAP hBmp, COLORREF color);
	void	FillWithBitmap(CDC* dc, HBITMAP hbmp, RECT r);
	void	DrawBitmap(CDC* dc, HBITMAP hbmp, RECT r, int DrawMode);
	int		GetBitmapWidth(HBITMAP hBitmap);
	int		GetBitmapHeight(HBITMAP hBitmap);
	void	RelayEvent(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnClicked();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg LRESULT OnRadioInfo(WPARAM, LPARAM);
	afx_msg LRESULT OnBMSetCheck(WPARAM, LPARAM);
	afx_msg LRESULT OnBMGetCheck(WPARAM, LPARAM);

protected:
	bool			m_Checked;				//radio & check buttons
	DWORD			m_Style;				//radio & check buttons
	bool			m_tracking;
	bool			m_button_down;
	CToolTipCtrl	m_tooltip;
	CBitmap			m_bNormal, m_bDown, m_bDisabled, m_bMask, m_bOver, m_bFocus; //skin bitmaps
	short			m_FocusRectMargin;		//dotted margin offset
	COLORREF		m_TextColor;			//button text color
	HRGN			hClipRgn;				//clipping region
	BOOL			m_Border;				//0=flat; 1=3D;
	short			m_DrawMode;				//0=normal; 1=stretch; 2=tiled;
};
