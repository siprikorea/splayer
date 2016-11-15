#include "stdafx.h"
#include "splayerbtn.h"
#include "resource.h"
#include <atlimage.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define WM_CXSHADE_RADIO WM_USER+0x100


BEGIN_MESSAGE_MAP(CSplayerButton, CButton)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_CXSHADE_RADIO, OnRadioInfo)
	ON_MESSAGE(BM_SETCHECK, OnBMSetCheck)
	ON_MESSAGE(BM_GETCHECK, OnBMGetCheck)
END_MESSAGE_MAP()


CSplayerButton::CSplayerButton()
{
	m_DrawMode = 1;			// normal drawing mode
	m_FocusRectMargin = 0;	// disable focus dotted rect
	hClipRgn = NULL;			// no clipping region
	m_TextColor = GetSysColor(COLOR_BTNTEXT); // default button text color
	m_button_down = m_tracking = m_Checked = false;
}


CSplayerButton::~CSplayerButton()
{
	if (hClipRgn) DeleteObject(hClipRgn);	// free clip region
}


void CSplayerButton::PreSubclassWindow()
{
	m_Style = GetButtonStyle();
	if ((m_Style & BS_AUTOCHECKBOX) == BS_AUTOCHECKBOX)
		m_Style = BS_CHECKBOX;
	else if ((m_Style & BS_AUTORADIOBUTTON) == BS_AUTORADIOBUTTON)
		m_Style = BS_RADIOBUTTON;
	else
		m_Style = BS_PUSHBUTTON;

	CButton::PreSubclassWindow();
	ModifyStyle(0, BS_OWNERDRAW);
}


BOOL CSplayerButton::OnEraseBkgnd(CDC* pDC)
{
	return 1;
}	// doesn't erase the button background


void CSplayerButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct);
	//TRACE("* Captured: %08X\n", ::GetCapture());

	//Check if the button state in not in inconsistent mode...
	POINT mouse_position;
	if ((m_button_down) && (::GetCapture() == m_hWnd) && (::GetCursorPos(&mouse_position))) {
		if (::WindowFromPoint(mouse_position) == m_hWnd) {
			if ((GetState() & BST_PUSHED) != BST_PUSHED) {
				//TRACE("* Inconsistency up detected! Fixing.\n");
				SetState(TRUE);
				return;
			}
		}
		else {
			if ((GetState() & BST_PUSHED) == BST_PUSHED) {
				//TRACE("* Inconsistency up detected! Fixing.\n");
				SetState(FALSE);
				return;
			}
		}
	}

	//TRACE("* Drawing: %08x\n", lpDrawItemStruct->itemState);
	CString sCaption;
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);	// get device context
	RECT r = lpDrawItemStruct->rcItem;					// context rectangle
	int cx = r.right - r.left;						// get width
	int cy = r.bottom - r.top;						// get height
													// get text box position
	RECT tr = { r.left + m_FocusRectMargin + 2,r.top,r.right - m_FocusRectMargin - 2 - 11,r.bottom - 11 };

	GetWindowText(sCaption);							// get button text
	pDC->SetBkMode(TRANSPARENT);

	CFont* pOldFont = pDC->SelectObject(GetFont());

	// Select the correct skin 
	if (lpDrawItemStruct->itemState & ODS_DISABLED) {	// DISABLED BUTTON
		if (m_bDisabled.m_hObject == NULL)
			// no skin selected for disabled state -> standard button
			pDC->FillSolidRect(&r, GetSysColor(COLOR_BTNFACE));
		else // paint the skin
			DrawBitmap(pDC, (HBITMAP)m_bDisabled, r, m_DrawMode);
		// if needed, draw the standard 3D rectangular border
		if (m_Border) pDC->DrawEdge(&r, EDGE_RAISED, BF_RECT);
		// paint the etched button text
		pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
		pDC->DrawText(sCaption, &tr, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	}
	else {										// SELECTED (DOWN) BUTTON
		if ((lpDrawItemStruct->itemState & ODS_SELECTED) || m_Checked) {
			if (m_bDown.m_hObject == NULL)
				// no skin selected for selected state -> standard button
				pDC->FillSolidRect(&r, GetSysColor(COLOR_BTNFACE));
			else { // paint the skin
				DrawBitmap(pDC, (HBITMAP)m_bDown, r, m_DrawMode);
			}
			// if needed, draw the standard 3D rectangular border
			if (m_Border) pDC->DrawEdge(&r, EDGE_SUNKEN, BF_RECT);
			pDC->SetTextColor(RGB(255, 255, 255));
		}
		else {											// DEFAULT BUTTON
			if (m_bNormal.m_hObject == NULL)
				// no skin selected for normal state -> standard button
				pDC->FillSolidRect(&r, GetSysColor(COLOR_BTNFACE));
			else // paint the skin
				if ((m_tracking) && (m_bOver.m_hObject != NULL)) {
					DrawBitmap(pDC, (HBITMAP)m_bOver, r, m_DrawMode);
				}
				else {
					if ((lpDrawItemStruct->itemState & ODS_FOCUS) && (m_bFocus.m_hObject != NULL)) {
						DrawBitmap(pDC, (HBITMAP)m_bFocus, r, m_DrawMode);
					}
					else {
						DrawBitmap(pDC, (HBITMAP)m_bNormal, r, m_DrawMode);
					}
				}
				// if needed, draw the standard 3D rectangular border
				if (m_Border) pDC->DrawEdge(&r, EDGE_RAISED, BF_RECT);
				pDC->SetTextColor(m_TextColor);
		}
		// paint the focus rect
		if ((lpDrawItemStruct->itemState & ODS_FOCUS) && (m_FocusRectMargin>0)) {
			r.left += m_FocusRectMargin;
			r.top += m_FocusRectMargin;
			r.right -= m_FocusRectMargin;
			r.bottom -= m_FocusRectMargin;
			DrawFocusRect(lpDrawItemStruct->hDC, &r);
		}
		// paint the enabled button text
		pDC->DrawText(sCaption, &tr, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	}

	pDC->SelectObject(pOldFont);
}


int CSplayerButton::GetBitmapWidth(HBITMAP hBitmap)
{
	BITMAP bm; GetObject(hBitmap, sizeof(BITMAP), (PSTR)&bm); return bm.bmWidth;
}


int CSplayerButton::GetBitmapHeight(HBITMAP hBitmap)
{
	BITMAP bm; GetObject(hBitmap, sizeof(BITMAP), (PSTR)&bm); return bm.bmHeight;
}


void CSplayerButton::DrawBitmap(CDC* dc, HBITMAP hbmp, RECT r, int DrawMode)
{
	//	DrawMode: 0=Normal; 1=stretch; 2=tiled fill
	if (DrawMode == 2) {
		FillWithBitmap(dc, hbmp, r);
		return;
	}
	if (!hbmp) return;	//safe check

	int cx = r.right - r.left;
	int cy = r.bottom - r.top;
	CDC dcBmp, dcMask;
	dcBmp.CreateCompatibleDC(dc);
	HBITMAP hOldBitmap = (HBITMAP)dcBmp.SelectObject(hbmp);

	if (m_bMask.m_hObject != NULL) {
		dcMask.CreateCompatibleDC(dc);
		HBITMAP hOldBitmap1 = (HBITMAP)dcMask.SelectObject(m_bMask);

		CDC hdcMem;
		hdcMem.CreateCompatibleDC(dc);
		CBitmap hBitmap;
		hBitmap.CreateCompatibleBitmap(dc, cx, cy);
		HBITMAP hOldBitmap2 = (HBITMAP)hdcMem.SelectObject(hBitmap);

		hdcMem.BitBlt(r.left, r.top, cx, cy, dc, 0, 0, SRCCOPY);
		if (!DrawMode) {
			hdcMem.BitBlt(r.left, r.top, cx, cy, &dcBmp, 0, 0, SRCINVERT);
			hdcMem.BitBlt(r.left, r.top, cx, cy, &dcMask, 0, 0, SRCAND);
			hdcMem.BitBlt(r.left, r.top, cx, cy, &dcBmp, 0, 0, SRCINVERT);
		}
		else {
			int bx = GetBitmapWidth(hbmp);
			int by = GetBitmapHeight(hbmp);
			hdcMem.StretchBlt(r.left, r.top, cx, cy, &dcBmp, 0, 0, bx, by, SRCINVERT);
			hdcMem.StretchBlt(r.left, r.top, cx, cy, &dcMask, 0, 0, bx, by, SRCAND);
			hdcMem.StretchBlt(r.left, r.top, cx, cy, &dcBmp, 0, 0, bx, by, SRCINVERT);
		}
		dc->BitBlt(r.left, r.top, cx, cy, &hdcMem, 0, 0, SRCCOPY);

		hdcMem.SelectObject(hOldBitmap2);
		hdcMem.DeleteDC();
		hBitmap.DeleteObject();

		dcMask.SelectObject(hOldBitmap1);
		dcMask.DeleteDC();
	}
	else {
		if (!DrawMode) {
			dc->BitBlt(r.left, r.top, cx, cy, &dcBmp, 0, 0, SRCCOPY);
		}
		else {
			int bx = GetBitmapWidth(hbmp);
			int by = GetBitmapHeight(hbmp);
			dc->StretchBlt(r.left, r.top, cx, cy, &dcBmp, 0, 0, bx, by, SRCCOPY);
		}
	}

	dcBmp.SelectObject(hOldBitmap);
	dcBmp.DeleteDC();
}


void CSplayerButton::FillWithBitmap(CDC* dc, HBITMAP hbmp, RECT r)
{
	if (!hbmp) return;
	CDC memdc;
	memdc.CreateCompatibleDC(dc);
	HBITMAP hOldBitmap = (HBITMAP)memdc.SelectObject(hbmp);
	int w = r.right - r.left;
	int	h = r.bottom - r.top;
	int x, y, z;
	int	bx = GetBitmapWidth(hbmp);
	int	by = GetBitmapHeight(hbmp);
	for (y = r.top; y < h; y += by) {
		if ((y + by)>h) by = h - y;
		z = bx;
		for (x = r.left; x < w; x += z) {
			if ((x + z)>w) z = w - x;
			dc->BitBlt(x, y, z, by, &memdc, 0, 0, SRCCOPY);
		}
	}
	memdc.SelectObject(hOldBitmap);
	memdc.DeleteDC();
}


void CSplayerButton::SetSkinBmp(UINT normal, UINT down, UINT over, UINT disabled, UINT focus, UINT mask,
	short drawmode, short border, short margin)
{
	m_bNormal.DeleteObject();	//free previous allocated bitmap
	m_bDown.DeleteObject();
	m_bOver.DeleteObject();
	m_bDisabled.DeleteObject();
	m_bFocus.DeleteObject();
	m_bMask.DeleteObject();

	if (normal>0) m_bNormal.LoadBitmap(normal);
	if (down>0)	  m_bDown.LoadBitmap(down);
	if (over>0)	  m_bOver.LoadBitmap(over);
	if (disabled>0) m_bDisabled.LoadBitmap(disabled);
	if (focus>0)  m_bFocus.LoadBitmap(focus);
	if (mask>0) m_bMask.LoadBitmap(mask);

	m_DrawMode = max(0, min(drawmode, 2));
	m_Border = border;
	m_FocusRectMargin = max(0, margin);

	if (mask>0) {
		if (hClipRgn)
			DeleteObject(hClipRgn);

		hClipRgn = CreateRgnFromBitmap(m_bMask, RGB(255, 255, 255));
		if (hClipRgn) {
			SetWindowRgn(hClipRgn, TRUE);
			CDC *pDC = GetDC();
			if (pDC) {
				SelectClipRgn(pDC->GetSafeHdc(), hClipRgn);
				ReleaseDC(pDC);
			}
		}
		if (m_DrawMode == 0) {
			SetWindowPos(NULL, 0, 0, GetBitmapWidth(m_bMask),
				GetBitmapHeight(m_bMask), SWP_NOZORDER | SWP_NOMOVE);
		}
	}
}


void CSplayerButton::SetSkinPng(UINT normal, UINT down, UINT over, UINT disabled, UINT focus, UINT mask,
	short drawmode, short border, short margin)
{
	m_bNormal.DeleteObject();	//free previous allocated bitmap
	m_bDown.DeleteObject();
	m_bOver.DeleteObject();
	m_bDisabled.DeleteObject();
	m_bFocus.DeleteObject();
	m_bMask.DeleteObject();

	CImage imgNormal;
	CImage imgDown;
	CImage imgOver;
	CImage imgDisabled;
	CImage imgFocus;
	CImage imgMask;

	LoadFromResource(imgNormal, (LPCTSTR)MAKEINTRESOURCE(IDB_PLAY), _T("PNG"));
	LoadFromResource(imgDown, (LPCTSTR)MAKEINTRESOURCE(IDB_PLAY), _T("PNG"));

	if (normal > 0)		m_bNormal.Attach(imgNormal.Detach());
	if (down > 0)		m_bDown.Attach(imgDown.Detach());
	if (over > 0)		m_bOver.Attach(imgOver.Detach());
	if (disabled > 0)	m_bDisabled.Attach(imgDisabled.Detach());
	if (focus > 0)		m_bFocus.Attach(imgFocus.Detach());
	if (mask > 0)		m_bMask.Attach(imgMask.Detach());

	m_DrawMode = max(0, min(drawmode, 2));
	m_Border = border;
	m_FocusRectMargin = max(0, margin);

	if (mask>0) {
		if (hClipRgn) DeleteObject(hClipRgn);
		hClipRgn = CreateRgnFromBitmap(m_bMask, RGB(255, 255, 255));
		if (hClipRgn) {
			SetWindowRgn(hClipRgn, TRUE);
			CDC *pDC = GetDC();
			if (pDC) {
				SelectClipRgn(pDC->GetSafeHdc(), hClipRgn);
				ReleaseDC(pDC);
			}
		}
		if (m_DrawMode == 0) {
			SetWindowPos(NULL, 0, 0, GetBitmapWidth(m_bMask),
				GetBitmapHeight(m_bMask), SWP_NOZORDER | SWP_NOMOVE);
		}
	}
}


BOOL CSplayerButton::LoadFromResource(CImage& img, LPCTSTR pName, LPCTSTR pType)
{
	HINSTANCE hInst = AfxGetInstanceHandle();

	IStream* pStream = NULL;
	HRSRC hResource = ::FindResource(hInst, pName, pType); if (!hResource) return FALSE;
	DWORD nImageSize = ::SizeofResource(hInst, hResource); if (!nImageSize) return FALSE;

	HGLOBAL m_hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, nImageSize);
	BYTE* pBytes = (BYTE*)::LoadResource(hInst, hResource);
	if (m_hBuffer)
	{
		void* pSource = ::LockResource(::LoadResource(hInst, hResource));
		if (!pSource) return FALSE;
		void* pDest = ::GlobalLock(m_hBuffer);
		if (pDest)
		{
			CopyMemory(pDest, pSource, nImageSize);
			if (::CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream) == S_OK)
			{
				img.Load(pStream); // 여기만 수정 하면
				pStream->Release();
			}
			::GlobalUnlock(m_hBuffer);
		}
		::GlobalFree(m_hBuffer);
		m_hBuffer = NULL;
	}
	if (img.IsNull()) return FALSE;
	return TRUE;
}


