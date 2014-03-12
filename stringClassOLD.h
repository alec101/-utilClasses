#pragma once

#define MAX_FORMAT_STRING 512
		// in caz ca tre formatat un dit-am-ai stringu, se poate face prin finalString= string+ this->f("bla", n)+ string
		// sau... marit MAX_FORMAT_STRING... ma gandesc la vre-o carte care sa aibe text variabil...


///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//must be changed from MS strncpy & strcpy to fast fors/whiles. MS code is now filled with shit, templates. ASM is a wet dead dream
//strlen is still OK
///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


class string8 {
	static char nrBuf[512];					//should be used only for number to string buffering
	static char fBuf[MAX_FORMAT_STRING];	//used only in complex format funcs ( .format() .f()
	//static wchar_t nrBuf[512];					//should be used only for number to string buffering
	//static wchar_t fBuf[MAX_FORMAT_STRING];	//used only in complex format funcs ( .format() .f()

public:
	char *buf;								//bufferu stringului
	//wchar_t *buf;
	
	string8();										//construieste un string gol
	string8(const char *s);			//this= s
	string8(const string8 &s);		//this= s
	string8(const int &v);				//conv int in string
	//string8(const int64 &v);			//conv int64 in string
	string8(const long long &v);			//conv int64 in string
	string8(const double &v);		//conv double in string
	string8(const float &v);			//converteste float in string
	
	~string8();						//dealoca bufferul
	void deleteData();				//dealoca bufferul

	void up();
	void down();
	size_t getLen() const;			//returns string length, minus terminator '\0'
	void encryptXOR();				//basic XOR encrypt / decrypt

	string8 &getLine(int l= 0);		//returns a string with line 'l' from current string (removes \n)
	int getNrLines();// [NOT TESTED]//returns nr lines in the string (based on \n occurance); returns 0 if buf is null


	string8 &format(const char *f, ...);		//format->string
	string8 &f(const char *f, ...);			//format->string

	string8 &operator=(const string8 &s);		//string= s
	string8 &operator=(const char *s);		//string= s
	string8 &operator=(const int &v);		//converteste int in string
	string8 &operator=(const int64 &v);	//converteste __int64 in string
	string8 &operator=(const double &v);		//converteste double in string
	string8 &operator=(const float &v);		//converteste float in string
	
	void operator+=(const string8 &s);		//adauga s la string
	void operator+=(const char *s);			//adauga s la string
	void operator+=(const char &s);			//adauga s la string (nu va considera s ca numar ci ca caracter)
	void operator+=(const short &n);		//adauga numarul ca text, la sfarsitul this*
	void operator+=(const int &n);			//adauga numarul ca text, la sfarsitul this*
	void operator+=(const int64 &n);		//adauga numarul ca text, la sfarsitul this*
	void operator+=(const double &n);		//adauga numarul ca text, la sfarsitul this*
	void operator+=(const float &n);		//adauga numarul ca text, la sfarsitul this*

	void operator-=(const int &n);			//scade n caractere din string

	string8 operator+(const string8 &s);		//returneaza this+ s - se folosesc obiecte temporare - copieri so nu e foarte rapida
	string8 operator+(const char *s);		//returneaza this+ s - se folosesc obiecte temporare - copieri so nu e foarte rapida

	string8 operator-(const int &n);			//taie din string n caractere de la sfarsit, returneaza un obiect temporar cu rezultatul (nu se modifica nimic)

	//char operator[](int n);				//returneaza string[n]

	bool operator==(const string8 &s) const;		//check daca e identic
	bool operator==(const char *s) const;			//check daca e identic
	bool operator==(const int &n) const;			//check daca valoarea din string e egala cu valoarea n
	bool operator==(const int64 &n) const;		//check daca valoarea din string e egala cu valoarea n
	bool operator==(const double &n) const;		//check daca valoarea din string e egala cu valoarea n
	bool operator==(const float &n) const;

	bool operator!=(const string8 &s) const;		//check daca nu e identic
	bool operator!=(const char *s) const;			//check daca nu e identic
	bool operator!=(const int &n) const;			//check daca valoarea din string nu e egala cu valoarea n
	bool operator!=(const int64 &n) const;		//check daca valoarea din string nu e egala cu valoarea n
	bool operator!=(const double &n) const;		//check daca valoarea din string nu e egala cu valoarea n
	bool operator!=(const float &n) const;

	bool operator!() const;						//checkuieste daca stringu e NULL

	operator char*();				//returneaza stringu ca char*
///astea nu stiu cat de bune sunt, posibil sa incurce
	operator int64();				//returneaza stringu ca numar
	operator int();					//returneaza stringu ca numar
	operator double();				//returneaza stringu ca numar
	operator bool();				//convert to int-> return as bool
};

static string8 opBuf;						//buffer fol pt operatori