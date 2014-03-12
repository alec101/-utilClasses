#include "stdafx.h"
#include "CSHeaderCtrl.h"


CSHeaderCtrl::CSHeaderCtrl() {
}

BEGIN_MESSAGE_MAP(CSHeaderCtrl, CHeaderCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CSHeaderCtrl::~CSHeaderCtrl() {
}

void CSHeaderCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) {
	NMCUSTOMDRAW *cd= (NMCUSTOMDRAW*)pNMHDR;

	*pResult= CDRF_DODEFAULT;

	if(cd->dwDrawStage == CDDS_PREPAINT) {
		*pResult= CDRF_NOTIFYITEMDRAW;
		
	} else if(cd->dwDrawStage == CDDS_ITEMPREPAINT) {
		SetTextColor(cd->hdc, RGB(255, 255, 255));
		
		if(cd->dwItemSpec)
			SetBkColor(cd->hdc, RGB(50, 150, 230));
		else
			SetBkColor(cd->hdc, RGB(50, 150, 230));

		*pResult = CDRF_NEWFONT;
	}
}


BOOL CSHeaderCtrl::OnEraseBkgnd(CDC* pDC) {
	//CRect rc;
	//GetClientRect(&rc);

	//pDC->FillSolidRect(&rc, RGB(50, 150, 230));
	return TRUE;
}