void CSplayerButton::SetSkinFromFile(CString normal, CString down, CString over, CString disabled, CString focus, CString mask,
	short drawmode, short border, short margin)
{
	m_bNormal.DeleteObject();	//free previous allocated bitmap
	m_bDown.DeleteObject();
	m_bOver.DeleteObject();
	m_bDisabled.DeleteObject();
	m_bMask.DeleteObject();
	m_bFocus.DeleteObject();

	if (!normal.IsEmpty()) { CImage img; img.Load(normal); m_bNormal.Attach(img.Detach()); }
	if (!down.IsEmpty()) { CImage img; img.Load(down); m_bDown.Attach(img.Detach()); }
	if (!over.IsEmpty()) { CImage img; img.Load(over); m_bOver.Attach(img.Detach()); }
	if (!focus.IsEmpty()) { CImage img; img.Load(focus); m_bFocus.Attach(img.Detach()); }

	if (!disabled.IsEmpty()) { CImage img; img.Load(disabled); m_bDisabled.Attach(img.Detach()); }
	else if (!normal.IsEmpty()) { CImage img; img.Load(normal); m_bDisabled.Attach(img.Detach()); }

	m_DrawMode = max(0, min(drawmode, 2));
	m_Border = border;
	m_FocusRectMargin = max(0, margin);

	if (!mask.IsEmpty()) {
		{ CImage img; img.Load(mask); m_bMask.Attach(img.Detach()); }
		if (hClipRgn) DeleteObject(hClipRgn);
		hClipRgn = CreateRgnFromBitmap(m_bMask, RGB(255, 255, 255));
		if (hClipRgn) {
			SetWindowRgn(hClipRgn, TRUE);
			CDC *pDC = GetDC();
			if (pDC) {
				SelectClipRgn(pDC->GetSafeHdc(), hClipRgn);
				ReleaseDC(pDC);
			}
		}
		if (m_DrawMode == 0) {
			SetWindowPos(NULL, 0, 0, GetBitmapWidth(m_bMask),
				GetBitmapHeight(m_bMask), SWP_NOZORDER | SWP_NOMOVE);
		}
	}
}


