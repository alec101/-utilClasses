#include "stdafx.h"
#include "CSListCtrl.h"

//---------------------------
//#include <uxtheme.h>
// trebe SetWindowTheme(list.header.hwnd, L"", L"") altfel thema xp-ului va desena peste
//---------------------------

BEGIN_MESSAGE_MAP(CSListCtrl, CListCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()


CSListCtrl::CSListCtrl() {
}

CSListCtrl::~CSListCtrl() {
}

int CSListCtrl::SubclassHeader() {
	CHeaderCtrl *p;
	int n= 0;

	p= GetHeaderCtrl();
	if(p && p->GetSafeHwnd()) {
		if(!header.SubclassWindow(p->GetSafeHwnd()))
			n= -1;
	}

	return n;
}

void CSListCtrl::PreSubclassWindow() {
	CListCtrl::PreSubclassWindow();
	SubclassHeader();
}

int CSListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if(CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	return SubclassHeader();
}
