#pragma once

#define WATCH_TIMER_NR 1000000
//scrl down for res.h&proj.rc textincludes

class CWinWatch: public CDialog {
	DECLARE_DYNAMIC(CWinWatch)
	UINT_PTR watchTimer;

public:
	int listSel;
	CListCtrl list;
	void createList();
	void updateList();

	chainList data;
	void addWatch(cvoid *v, string varName, int type, bool s= true);	// type: 0 char, 1 short, 2 int, 3 float, 4 double, 5 string(char*), 6 bool. s: signed or not
	void delWatch(cvoid *v);
	
	void delData();

	CWinWatch(CWnd* pParent= NULL);						// standard constructor
	virtual ~CWinWatch();

// Dialog Data
	enum { IDD = IDD_WATCH };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

// watches chain list
///------------------
class watchData: public chainData {
public:
	int type;		// 0 char, 1 short, 2 int, 3 float, 4 double, 5 string, 6 bool
	bool sig;
	cvoid *var;
	string varName;
};



/*
resource.h
----------
#define IDD_WATCH                       xxx
#define IDC_WATCH_LIST1                 xxxx

projectName.rc
--------------

..................................................................................................
/////////////////////////////////////////////////////////////////////////////
//
// Dialog
//

IDD_WATCH DIALOGEX 0, 0, 214, 212
STYLE DS_SETFONT | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME
EXSTYLE WS_EX_TOOLWINDOW
CAPTION "Vars Watch"
FONT 8, "Bitstream Vera Sans Mono", 400, 0, 0x0
BEGIN
    CONTROL         "",IDC_WATCH_LIST1,"SysListView32",LVS_REPORT | LVS_ALIGNLEFT | WS_BORDER | WS_TABSTOP,7,7,200,198
END

..................................................................................................

/////////////////////////////////////////////////////////////////////////////
//
// DESIGNINFO
//

GUIDELINES DESIGNINFO 
BEGIN
	...
    IDD_WATCH, DIALOG
    BEGIN
        LEFTMARGIN, 7
        RIGHTMARGIN, 207
        TOPMARGIN, 7
        BOTTOMMARGIN, 205
    END
END
...................................................................................................
*/