HRGN CSplayerButton::CreateRgnFromBitmap(HBITMAP hBmp, COLORREF color)
{
	if (!hBmp) return NULL;

	BITMAP bm;
	GetObject(hBmp, sizeof(BITMAP), &bm);	// get bitmap attributes

	CDC dcBmp;
	CDC *pDC = GetDC();
	if (!pDC) return NULL;
	dcBmp.CreateCompatibleDC(pDC);	//Creates a memory device context for the bitmap
	ReleaseDC(pDC);
	HBITMAP hOldBitmap = (HBITMAP)dcBmp.SelectObject(hBmp);	//selects the bitmap in the device context

	const DWORD RDHDR = sizeof(RGNDATAHEADER);
	const DWORD MAXBUF = 40;		// size of one block in RECTs
									// (i.e. MAXBUF*sizeof(RECT) in bytes)
	LPRECT	pRects;
	DWORD	cBlocks = 0;			// number of allocated blocks

	INT		i, j;					// current position in mask image
	INT		first = 0;				// left position of current scan line
									// where mask was found
	bool	wasfirst = false;		// set when if mask was found in current scan line
	bool	ismask;					// set when current color is mask color

									// allocate memory for region data
	RGNDATAHEADER* pRgnData = (RGNDATAHEADER*)new BYTE[RDHDR + ++cBlocks * MAXBUF * sizeof(RECT)];
	memset(pRgnData, 0, RDHDR + cBlocks * MAXBUF * sizeof(RECT));
	// fill it by default
	pRgnData->dwSize = RDHDR;
	pRgnData->iType = RDH_RECTANGLES;
	pRgnData->nCount = 0;
	for (i = 0; i < bm.bmHeight; i++)
		for (j = 0; j < bm.bmWidth; j++) {
			// get color
			ismask = (dcBmp.GetPixel(j, bm.bmHeight - i - 1) != color);
			// place part of scan line as RECT region if transparent color found after mask color or
			// mask color found at the end of mask image
			if (wasfirst && ((ismask && (j == (bm.bmWidth - 1))) || (ismask ^ (j<bm.bmWidth)))) {
				// get offset to RECT array if RGNDATA buffer
				pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
				// save current RECT
				pRects[pRgnData->nCount++] = CRect(first, bm.bmHeight - i - 1, j + (j == (bm.bmWidth - 1)), bm.bmHeight - i);
				// if buffer full reallocate it
				if (pRgnData->nCount >= cBlocks * MAXBUF) {
					LPBYTE pRgnDataNew = new BYTE[RDHDR + ++cBlocks * MAXBUF * sizeof(RECT)];
					memcpy(pRgnDataNew, pRgnData, RDHDR + (cBlocks - 1) * MAXBUF * sizeof(RECT));
					delete pRgnData;
					pRgnData = (RGNDATAHEADER*)pRgnDataNew;
				}
				wasfirst = false;
			}
			else if (!wasfirst && ismask) {		// set wasfirst when mask is found
				first = j;
				wasfirst = true;
			}
		}
	dcBmp.SelectObject(hOldBitmap);
	dcBmp.DeleteDC();	//release the bitmap
						// create region
						/*  Under WinNT the ExtCreateRegion returns NULL (by Fable@aramszu.net) */
						//	HRGN hRgn = ExtCreateRegion( NULL, RDHDR + pRgnData->nCount * sizeof(RECT), (LPRGNDATA)pRgnData );
						/* ExtCreateRegion replacement { */
	HRGN hRgn = CreateRectRgn(0, 0, 0, 0);
	ASSERT(hRgn != NULL);
	pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
	for (i = 0;i<(int)pRgnData->nCount;i++)
	{
		HRGN hr = CreateRectRgn(pRects[i].left, pRects[i].top, pRects[i].right, pRects[i].bottom);
		VERIFY(CombineRgn(hRgn, hRgn, hr, RGN_OR) != ERROR);
		if (hr) DeleteObject(hr);
	}
	ASSERT(hRgn != NULL);
	/* } ExtCreateRegion replacement */

	delete pRgnData;
	return hRgn;
}


