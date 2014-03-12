#pragma once

class CSHeaderCtrl:public CHeaderCtrl {
public:
	CSHeaderCtrl();
	virtual ~CSHeaderCtrl();

protected:
	//{{AFX_MSG(CColorHeaderCtrl)
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};