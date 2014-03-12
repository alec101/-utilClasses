#pragma once

class dateClass {
public:
	int year, mon, day, hour, minute, sec, milisec;

	dateClass();
	dateClass(cint &y, cint &m, cint &d, cint &h= 0, cint &mi= 0, cint &s= 0, cint &mili= 0);
	dateClass(cchar *y, cchar *m, cchar *d, cchar *h= null, cchar *mi= null, cchar *s= null, cchar *mili= null);
	dateClass(SYSTEMTIME *t);
	dateClass(const dateClass &t);

	void set(cint &y, cint &m, cint &d, cint &h= 0, cint &mi= 0, cint &s= 0, cint &mili= 0);
	void set(cchar *y, cchar *m, cchar *d, cchar *h= null, cchar *mi= null, cchar *s= null, cchar *mili= null);
	void set(SYSTEMTIME *t);					//sets internal vars from SYSTEMTIME struct

	void get(SYSTEMTIME *t);					//assigns internal vars to SYSTEMTIME struct
	void getHours(const dateClass &from, const dateClass &to);		//gets nr of hours, &from => &to
	void getDays(const dateClass &from, const dateClass &to);		//gets nr of days, &from => &to
	void getDaysOnly(const dateClass &from, const dateClass &to);	//gets nr of days, &from => &to, ignores hours/mins/secs
	void getMonths(const dateClass &from, const dateClass &to);		//gets nr of months &from => &to
	void getMonthsOnly(const dateClass &from, const dateClass &to); //gets nr of months, &from => &to, ignores days/hours/mins/secs

	void save(FILE *f) const;							//saves all vars in f
	void load(FILE *f);							//loads all vars from f
	void getLocalTime();

	int getMonLeni(cint &m, cint &y) const;		//returns nr of days in month (needs year for that damn tricky month)
	int getMonLen() const;						//returns month length for *this
	int getMonLenST(SYSTEMTIME &t) const;		//returns month length for specified SYSTEMTIME&

	void addYears(cint &n);						//set negative n for substraction
	void addMons(cint &n);						//set negative n for substraction
	void addDays(cint &n);						//set negative n for substraction
	void addHours(cint &n);						//set negative n for substraction
	void addMins(cint &n);						//set negative n for substraction
	void addSecs(cint &n);						//set negative n for substraction
	void addMilisecs(cint &n);					//set negative n for substraction

	void add(cint &y, cint &m, cint &d, cint &h, cint &mi, cint &s= 0, cint &mili= 0); //set negative vars for substraction
	void addDate(cint &y, cint &m, cint &d);					// adds/substracts only years/months/days, set negative vars for substraction
	void addTime(cint &h, cint &mi, cint &s, cint &mili= 0);	// adds/substracts only hours/mins/secs/milisecs, vars can be negative

	dateClass operator+(const dateClass &) const;
	dateClass operator-(const dateClass &) const;
	void operator+=(const dateClass &);
	void operator-=(const dateClass &);

	ushort cmpFlag;								// 0= cmp all, 1= time compare only, 2= date compare only

	bool operator<(const dateClass &) const;
	bool operator<=(const dateClass &) const;
	bool operator>(const dateClass &) const;
	bool operator>=(const dateClass &) const;
	bool operator==(const dateClass &) const;
	bool operator!=(const dateClass &) const;
	dateClass &operator=(const dateClass &);
	dateClass &operator=(const SYSTEMTIME &);
	operator SYSTEMTIME() const;

	void delData();								//sets vars to 0
};
