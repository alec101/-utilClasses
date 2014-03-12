#include "stdafx.h"
#include "stringClass.h"
#include "chainList.h"
#include "WinWatch.h"

IMPLEMENT_DYNAMIC(CWinWatch, CDialog)

BEGIN_MESSAGE_MAP(CWinWatch, CDialog)
	ON_WM_CLOSE()
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
END_MESSAGE_MAP()

///----------///
// Init Funcs //
///----------///

CWinWatch::CWinWatch(CWnd* pParent): CDialog(CWinWatch::IDD, pParent= pParent) {
	listSel= -1;
	watchTimer= 0;
}

CWinWatch::~CWinWatch() {
	data.delData();
}

void CWinWatch::delData() {
	data.delData();
}

void CWinWatch::OnClose() {
	if(watchTimer)
		KillTimer(watchTimer);
	watchTimer= 0;
	CDialog::OnClose();
}

BOOL CWinWatch::OnInitDialog() {
	CDialog::OnInitDialog();

	CRect rect;
	list.GetWindowRect(&rect);
	list.InsertColumn(3, "Type", LVCFMT_CENTER, rect.Width()/ 5- 8);
	list.InsertColumn(1, "Var name", LVCFMT_RIGHT, rect.Width()/ 2- 43);
	list.InsertColumn(2, "Contents", LVCFMT_LEFT, rect.Width()/ 2- 43);
	list.SetTextColor(RGB(255, 255, 255));
	list.SetTextBkColor(RGB(70, 80, 100));
	list.SetBkColor(RGB(70, 80, 100));

	createList();
	return TRUE;
}

void CWinWatch::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_WATCH_LIST1, list);
}

void CWinWatch::OnCancel() {
	if(watchTimer)
		KillTimer(watchTimer);
	watchTimer= 0;
	CDialog::OnCancel();
}

void CWinWatch::OnOK() {
	//CDialog::OnOK();				//DISABLED
}

///-------------///
// Control Funcs //
///-------------///

void CWinWatch::OnSizing(UINT fwSide, LPRECT pRect) {
	CDialog::OnSizing(fwSide, pRect);

	if(list) {
		RECT r;
		GetClientRect(&r);
		list.SetWindowPos(null, r.left+ 12, r.top+ 11, r.right- r.left- 24, r.bottom- r.top- 22, 0);
	}
}

void CWinWatch::OnSize(UINT nType, int cx, int cy) {
	CDialog::OnSize(nType, cx, cy);

	if(list) {
		RECT r;
		GetClientRect(&r);
		list.SetWindowPos(null, r.left+ 12, r.top+ 11, r.right- r.left- 24, r.bottom- r.top- 22, 0);
	}
}

void CWinWatch::createList() {
	int a;

	list.DeleteAllItems();
	list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	
	for(a=0; a< data.nrNodes; a++) {
		if(a== listSel)	list.InsertItem(LVIF_TEXT|LVIF_STATE, a, "", LVIS_SELECTED, LVIS_SELECTED, 0, 0);
		else			list.InsertItem(LVIF_TEXT|LVIF_STATE, a, "", 0, LVIS_SELECTED, 0, 0);
	}

	updateList();
}

void CWinWatch::updateList() {
	int a;
	string sp= "";
	watchData *p= (watchData*)data.first;

	for(a=0; a< data.nrNodes; a++) {

		if(p->varName.buf)
			list.SetItemText(a, 1, sp.f("%s", p->varName));
		else
			list.SetItemText(a, 1, sp.f("null address"));

		if(p->var== null)
			list.SetItemText(a, 2, sp.f("null address"));
		else
			switch(p->type) {
				case 0:
					list.SetItemText(a, 2, sp.f("%d, %c", p->sig? *((char*)p->var): *((uchar*)p->var), p->sig? *((char*)p->var): *((uchar*)p->var)));
					list.SetItemText(a, 0, p->sig? "char": "u char" );
					break;
				case 1:
					list.SetItemText(a, 2, sp.f("%d", p->sig? *((short*)p->var): *((ushort*)p->var)));
					list.SetItemText(a, 0, p->sig? "short": "u short" );
					break;
				case 2:
					list.SetItemText(a, 2, sp.f("%d", p->sig? (int)*((int*)p->var): *((uint*)p->var)));
					list.SetItemText(a, 0, p->sig? "int": "u int");
					break;
				case 3:
					list.SetItemText(a, 2, sp.f("%f", *((float*)p->var)));
					list.SetItemText(a, 0, "float");
					break;
				case 4:
					list.SetItemText(a, 2, sp.f("%f", *((double*)p->var)));
					list.SetItemText(a, 0, "double");
					break;
				case 5:
					list.SetItemText(a, 2, sp.f("%s", p->var));
					list.SetItemText(a, 0, "string");
					break;
				case 6:
					list.SetItemText(a, 2, (bool)* ((bool*)p->var)? "true": "false");
					list.SetItemText(a, 0, "boolean");
					break;
				default:
					break;
		}
		
		p= (watchData*)p->next;
	}

	if(listSel>= 0) {
		list.SetItemState(listSel, LVIS_SELECTED, LVIS_SELECTED);
		list.SetHotItem(listSel);
		list.EnsureVisible(listSel, false);
	}
}

///----------///
// Watch Data //
///----------///

void CWinWatch::addWatch(cvoid *v, string varName, int type, bool s) {
	watchData *p= new watchData;
	data.add(p);

	p->var= v;
	
	p->varName= (!varName)? "not named": varName;
	p->type= type;
	p->sig= s;
	createList();
	updateList();
	data.search(p);
}

void CWinWatch::delWatch(cvoid *v) {
	watchData *p;
	for(int a= 0; a< data.nrNodes; a++) {
		p= (watchData*)data.get(a);
		if(p->var== v) break;
	}
	data.del(p);
	createList();
	updateList();
}

void CWinWatch::OnShowWindow(BOOL bShow, UINT nStatus) {
	if(IsWindowVisible()) {
		KillTimer(watchTimer);
		watchTimer= 0;
	} else
		watchTimer= SetTimer(WATCH_TIMER_NR, 1000, 0);

	CDialog::OnShowWindow(bShow, nStatus);
}

void CWinWatch::OnTimer(UINT_PTR nIDEvent) {
	if(nIDEvent== watchTimer){ 
		updateList();
	}

	CDialog::OnTimer(nIDEvent);
}

