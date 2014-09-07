#include "common.h"

int getFontHeight(HFONT newFont) {
	HDC hDC;
	HFONT oldFont;
	TEXTMETRIC metric;
	
	hDC = GetDC(0);
	oldFont = SelectObject(hDC, newFont);
	GetTextMetrics(hDC, &metric);
	SelectObject(hDC, oldFont);
	ReleaseDC(NULL, hDC);
	
	return metric.tmHeight;
}

void getTextSize(HFONT newFont, const TCHAR *text, int *wid, int *hei) {
	HDC hDC;
	HFONT oldFont;
	SIZE sz;
	
	hDC = GetDC(0);
	oldFont = SelectObject(hDC, newFont);
	GetTextExtentPoint32(hDC, text, _tcslen(text), &sz);
	SelectObject(hDC, oldFont);
	ReleaseDC(NULL, hDC);
	
	if (wid != NULL) {
		*wid = sz.cx;
	}
	if (hei != NULL) {
		*hei = sz.cy;
	}
}

