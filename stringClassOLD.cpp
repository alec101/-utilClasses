#include "source/precompiled.h"			// <- ori asta

	#include "typeShortcuts.h"		// <- ori astea
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdarg.h>
  #include <ctype.h>
	#include "stringClass.h"		// <-

// sunt probleme pt stringuri mai mari de 512. TRE MODIFICAT (eventual un singur buffer static mare, parte din clasa) 
// sau alocare dinamica... buferele astea de 512 sunt cam cretine
char string::nrBuf[512];
char string::fBuf[MAX_FORMAT_STRING];

string &string::format(const char *f, ...) {
	size_t l= strlen(f);
	if(l> MAX_FORMAT_STRING- 1) {
		*this= "FORMAT FAILED";
		return *this;
	}

	va_list args;
    va_start(args, f);
    vsprintf(fBuf, f, args);
    va_end(args);

	deleteData();
	buf= new char[strlen(fBuf)+ 1];
	strcpy(buf, fBuf);
	return *this;
}

string &string::f(const char *format, ...) {
	size_t l= strlen(format);
	if(l> MAX_FORMAT_STRING- 1) {
		*this= "FORMAT FAILED";
		return *this;
	}

	va_list args;
    va_start(args, format);
    vsprintf(fBuf, format, args);
    va_end(args);

	deleteData();
	buf= new char[strlen(fBuf)+ 1];
	strcpy(buf, fBuf);
	return *this;
}


//CONSTRUCTORS
string::string() {
	buf= NULL;
}

string::string(const char *s) {
	if(!s) {
		buf= NULL;
		return;
	}
		
	buf= new char[strlen(s)+ 1];
	strcpy(buf, s);
}

string::string(const string &s) {
	if(!s.buf) {
		buf= NULL;
		return;
	}

	buf= new char[strlen(s.buf)+ 1];
	strcpy(buf, s.buf);
}

string::string(const int &v) {
    sprintf(nrBuf, "%d", v);
	buf= new char[strlen(nrBuf)+ 1];
	strcpy(buf, nrBuf);
}

string::string(const float &v) {
	sprintf(nrBuf, "%.7f", v);						// tre facut un string pt nr reale, ca sa poa sa fie modificat defaultu cand se afiseaza... eventual
	buf= new char[strlen(nrBuf)+ 1];
	strcpy(buf, nrBuf);
}

string::string(const double &v) {
	sprintf(nrBuf, "%.7f", v);						// !!!!!!!
	buf= new char[strlen(nrBuf)+ 1];
	strcpy(buf, nrBuf);
}

string::string(const int64 &v) {
	sprintf(nrBuf, "%lld", v);
	buf= new char[strlen(nrBuf)+ 1];
	strcpy(buf, nrBuf);
}

//DESTRUCTOR
string::~string() {
	if(buf) delete[] buf; buf= NULL;
}

//FUNCS
void string::deleteData() {
	if(buf)	delete[] buf; buf= NULL;
}

void string::up() {
    if(buf) {
        char *s= buf;
        while(s) {
            *s= toupper (*s);
            s++;
        }
    }
  
   
//	if(buf)	strupr(buf);
}

void string::down() {
    if(buf) {
        char *s= buf;
        while(s) {
            *s= tolower(*s);
            s++;
        }
    }
    
	//if(buf)	_strlwr(buf);
}

size_t string::getLen() const {
	if(buf)
		return strlen(buf);
	return 0;
}

void string::encryptXOR() {
	if(!buf) return;
	char k= 147;
	for(uint a= 0; a< getLen(); a++)
		buf[a]= buf[a]^ k;
}

string &string::getLine(int l) {
	if(!buf) return opBuf= "";			// preetier, returneaza ceva printabil, fara eroare, in caz ca nu exista nimic in string

	int a, len= 0;
	char *from= null;
	char *p= buf;
	
	for(a= 0; a< l; a++)						// skip to l
		while(((*p)!= '\0') && ((*p)!= '\n'))
			p++;

	from= p;									// found starting point of line to be returned

	while (((*p)!= '\0') && ((*p)!= '\n')) {	// ch
		p++;
		len++;
	}

	if(!len) return opBuf= "";

	opBuf.deleteData();
	opBuf.buf= new char[len+ 1];

	for(a= 0; a< len; a++)
		opBuf.buf[a]= from[a];

	opBuf.buf[a]= '\0';

	return opBuf;
}

