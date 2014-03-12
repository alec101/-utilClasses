#pragma once

#include "csHeaderCtrl.h"

class CSListCtrl: public CListCtrl {
public:
	CSListCtrl();
	virtual ~CSListCtrl();
	CSHeaderCtrl header;
protected:
	

	virtual void PreSubclassWindow();
	int	SubclassHeader();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};