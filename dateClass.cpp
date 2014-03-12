#include "stdafx.h"    //ori una 
//#include <winbase.h> //ori alta

#include "dateClass.h"

dateClass::dateClass() {
	cmpFlag= 0;
	delData();
}

dateClass::dateClass(cint &y, cint &m, cint &d, cint &h, cint &mi, cint &s, cint &mili):
	year(y), mon(m), day(d), hour(h), minute(mi), sec(s), milisec(mili) { cmpFlag= 0; }

dateClass::dateClass(cchar *y, cchar *m, cchar *d, cchar *h, cchar *mi, cchar *s, cchar *mili) {
	year=	 (y? atoi(y): 0);
	mon=	 (m? atoi(m): 0);
	day=	 (d? atoi(d): 0);
	hour=	 (h? atoi(h): 0);
	minute=	 (mi? atoi(mi): 0);
	sec=	 (s? atoi(s): 0);
	milisec= (mili? atoi(mili): 0);
	cmpFlag= 0;
}

dateClass::dateClass(SYSTEMTIME *t):
	year(t->wYear),
	mon(t->wMonth),
	day(t->wDay),
	hour(t->wHour),
	minute(t->wMinute),
	sec(t->wSecond),
	milisec(t->wMilliseconds) { cmpFlag= 0; }

dateClass::dateClass(const dateClass &t):
	year(t.year),
	mon(t.mon),
	day(t.day),
	hour(t.hour),
	minute(t.minute),
	sec(t.sec),
	milisec(t.milisec) { cmpFlag= 0; }

void dateClass::delData() {
	year= mon= day= hour= minute= sec= milisec= 0;
}

void dateClass::set(cint &y, cint &m, cint &d, cint &h, cint &mi, cint &s, cint &mili) {
	year= y, mon= m, day= d, hour= h, minute= mi, sec= s, milisec= mili;
}

void dateClass::set(cchar *y, cchar *m, cchar *d, cchar *h, cchar *mi, cchar *s, cchar *mili) {
	year=	(y?  atoi(y): 0);
	mon=	(m?  atoi(m): 0);
	day=	(d?  atoi(d): 0);
	hour=	(h?  atoi(h): 0);
	minute= (mi? atoi(mi): 0);
	sec=	(s?  atoi(s): 0);
	milisec= (mili? atoi(mili): 0);
}

void dateClass::set(SYSTEMTIME *t) {
	if(!t) return;
	year=	t->wYear;
	mon=	t->wMonth;
	day=	t->wDay;
	hour=	t->wHour;
	minute= t->wMinute;
	sec=	t->wSecond;
	milisec= t->wMilliseconds;
}

void dateClass::get(SYSTEMTIME *t) {
	if(!t) return;
	t->wYear=	year;
	t->wMonth=	mon;
	t->wDay=	day;
	t->wDayOfWeek= 0;
	t->wHour=	hour;
	t->wMinute= minute;
	t->wSecond= sec;
	t->wMilliseconds= milisec;
}

///-------------///
// Special funcs //
///-------------///

void dateClass::getLocalTime() {
	SYSTEMTIME t;
	GetLocalTime(&t);

	year=	t.wYear;
	mon=	t.wMonth;
	day=	t.wDay;
	hour=	t.wHour;
	minute= t.wMinute;
	sec=	t.wSecond;
	milisec= t.wMilliseconds;
}

void dateClass::save(FILE *f) const {
	if(!f) return;

	fwrite(&year,	sizeof(year),	1, f);
	fwrite(&mon,	sizeof(mon),	1, f);
	fwrite(&day,	sizeof(day),	1, f);
	fwrite(&hour,	sizeof(hour),	1, f);
	fwrite(&minute, sizeof(minute), 1, f);
	fwrite(&sec,	sizeof(sec),	1, f);
	fwrite(&milisec, sizeof(milisec), 1, f);
}

void dateClass::load(FILE *f) {
	if(!f) return;

	fread(&year,	sizeof(year),	1, f);
	fread(&mon,		sizeof(mon),	1, f);
	fread(&day,		sizeof(day),	1, f);
	fread(&hour,	sizeof(hour),	1, f);
	fread(&minute,	sizeof(minute), 1, f);
	fread(&sec,		sizeof(sec),	1, f);
	fread(&milisec, sizeof(milisec), 1, f);
}

int dateClass::getMonLeni(cint &m, cint &y) const {
	switch(m) {
		case 1: return 31;
		case 2: return ((y% 4)==0)? 29: 28;
		case 3: return 31;
		case 4: return 30;
		case 5: return 31;
		case 6: return 30;
		case 7: return 31;
		case 8: return 31;
		case 9: return 30;
		case 10: return 31;
		case 11: return 30;
		case 12: return 31;
		default: return -1;
	}
}

int dateClass::getMonLen() const {
	switch(mon) {
		case 1: return 31;
		case 2: return ((year% 4)==0)? 29: 28;
		case 3: return 31;
		case 4: return 30;
		case 5: return 31;
		case 6: return 30;
		case 7: return 31;
		case 8: return 31;
		case 9: return 30;
		case 10: return 31;
		case 11: return 30;
		case 12: return 31;
		default: return -1;
	}
}