int string::getNrLines() {
	if(!buf)
		return 0;

	char *p= buf;
	int ret= 0;
	while(*p != '\0') {
		ret++;
		while( (*p != '\0') && (*p != '\n'))
			p++;
	}
	return ret;
}

//OPERATORS
///--------

string &string::operator =(const string &s) {
	deleteData();

	if(!s.buf) return *this;

	size_t l= strlen(s.buf);
	buf= new char[l+ 1];
	strcpy(buf, s.buf);
	return *this;
}

string &string::operator =(const char *s) {
	deleteData();

	if(!s) return *this;

	buf= new char[strlen(s)+ 1];
	strcpy(buf, s);
	return *this;
}

string &string::operator =(const int &v) {
	deleteData();
	
    sprintf(nrBuf, "%d", v);
	buf= new char[strlen(nrBuf)+ 1];
	strcpy(buf, nrBuf);
	return *this;
}

string &string::operator =(const int64 &v) {
	deleteData();

    sprintf(nrBuf, "%lld", v);
	buf= new char[strlen(nrBuf)+ 1];
	strcpy(buf, nrBuf);
	return *this;
}

string &string::operator =(const double &v) {
	deleteData();

	sprintf(nrBuf, "%.7f", v);
	buf= new char[strlen(nrBuf)+ 1];
	strcpy(buf, nrBuf);
	return *this;
}

string &string::operator =(const float &v) {
	deleteData();

	sprintf(nrBuf, "%.7f", v);
	buf= new char[strlen(nrBuf)+ 1];
	strcpy(buf, nrBuf);
	return *this;
}

void string::operator+=(const string &s) {
	char *t= NULL;
	size_t l= 0;

	if(buf) l= strlen(buf);
	if(s.buf) l+= strlen(s.buf);

	if(l) {
		t= new char[l+ 1];
		t[0]= 0;
		if(buf) strcpy(t, buf);
		if(s.buf) strcat(t, s.buf);
	}

	deleteData();
	buf= t;
}

void string::operator+=(const char *s) {
	char *t= NULL;
	size_t l= 0;

	if(buf) l= strlen(buf);
	if(s) l+= strlen(s);

	if(l) {
		t= new char[l+ 1];
		t[0]=0;
		if(buf) strcpy(t, buf);
		if(s) strcat(t, s);
	}

	deleteData();
	buf= t;
}

void string::operator+=(const char &s) {
	if(buf) {
		uint64 a, l= strlen(buf);
		char *t= new char[l+ 2];			/// adds 1 more char to alloc

		for(a= 0; a< l; a++)				/// strcpy
			t[a]= buf[a];
		t[a]= s;
		t[a+ 1]= 0;

		delete[] buf;
		buf= t;
	} else {
		buf= new char[2];
		buf[1]= s;
		buf[2]= 0;
	}
}

// add number @ end of string funcs
void string::operator+=(const int &n) {
	string t= n;	*this+= t;
}

void string::operator+=(const int64 &n) {
	string t= n;	*this+= t;
}

void string::operator+=(const double &n) {
	string t= n;	*this+= t;
}

void string::operator+=(const float &n) {
	string t= n;	*this+= t;
}

void string::operator+=(const short &n) {
	string t= n;	*this+= t;
}

// scade n caractere din string
void string::operator-=(const int &n) {
	size_t a, l= 0;

	if(buf) 
		l= strlen(buf);
	else 
		return;

	if(n>= l) deleteData();			// nu mai ramane nimic
	else {
		char *t= new char[l- n+ 1];
		for(a= 0; a< (l- n); a++)
			t[a]= buf[a];

		t[a]= '\0';
		deleteData();
		buf= t;
	}
}

// FOLOSESTE opBuf //                                        <-------------------------- cica -.-
string string::operator +(const string &s) {
	opBuf.deleteData();

	if(!buf) {
		if(!s.buf) return opBuf;
		opBuf.buf= new char[strlen(s.buf)+ 1];
		strcpy(opBuf.buf, s.buf);
		return opBuf;
	}
	if(!s.buf) {
		opBuf.buf= new char[strlen(buf)+ 1];
		strcpy(opBuf, buf);
		return opBuf;
	}
	
	opBuf.buf= new char[strlen(buf)+ strlen(s.buf)+ 1];
	strcpy(opBuf.buf, buf);
	strcat(opBuf.buf, s.buf);

	return opBuf;		// banuiesc ca se va face un object temporar in care se copiaza opBuf, pt ca nu se returneaza referinta
}