COLORREF CSplayerButton::SetTextColor(COLORREF new_color)
{
	COLORREF tmp_color = m_TextColor;
	m_TextColor = new_color;
	return tmp_color;			//returns the previous color
}


void CSplayerButton::SetToolTipText(CString s)
{
	if (m_tooltip.m_hWnd == NULL) {
		if (m_tooltip.Create(this))	//first assignment
			if (m_tooltip.AddTool(this, (LPCTSTR)s))
				m_tooltip.Activate(1);
	}
	else {
		m_tooltip.UpdateTipText((LPCTSTR)s, this);
	}
}


void CSplayerButton::SetToolTipColor(COLORREF crText, COLORREF crBkgnd)
{
	if (m_tooltip.m_hWnd != NULL)
	{
		m_tooltip.SetTipTextColor(crText);
		m_tooltip.SetTipBkColor(crBkgnd);
	}
}


void CSplayerButton::RelayEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// This function will create a MSG structure, fill it in a pass it to
	// the ToolTip control, m_ttip.  Note that we ensure the point is in window
	// coordinates (relative to the control's window).
	if (NULL != m_tooltip.m_hWnd) {
		MSG msg;
		msg.hwnd = m_hWnd;
		msg.message = message;
		msg.wParam = wParam;
		msg.lParam = lParam;
		msg.time = 0;
		msg.pt.x = LOWORD(lParam);
		msg.pt.y = HIWORD(lParam);

		m_tooltip.RelayEvent(&msg);
	}
}