int dateClass::getMonLenST(SYSTEMTIME &t) const {
	switch(t.wMonth) {
		case 1: return 31;
		case 2: return ((t.wYear% 4)==0)? 29: 28;
		case 3: return 31;
		case 4: return 30;
		case 5: return 31;
		case 6: return 30;
		case 7: return 31;
		case 8: return 31;
		case 9: return 30;
		case 10: return 31;
		case 11: return 30;
		case 12: return 31;
		default: return -1;
	}
}

void dateClass::getHours(const dateClass &from, const dateClass &to) {
	dateClass t(to);
	int a;
	if(from<= to) {
		for(a= 0; from< t; a++)
			t.addHours(-1);

		t.addHours(1);
		a--;
		*this= t- from;
		hour+= a;
	} else {
		for(a= 0; from> t; a++)
			t.addHours(1);

		t.addHours(-1);
		a--;
		*this= from- t;
		hour+= a;
	}
}

void dateClass::getDays(const dateClass &from, const dateClass &to) {
	dateClass t(to);
	int a;
	if(from<= to) {
		for(a= 0; from< t; a++)
			t.addDays(-1);
		t.addDays(1);
		a--;
		*this= t- from;
		day+= a;
	} else {
		for(a= 0; from> t; a++)
			t.addDays(1);
		t.addDays(-1);
		a--;
		*this= from- t;
		day-= a;
	}
}

void dateClass::getDaysOnly(const dateClass &from, const dateClass &to) {
	dateClass t1(from), t2(to);

	t1.hour= t1.minute= t1.sec= t1.milisec= 0;
	t2.hour= t2.minute= t2.sec= t2.milisec= 0;
	delData();
	
	if(from<= to)
		for(day= 0; t1< t2; day++)
			t2.addDays(-1);
	else
		for(day= 0; t1> t2; day--)
			t2.addDays(1);
}

void dateClass::getMonths(const dateClass &from, const dateClass &to) {
	*this= (from<= to)? to- from: from- to;
	mon+= year* 12;
	year= 0;
}

void dateClass::getMonthsOnly(const dateClass &from, const dateClass &to) {
	delData();
	if(from<= to)
		mon= (to.mon- from.mon)+ (to.year- from.year)* 12;
	else
		mon= (from.mon- to.mon)+ (from.year- to.year)* 12;
}

///---------///
// Operators //
///---------///

bool dateClass::operator<(const dateClass &d) const{
	switch(cmpFlag) {
	case 0:
		if(year>  d.year) return false;
		if(year<  d.year) return true;
		if(year== d.year) {
			if(mon>  d.mon) return false;
			if(mon<  d.mon) return true;
			if(mon== d.mon) {
				if(day>  d.day) return false;
				if(day<  d.day) return true;
				if(day== d.day) {
					if(hour>  d.hour) return false;
					if(hour<  d.hour) return true;
					if(hour== d.hour) {
						if(minute>  d.minute) return false;
						if(minute<  d.minute) return true;
						if(minute== d.minute) {
							if(sec> d.sec) return false;
							if(sec< d.sec) return true;
							if(sec== d.sec) {
								if(milisec> d.milisec) return false;
								if(milisec< d.milisec) return true;
								if(milisec== d.milisec) return false;
							}
						}
					}
				}
			}
		}
		break;
	case 1:
		if(hour>  d.hour) return false;
		if(hour<  d.hour) return true;
		if(hour== d.hour) {
			if(minute>  d.minute) return false;
			if(minute<  d.minute) return true;
			if(minute== d.minute) {
				if(sec> d.sec) return false;
				if(sec< d.sec) return true;
				if(sec== d.sec) {
					if(milisec> d.milisec) return false;
					if(milisec< d.milisec) return true;
					if(milisec== d.milisec) return false;
				}
			}
		}
		break;
	case 2:
		if(year>  d.year) return false;
		if(year<  d.year) return true;
		if(year== d.year) {
			if(mon>  d.mon) return false;
			if(mon<  d.mon) return true;
			if(mon== d.mon) {
				if(day>  d.day) return false;
				if(day<  d.day) return true;
				if(day== d.day) return false;
			}
		}
		break;
	}

	return false;
}

bool dateClass::operator<=(const dateClass &d) const {
	if((*this == d) || (*this < d)) return true;
	return false;
}

bool dateClass::operator>(const dateClass &d) const {
	if(*this== d) return false;
	if(*this< d) return false;
	return true;
}

bool dateClass::operator>=(const dateClass &d) const {
	if((*this == d) || (*this> d)) return true;
	return false;
}