string string::operator +(const char *s) {
	opBuf.deleteData();

	if(!buf) {
		if(!s)
			return *this;

		return opBuf= s;
	}
	if(!s)
		return opBuf= buf;

	opBuf.buf= new char[strlen(buf)+ strlen(s)+ 1];
	strcpy(opBuf, buf);
	strcat(opBuf, s);
	return opBuf;	// same, obiect temporar, so functia nu e foarte rapida
}

string string::operator -(const int &n) {
	size_t a;
	opBuf.deleteData();

	if(!buf) return opBuf;

	size_t l= strlen(buf);
	if(n>= l) return opBuf;
	else {
		opBuf.buf= new char[l- n+ 1];
		for(a= 0; a< (l- n); a++)
			opBuf.buf[a]= buf[a];

		opBuf.buf[a]='\0';
	}
	return opBuf;
}

/*
char string::operator [](int n)
{
	return buf[n];
}
*/

//BOOLEAN OPERATIONS
//__________________
bool string::operator ==(const string &s) const {
	if(!buf) {
		if(!s.buf) return true;
		return false;
	}
	if(!s.buf)
		return false;

	bool r= true;
	if(strcmp(buf, s.buf)) r= false;
	return r;
}

bool string::operator ==(const char *s) const {
	if(!buf) {
		if(!s) return true;
		return false;
	}
	if(!s) 
		return false;

	bool r= true;
	if(strcmp(buf, s)) r= false;
	return r;
}

bool string::operator ==(const double &n) const {
	if(!buf) {
		if(!n)
			return true;
		return false;
	}

	double t= atof(buf);
	return t== n;
}

bool string::operator ==(const float &n) const {
	if(!buf) {
		if(!n)
			return true;
		return false;
	}

	float t= (float)atof(buf);
	return t== n;
}

bool string::operator ==(const int &n) const {
	if(!buf) {
		if(!n)
			return true;
		return false;
	}

	int t= atoi(buf);
	return t== n;
}

bool string::operator ==(const int64 &n) const {
	if(!buf) {
		if(!n) return true;
		return false;
	}

	int64 t= atoll(buf);
	return t== n;
}

bool string::operator !=(const string &s) const {
	if(!buf) {
		if(!s.buf)
			return false;
		return true;
	}
	if(!s.buf)
		return true;

	bool r= true;
	if(!strcmp(buf, s.buf)) r= false;
	return r;
}

bool string::operator !=(const char *s) const {
	if(!buf) {
		if(!s)
			return false;
		return true;
	}
	if(!s)
		return true;

	bool r= true;
	if(!strcmp(buf, s)) r= false;
	return r;
}

bool string::operator !=(const double &n) const {
	if(!buf) {
		if(!n)
			return false;
		return true;
	}

	double t= atof(buf);
	return t!= n;
}

bool string::operator !=(const float &n) const {
	if(!buf) {
		if(!n)
			return false;
		return true;
	}

	float t= (float)atof(buf);
	return t!=n;
}

bool string::operator !=(const int &n) const {
	if(!buf) {
		if(!n)
			return false;
		return true;
	}

	int t= atoi(buf);
	return t!=n;
}

bool string::operator !=(const int64 &n) const {
	if(!buf) {
		if(!n)
			return false;
		return true;
	}

	int64 t= atoll(buf);
	return t!=n;
}

bool string::operator!() const {
	return (!buf)? true: false;
}

//CONVERSION
//__________
string::operator char*() {
	return buf;
}

string::operator double() {
	if(!buf)
		return NULL;
	return atof(buf);
}

string::operator int() {
	if(!buf)
		return NULL;
	return atoi(buf);
}

string::operator int64() {
	if(!buf)
		return NULL;
	return atoll(buf);
}

string::operator bool() {
	if(!buf)
		return false;
	return atoi(buf)? true: false;
}