void CSplayerButton::OnLButtonDblClk(UINT flags, CPoint point)
{
	SendMessage(WM_LBUTTONDOWN, flags, MAKELPARAM(point.x, point.y));
}


void CSplayerButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	//TRACE("* %08X: down\n", ::GetTickCount());

	//Pass this message to the ToolTip control
	RelayEvent(WM_LBUTTONDOWN, (WPARAM)nFlags, MAKELPARAM(LOWORD(point.x), LOWORD(point.y)));

	//If we are tracking this button, cancel it
	if (m_tracking) {
		TRACKMOUSEEVENT t = {
			sizeof(TRACKMOUSEEVENT),
			TME_CANCEL | TME_LEAVE,
			m_hWnd,
			0
		};
		if (::_TrackMouseEvent(&t)) {
			m_tracking = false;
		}
	}

	//Default-process the message
	CButton::OnLButtonDown(nFlags, point);
	m_button_down = true;
}


void CSplayerButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	//TRACE("* %08X: up\n", ::GetTickCount());

	if (m_Style) { //track mouse for radio & check buttons
		POINT p2 = point;
		::ClientToScreen(m_hWnd, &p2);
		HWND mouse_wnd = ::WindowFromPoint(p2);
		if (mouse_wnd == m_hWnd) { // mouse is in button
			if (m_Style == BS_CHECKBOX)
				SetCheck(m_Checked ? 0 : 1);
			if (m_Style == BS_RADIOBUTTON)
				SetCheck(1);
		}
	}
	//Pass this message to the ToolTip control
	RelayEvent(WM_LBUTTONUP, (WPARAM)nFlags, MAKELPARAM(LOWORD(point.x), LOWORD(point.y)));

	//Default-process the message
	m_button_down = false;
	CButton::OnLButtonUp(nFlags, point);
}