bool dateClass::operator==(const dateClass &d) const {
	switch (cmpFlag) {
	case 0:
		if(year!=    d.year)	return false;
		if(mon!=	 d.mon)		return false;
		if(day!=     d.day)		return false;
		if(hour!=	 d.hour)	return false;
		if(minute!=	 d.minute)	return false;
		if(sec!=	 d.sec)		return false;
		if(milisec!= d.milisec) return false;
		break;
	case 1:
		if(hour!=	 d.hour)	return false;
		if(minute!=	 d.minute)	return false;
		if(sec!=	 d.sec)		return false;
		if(milisec!= d.milisec) return false;
		break;
	case 2:
		if(year!=    d.year)	return false;
		if(mon!=	 d.mon)		return false;
		if(day!=     d.day)		return false;
		break;
	}
	return true;
}

bool dateClass::operator!=(const dateClass &d) const {
	if(*this == d) return false;
	return true;
}

dateClass &dateClass::operator=(const dateClass &d) {
	year=    d.year;
	mon=	 d.mon;
	day=     d.day;
	hour=    d.hour;
	minute=	 d.minute;
	sec=	 d.sec;
	milisec= d.milisec;
	return *this;
}

dateClass &dateClass::operator =(const SYSTEMTIME &t) {
	year=	t.wYear;
	mon=	t.wMonth;
	day=	t.wDay;
	hour=	t.wHour;
	minute= t.wMinute;
	sec=	t.wSecond;
	milisec= t.wMilliseconds;
	return *this;
}

dateClass::operator SYSTEMTIME() const{
	SYSTEMTIME t;
	t.wYear=		year;
	t.wMonth=		mon;
	t.wDay=			day;
	t.wDayOfWeek=	0;
	t.wHour=		hour;
	t.wMinute=		minute;
	t.wSecond=		sec;
	t.wMilliseconds= milisec;
	return t;
}

///------------------------///
// Add/Substract time funcs //
///------------------------///

void dateClass::addYears(cint &n) {
	year+= n;
}

void dateClass::addMons(cint &n) {
	if((year== 0)&& ((mon+ n)== 0)) {
		mon= 0;
	} else {								//trebe facut si if si pt BC
		int a= mon+ n;
		int b= ((n< 0)? a-12: a-1)/ 12;
		mon= a- (b* 12);
		addYears(b);
	}
}

void dateClass::addDays(cint &n) {
	int a, b;
	if(n>= 0) {
		a= day+ n;
		while(1) {
			b= getMonLen();
			if(a> b) {
				a-= b;
				addMons(1);
			} else break;
		}
		day= a;
	} else {
		if(((day+ n)== 0) && (year== 0) && (mon== 0)) {
			day= 0;
			return;
		}
		a= day+ n;
		while(1) {
			if(a< 1) {
				addMons(-1);
				b= getMonLen();
				a= b- abs(a);
			} else break;
		}
		day= a;
	}
}

void dateClass::addHours(cint &n) {
	int a= hour+ n;
	int b= ((n< 0)? a- 23: a)/ 24;
	hour= a- (b* 24);
	addDays(b);
}

void dateClass::addMins(cint &n) {
	int a= minute+ n;
	int b= ((n< 0)? a- 59: a)/ 60;
	minute= a- (b* 60);
	addHours(b);
}

void dateClass::addSecs(cint &n) {
	int a= sec+ n;
	int b= ((n< 0)? a- 59: a)/ 60;
	sec= a- (b* 60);
	addMins(b);
}

void dateClass::addMilisecs(cint &n) {
	int a= milisec+ n;
	int b= ((n< 0)? a- 999: a)/ 1000;
	milisec= a- (b* 1000);
	addSecs(b);
}

void dateClass::add(cint &y, cint &m, cint &d, cint &h, cint &mi, cint &s, cint &mili) {
	/*
	addMilisecs(mili);
	addSecs(s);
	addMins(mi);
	addHours(h);
	addDays(d);
	addMons(m);
	addYears(y);
	*/
	addMilisecs(mili);
	addSecs(s);
	addMins(mi);
	addHours(h);

	addYears(y);
	addMons(m);
	addDays(d);


}

void dateClass::addDate(const int &y, const int &m, const int &d) {
	addDays(d);
	addMons(m);
	addYears(y);
}

void dateClass::addTime(cint &h, cint &mi, cint &s, cint &mili) {
	addMilisecs(mili);
	addSecs(s);
	addMins(mi);
	addHours(h);
}

dateClass dateClass::operator+(const dateClass &d) const{
	dateClass r;
	r= *this;
	r.add(d.year, d.mon, d.day, d.hour, d.minute, d.sec, d.milisec);

	return r;
}

dateClass dateClass::operator-(const dateClass &d) const{
	dateClass r;
	r= *this;
	r.add(-d.year, -d.mon, -d.day, -d.hour, -d.minute, -d.sec, -d.milisec);

	return r;
}

void dateClass::operator+=(const dateClass &d) {
	add(d.year, d.mon, d.day, d.hour, d.minute, d.sec, d.milisec);
}

void dateClass::operator-=(const dateClass &d) {
	add(-d.year, -d.mon, -d.day, -d.hour, -d.minute, -d.sec, -d.milisec);
}