void CSplayerButton::OnMouseMove(UINT nFlags, CPoint point)
{
	//TRACE("* %08X: Mouse\n", ::GetTickCount());

	//Pass this message to the ToolTip control
	RelayEvent(WM_MOUSEMOVE, (WPARAM)nFlags, MAKELPARAM(LOWORD(point.x), LOWORD(point.y)));

	//If we are in capture mode, button has been pressed down
	//recently and not yet released - therefore check is we are
	//actually over the button or somewhere else. If the mouse
	//position changed considerably (e.g. we moved mouse pointer
	//from the button to some other place outside button area)
	//force the control to redraw
	//
	if ((m_button_down) && (::GetCapture() == m_hWnd)) {
		POINT p2 = point;
		::ClientToScreen(m_hWnd, &p2);
		HWND mouse_wnd = ::WindowFromPoint(p2);

		bool pressed = ((GetState() & BST_PUSHED) == BST_PUSHED);
		bool need_pressed = (mouse_wnd == m_hWnd);
		if (pressed != need_pressed) {
			//TRACE("* %08X Redraw\n", GetTickCount());
			SetState(need_pressed ? TRUE : FALSE);
			Invalidate();
		}
	}
	else {

		//Otherwise the button is released. That means we should
		//know when we leave its area - and so if we are not tracking
		//this mouse leave event yet, start now!
		//
		if (!m_tracking) {
			TRACKMOUSEEVENT t = {
				sizeof(TRACKMOUSEEVENT),
				TME_LEAVE,
				m_hWnd,
				0
			};
			if (::_TrackMouseEvent(&t)) {
				//TRACE("* Mouse enter\n");
				m_tracking = true;
				Invalidate();
			}
		}
	}

	//Forward this event to superclass
	CButton::OnMouseMove(nFlags, point);
}


LRESULT CSplayerButton::OnMouseLeave(WPARAM, LPARAM)
{
	ASSERT(m_tracking);
	//TRACE("* Mouse leave\n");
	m_tracking = false;
	Invalidate();
	return 0;
}


void CSplayerButton::OnKillFocus(CWnd *new_wnd)
{
	if (::GetCapture() == m_hWnd) {
		::ReleaseCapture();
		ASSERT(!m_tracking);
		m_button_down = false;
	}
	CButton::OnKillFocus(new_wnd);
}


BOOL CSplayerButton::OnClicked()
{
	if (::GetCapture() == m_hWnd) {
		::ReleaseCapture();
		ASSERT(!m_tracking);
	}
	m_button_down = false;
	//Invalidate();
	return FALSE;
}


LRESULT CSplayerButton::OnRadioInfo(WPARAM wparam, LPARAM)
{
	if (m_Checked) {	//only checked buttons need to be unchecked
		m_Checked = false;
		Invalidate();
	}
	return 0;
}


void CSplayerButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ((m_Style) && (nChar == ' ')) { //needed stuff for check & radio buttons
		if (m_Style == BS_CHECKBOX)
			SetCheck(m_Checked ? 0 : 1);
		if (m_Style == BS_RADIOBUTTON)
			SetCheck(1);
	}
	CButton::OnKeyDown(nChar, nRepCnt, nFlags);
}


LRESULT CSplayerButton::OnBMSetCheck(WPARAM wparam, LPARAM)
{
	m_Checked = wparam != 0;
	switch (m_Style)
	{
	case BS_RADIOBUTTON:
		if (m_Checked) { //uncheck the other radio buttons (in the same group)
			HWND hthis, hwnd2, hpwnd;
			hpwnd = GetParent()->GetSafeHwnd();	//get button parent handle
			hwnd2 = hthis = GetSafeHwnd();			//get this button handle
			if (hthis && hpwnd) {				//consistency check
				for (; ; ) {	//scan the buttons within the group
					hwnd2 = ::GetNextDlgGroupItem(hpwnd, hwnd2, 0);
					//until we reach again this button
					if ((hwnd2 == hthis) || (hwnd2 == NULL)) break;
					//post the uncheck message
					::PostMessage(hwnd2, WM_CXSHADE_RADIO, 0, 0);
				}
			}
		}
		break;
	case BS_PUSHBUTTON:
		m_Checked = false;
		ASSERT(false); // Must be a Check or Radio button to use this function
	}

	Invalidate();
	return 0;
}


LRESULT CSplayerButton::OnBMGetCheck(WPARAM wparam, LPARAM)
{
	return m_Checked;
}
