#include "pch.h"

#include <stdio.h>
#include <stdarg.h>
#include "typeShortcuts.h"
#include "stringClass8.h"


/* UTF 8 research
  -they say to start a file with a ZERO WIDTH NOBREAK SPACE (U+FEFF), which is
     in this role also referred to as the �signature� or �byte-order mark (BOM)�
  -linux avoids BOM, so i don't think there's gonna be any testing using BOM;
     only to ignore a BOM if the file starts with it

  -The following byte sequences are used to represent a character.
 *  The sequence to be used depends on the Unicode number of the character: 
    U-00000000 � U-0000007F:  0xxxxxxx 
    U-00000080 � U-000007FF:  110xxxxx 10xxxxxx 
    U-00000800 � U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx 
    U-00010000 � U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx 
    U-00200000 � U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 
    U-04000000 � U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

  -0300-036F Combining Diacritical Marks, since version 1.0, with modifications in subsequent versions down to 4.1
   1DC0-1DFF Combining Diacritical Marks Supplement, versions 4.1 to 5.2
   20D0-20FF Combining Diacritical Marks for Symbols, since version 1.0, with modifications in subsequent versions down to 5.1
   FE20-FE2F Combining Half Marks, versions 1.0, updates in 5.2

  -An important note for developers of UTF-8 decoding routines: For security reasons,
     a UTF-8 decoder must not accept UTF-8 sequences that are longer than necessary
     to encode a character. For example, the character U+000A (line feed) must be
     accepted from a UTF-8 stream only in the form 0x0A, but not in any of the following
     five possible overlong forms: 
    0xC0 0x8A
    0xE0 0x80 0x8A
    0xF0 0x80 0x80 0x8A
    0xF8 0x80 0x80 0x80 0x8A
    0xFC 0x80 0x80 0x80 0x80 0x8A

   Any overlong UTF-8 sequence could be abused to bypass UTF-8 substring tests
      that look only for the shortest possible encoding.
   All overlong UTF-8 sequences start with one of the following byte patterns: 
    
    1100000x (10xxxxxx) 
    11100000 100xxxxx (10xxxxxx) 
    11110000 1000xxxx (10xxxxxx 10xxxxxx) 
    11111000 10000xxx (10xxxxxx 10xxxxxx 10xxxxxx) 
    11111100 100000xx (10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx) 
    
   Also note that the code positions U+D800 to U+DFFF (UTF-16 surrogates) as well
     as U+FFFE and U+FFFF must not occur in normal UTF-8 or UCS-4 data.
   UTF-8 decoders should treat them like malformed or overlong sequences for safety reasons.

   Markus Kuhn�s UTF-8 decoder stress test file contains a systematic collection
   of malformed and overlong UTF-8 sequences and will help you to verify
   the robustness of your decoder.
   http://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt

   putwchar(c)
{
  if (c < 0x80) {
    putchar (c);
  }
  else if (c < 0x800) {
    putchar (0xC0 | c>>6);
    putchar (0x80 | c & 0x3F);
  }
  else if (c < 0x10000) {
    putchar (0xE0 | c>>12);
    putchar (0x80 | c>>6 & 0x3F);
    putchar (0x80 | c & 0x3F);
  }
  else if (c < 0x200000) {
    putchar (0xF0 | c>>18);
    putchar (0x80 | c>>12 & 0x3F);
    putchar (0x80 | c>>6 & 0x3F);
    putchar (0x80 | c & 0x3F);
  }
}

*/




char *buf8_string= null;       /// utf8  buffer (nothing to bother here)
ulong *buf32_string= null;     /// utf32 buffer (nothing to bother here)
ushort *bufwin_string= null;   /// windows compat (nothing to bother here)
char fBuf[1024];               

string8 &string8::f(const char *format, ...) {
	size_t l= strlen(format);
	if(l> 1023) {
		*this= "TOO BIG, FORMAT FAILED";
		return *this;
	}
/// create formatted text (use a buffer)
  fBuf[0]= '\0';
	va_list args;
  va_start(args, format);
  vsprintf(fBuf, format, args);
  va_end(args);

	return *this= fBuf;
}


string8::string8():d(null), len(0), nrchars(0)
{}


string8::~string8() {
  delData();
  if(buf8_string) { delete[] buf8_string; buf8_string= null; }
  if(buf32_string) { delete[] buf32_string; buf32_string= null; }
  if(bufwin_string) { delete[] bufwin_string; bufwin_string= null; }
}


void string8::delData() {
  if(d) { delete[] d;  d= null; }
  len= 0;
  nrchars= 0;
}


// OPERATOR= //
///---------///
string8 &string8::operator=(const string8 &s) {
  if(!s.len) {
    delData();
    return *this;
  }
  len= s.len;
  nrchars= s.nrchars;
  
/// copy
  if(d) delete[] d;
  d= new char[len+ 1];
  for(uint a= 0; a< len+ 1; a++)
    d[a]= s.d[a];

  return *this;
}


string8 &string8::operator=(const char *s) {
  if(!s) {
    if(d)
      delData();
    return *this;
  }
  delData();
  size_t l= strlen(s);
  if(!l) return *this;
  
/// string sizes
  len= l;
  nrchars= strchars(s);
  
/// alloc+ copy
  d= new char[l+ 1];
  d[l]= 0;                    /// terminator
  while(l) {
    l--;
    d[l]= s[l];           // d[--l] won't decrease s[l] too in clang :(
  }
  
  return *this;
}


string8 &string8::operator=(const ulong *s) {
  delData();
  nrchars= strlen32(s);
  if(!nrchars) return *this;
  
/// length in bytes of d (len is 0 cuz of delData())
  for(size_t a= 0; a< nrchars; a++)	     // for each character in s
    if(s[a]<=      0x0000007F) len++;   /// 1 byte  U-00000000�U-0000007F:  0xxxxxxx 
    else if(s[a]<= 0x000007FF) len+= 2; /// 2 bytes U-00000080�U-000007FF:  110xxxxx 10xxxxxx 
    else if(s[a]<= 0x0000FFFF) len+= 3; /// 3 bytes U-00000800�U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx 
    else if(s[a]<= 0x001FFFFF) len+= 4; /// 4 bytes U-00010000�U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx 
    else if(s[a]<= 0x03FFFFFF) len+= 5; /// 5 bytes U-00200000�U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 
    else if(s[a]<= 0x7FFFFFFF) len+= 6; /// 6 bytes U-04000000�U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

/// create d
  d= new char[len+ 1];

  uchar *p= (uchar *)d;

  for(size_t a= 0; a< nrchars; a++) // for each character in s
    if(s[a]<= 0x0000007F) {         //  1 byte   U-00000000�U-0000007F:  0xxxxxxx 
      *p++=   s[a];
    } else if(s[a]<= 0x000007FF) {  //  2 bytes  U-00000080�U-000007FF:  110xxxxx 10xxxxxx 
      *p++=  (s[a]>> 6)        | 0xC0;          /// [BYTE 1]       >> 6= 000xxxxx 00000000  then header | c0 (11000000)
      *p++=  (s[a]&       0x3f)| 0x80;          /// [BYTE 2]         3f= 00000000 00xxxxxx  then header | 80 (10000000)
    } else if(s[a]<= 0x0000FFFF) {  //  3 bytes  U-00000800�U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx 
      *p++=  (s[a]>> 12)       | 0xE0;          /// [BYTE 1]      >> 12= 0000xxxx 00000000 00000000  then header | e0 (11100000)
      *p++= ((s[a]>> 6)&  0x3F)| 0x80;          /// [BYTE 2]  >> 6 & 3f= 00000000 00xxxxxx 00000000  then header | 80 (10000000)
      *p++=  (s[a]&       0x3F)| 0x80;          /// [BYTE 3]       & 3f= 00000000 00000000 00xxxxxx  then header | 80 (10000000)
    } else if(s[a]<= 0x001FFFFF) {	//  4 bytes	U-00010000�U-001FFFFF:   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx 
      *p++=  (s[a]>> 18)       | 0xF0;          /// [BYTE 1]      >> 18= 00000xxx 00000000 00000000 00000000  then header | f0 (11110000)
      *p++= ((s[a]>> 12)& 0x3F)| 0x80;          /// [BYTE 2] >> 12 & 3f= 00000000 00xxxxxx 00000000 00000000  then header | 80 (10000000)
      *p++= ((s[a]>>  6)& 0x3F)| 0x80;          /// [BYTE 3] >>  6 & 3f= 00000000 00000000 00xxxxxx 00000000  then header | 80 (10000000)
      *p++=  (s[a]&       0x3F)| 0x80;          /// [BYTE 4]       & 3f= 00000000 00000000 00000000 00xxxxxx  then header | 80 (10000000)

// last 2 bytes, UNUSED by utf ATM, but there be the code
    } else if(s[a]<= 0x03FFFFFF) {  //  5 bytes U-00200000�U-03FFFFFF:   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 
      *p++=  (s[a]>> 24)       | 0xF8;          /// [BYTE 1]      >> 24= 000000xx 00000000 00000000 00000000 00000000  then header | f8 (11111000)
      *p++= ((s[a]>> 18)& 0x3f)| 0x80;          /// [BYTE 2] >> 18 & 3f= 00000000 00xxxxxx 00000000 00000000 00000000  then header | 80 (10000000)
      *p++= ((s[a]>> 12)& 0x3f)| 0x80;          /// [BYTE 3] >> 12 & 3f= 00000000 00000000 00xxxxxx 00000000 00000000  then header | 80 (10000000)
      *p++= ((s[a]>>  6)& 0x3f)| 0x80;          /// [BYTE 4] >>  6 & 3f= 00000000 00000000 00000000 00xxxxxx 00000000  then header | 80 (10000000)
      *p++=  (s[a]&       0x3f)| 0x80;          /// [BYTE 5]       & 3f= 00000000 00000000 00000000 00000000 00xxxxxx  then header | 80 (10000000)
    } else if(s[a]<= 0x7FFFFFFF) {  //  6 bytes U-04000000�U-7FFFFFFF:   1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
      *p++=  (s[a]>> 30)       | 0xFC;          /// [BYTE 1]      >> 30= 0000000x 00000000 00000000 00000000 00000000 00000000  then header | fc (11111100)
      *p++= ((s[a]>> 24)& 0x3f)| 0x80;          /// [BYTE 2] >> 24 & 3f= 00000000 00xxxxxx 00000000 00000000 00000000 00000000  then header | 80 (10000000)
      *p++= ((s[a]>> 18)& 0x3f)| 0x80;          /// [BYTE 3] >> 18 & 3f= 00000000 00000000 00xxxxxx 00000000 00000000 00000000  then header | 80 (10000000)
      *p++= ((s[a]>> 12)& 0x3f)| 0x80;          /// [BYTE 4] >> 12 & 3f= 00000000 00000000 00000000 00xxxxxx 00000000 00000000  then header | 80 (10000000)
      *p++= ((s[a]>>  6)& 0x3f)| 0x80;          /// [BYTE 5] >>  6 & 3f= 00000000 00000000 00000000 00000000 00xxxxxx 00000000  then header | 80 (10000000)
      *p++=  (s[a]&       0x3f)| 0x80;          /// [BYTE 6]         3f= 00000000 00000000 00000000 00000000 00000000 00xxxxxx  then header | 80 (10000000)
    }
  *p= 0;                                        // string terminator

  return *this;
}

// WINDOWS COMPATIBILITY
string8 &string8::operator=(const ushort *s) {
  delData();

  nrchars= strlenWin(s);
  if(!nrchars) return *this;
  
/// length in bytes of d
  for(size_t a= 0; a< nrchars; a++)	 // for each character in s
    if(s[a]<=      0x007F) len++;   /// 1 byte  U-00000000�U-0000007F:  0xxxxxxx 
    else if(s[a]<= 0x07FF) len+= 2; /// 2 bytes U-00000080�U-000007FF:  110xxxxx 10xxxxxx 
    else if(s[a]<= 0xFFFF) len+= 3; /// 3 bytes U-00000800�U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx 
                                  // windows wide char fits in max 3 bytes
/// create d
  d= new char[len+ 1];
  uchar *p= (uchar *)d;

  for(size_t a= 0; a< nrchars; a++)// for each character in s
    if(s[a]<= 0x007F) {         //  1 byte   U-00000000�U-0000007F:  0xxxxxxx 
      *p++=   s[a];
    } else if(s[a]<= 0x07FF) {  //  2 bytes  U-00000080�U-000007FF:  110xxxxx 10xxxxxx 
      *p++=  (s[a]>> 6)        | 0xC0;      /// [BYTE 1]       >> 6= 000xxxxx 00000000  then header | c0 (11000000)
      *p++=  (s[a]&       0x3f)| 0x80;      /// [BYTE 2]         3f= 00000000 00xxxxxx  then header | 80 (10000000)
    } else if(s[a]<= 0xFFFF) {  //  3 bytes  U-00000800�U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx 
      *p++=  (s[a]>> 12)       | 0xE0;      /// [BYTE 1]      >> 12= 0000xxxx 00000000 00000000  then header | e0 (11100000)
      *p++= ((s[a]>> 6)&  0x3F)| 0x80;      /// [BYTE 2]  >> 6 & 3f= 00000000 00xxxxxx 00000000  then header | 80 (10000000)
      *p++=  (s[a]&       0x3F)| 0x80;      /// [BYTE 3]       & 3f= 00000000 00000000 00xxxxxx  then header | 80 (10000000)
    }
  *p= 0;                                    // string terminator

  return *this;
}


string8 &string8::operator=(const ulong c) {
  ulong buf[2]= { c, 0 };
  return *this= buf;
}


// OPERATOR += //
///-----------///

// this one is heavily used
string8 &string8::operator+=(const string8 &s) {
  if(!s.len) return *this;            /// other string8 empty -> return *this
  if(!len) return *this= s;           /// current string8 empty -> just copy other string8
  
  char *p= new char[len+ s.len+ 1];   /// new string8 allocation
  
/// copy string8 1 to new string8
  uint a= 0;
  for(a= 0; a< len; a++)
    p[a]= d[a];

  delete[] d;
  d= p;
/// copy string8 2 to new string8
  p+= a;
  for(a= 0; a< s.len+ 1; a++)         /// +1= terminator
    *p++= s.d[a];

/// string sizes 
  nrchars+= s.nrchars;
  len+= s.len;
  return *this;
}


string8 &string8::operator+=(const char *s) {
  if(!s) return *this;
  if(!len) return *this= s;

  string8 p= s;
  return *this+= p;
}


string8 &string8::operator+=(const ushort *s) {
  if(!s) return *this;
  if(!len) return *this= s;

  string8 p= s;
  return *this+= p;
}


string8 &string8::operator+=(const ulong *s) {
  if(!s) return *this;
  if(!len) return *this= s;

  string8 p= s;
  return *this+= p;
}


string8 &string8::operator+=(const ulong c) {
  if(!c) return *this;
  if(!len) return *this= c;

  string8 p= c;
  return *this+= p;
}


// CONVERSION operators //
///--------------------///

// UTF-8 conversion from UTF-32 or basically an array of ulongs (internal format)




ulong *string8::convert32() {
  if(!len) return null;

  ulong *buf32= new ulong[nrchars+ 1];

// copy
  cuchar *p= (cuchar*)d;

  for(size_t a= 0; a< nrchars+ 1; a++) {       // for each character
/// character is ascii 0-127
    if(*p < 128) {
      buf32[a]= *p++;
      continue;
    }
/// character has 2 bytes
    if((*p& 0xe0) == 0xc0) {                  /// test if first 3 bits (0xe0= 11100000) are 110xxxxx (0xc0= 11000000)
      buf32[a]= (*p++)& 0x1f;                 /// [byte 1] last 5 bits (0x1f= 00011111)
      buf32[a]<<=6; buf32[a]+= (*p++)& 0x3f;  /// [byte 2] last 6 bits (0x3f= 00111111)
      continue;
    }
/// character has 3 bytes
    if((*p& 0xf0) == 0xe0) {                  /// test if first 4 bits (0xf0= 11110000) are 1110xxxx (0xe0= 11100000)
      buf32[a]= (*p++)& 0x0f;                 /// [byte 1] last 4 bits (0x0f= 00001111)
      buf32[a]<<= 6; buf32[a]+= (*p++)& 0x3f; /// [byte 2] last 6 bits (0x3f= 00111111)
      buf32[a]<<= 6; buf32[a]+= (*p++)& 0x3f; /// [byte 3] last 6 bits (0x3f= 00111111)
      continue;
    }
/// character has 4 bytes
    if((*p& 0xf8) == 0xf0) {                  /// test if first 5 bits (0xf8= 11111000) are 11110xxx (0xf0= 11110000)
      buf32[a]= (*p++)& 0x07;                 /// [byte 1] last 3 bits (0x07= 00000111)
      buf32[a]<<= 6; buf32[a]+= (*p++)& 0x3f; /// [byte 2] last 6 bits (0x3f= 00111111)
      buf32[a]<<= 6; buf32[a]+= (*p++)& 0x3f; /// [byte 3] last 6 bits (0x3f= 00111111)
      buf32[a]<<= 6; buf32[a]+= (*p++)& 0x3f; /// [byte 4] last 6 bits (0x3f= 00111111)
      continue;
    }
// the last 2 bytes are not used, but avaible if in the future unicode will expand
/// character has 5 bytes
    if((*p& 0xfc) == 0xf8) {                      /// test if first 6 bits (0xfc= 11111100) are 111110xx (0xf8= 11111000)
      buf32[a]= (*p++)& 0x03;                   /// [byte 1]   last 2 bits (0x03= 00000011)
      for(char b= 0; b< 4; b++)
        buf32[a]<<= 6; buf32[a]+= (*p++)& 0x3f; /// [byte 2-5] last 6 bits (0x3f= 00111111)
      continue;
    }
/// character has 6 bytes
    if((*p& 0xfe) == 0xfc) {        /// test if first 7 bits (0xfe= 11111110) are 1111110x (0xfc= 11111100)
      buf32[a]= (*p++)& 0x01;                   /// [byte 1]   last bit    (0x01= 00000001)
      for(char b= 0; b< 5; b++)
        buf32[a]<<= 6; buf32[a]+= (*p++)& 0x3f; /// [byte 2-6] last 6 bits (0x3f= 00111111)
      continue;
    }
  }	/// for each character
  
  if(buf32_string) delete[] buf32_string;
  return buf32_string= buf32;
}


// windows compatibility
ushort *string8::convertWin() {
  if(!len) return null;

/// static buffer alloc (this buffer is auto-deleted @ destructor call)
  if(bufwin_string) delete[] bufwin_string;
  bufwin_string= new ushort[nrchars+ 1];

// copy
  cuchar *p= (cuchar*)d;

  for(size_t a= 0; a< nrchars+ 1; a++) {          // for each character
/// character is ascii 0-127
    if(*p < 128) {
      bufwin_string[a]= *p++;
      continue;
    }
/// character has 2 bytes
    if((*p& 0xe0) == 0xc0) {                                  /// test if first 3 bits (0xe0= 11100000) are 110xxxxx (0xc0= 11000000)
      bufwin_string[a]= (*p++)& 0x1f;                         /// [byte 1] last 5 bits (0x1f= 00011111)
      bufwin_string[a]<<=6; bufwin_string[a]+= (*p++)& 0x3f;  /// [byte 2] last 6 bits (0x3f= 00111111)
      continue;
    }
/// character has 3 bytes
    if((*p& 0xf0) == 0xe0) {                                  /// test if first 4 bits (0xf0= 11110000) are 1110xxxx (0xe0= 11100000)
      bufwin_string[a]= (*p++)& 0x0f;                         /// [byte 1] last 4 bits (0x0f= 00001111)
      bufwin_string[a]<<= 6; bufwin_string[a]+= (*p++)& 0x3f; /// [byte 2] last 6 bits (0x3f= 00111111)
      bufwin_string[a]<<= 6; bufwin_string[a]+= (*p++)& 0x3f; /// [byte 3] last 6 bits (0x3f= 00111111)
      continue;
    }
/// character has 4 bytes
    if((*p& 0xf8) == 0xf0) {              /// test if first 5 bits (0xf8= 11111000) are 11110xxx (0xf0= 11110000)
      bufwin_string[a]= 0xFFFD;           // windows wide char can't handle unicode values greater than 2 bytes (lulz)
      p++;
      continue;
    }
// the last 2 bytes are not used, but avaible if in the future unicode will expand
/// character has 5 bytes
    if((*p& 0xfc) == 0xf8) {              /// test if first 6 bits (0xfc= 11111100) are 111110xx (0xf8= 11111000)
      bufwin_string[a]= 0xFFFD;           // windows wide char can't handle unicode values greater than 2 bytes (lulz)
      p++;
      continue;
    }
/// character has 6 bytes
    if((*p& 0xfe) == 0xfc) {              /// test if first 7 bits (0xfe= 11111110) are 1111110x (0xfc= 11111100)
      bufwin_string[a]= 0xFFFD;           // windows wide char can't handle unicode values greater than 2 bytes (lulz)
      p++;
      continue;
    }
  }	/// for each character
  
  return bufwin_string;
}


// operator[] / getChar(..) //
///------------------------///

cchar *string8::operator[](uint n) const {
  cuchar *p= (cuchar *)d;

  for(uint a= 0; a< n; p++)
    if((*p& 0xc0)!= 0x80)   /// 0xc0= 11000000, first two bits  0x80= 10000000, test for 10xxxxxx the last 6 bits wont matter, as they are not tested so test to 10000000
      a++;

  return (cchar*)p;
}


ulong string8::getChar(uint n) const {
  return utf8to32(this->operator[](n));
}




///------------------------///
// OPERATOR - / OPERATOR -= // clears n chars from string8s
///------------------------///


string8 &string8::operator-=(int n) {
/// basic checks
  if(!n) return *this;
  if(nrchars- n<= 0) {
    delData();
    return *this;
  }
  
  return *this= (*this).operator-(n);     /// ... heh make shure it is the operator-
}


string8 string8::operator-(int n) const {
/// basic checks
  if(!n) return string8(*this);
  if(nrchars- n<= 0) return string8();

  string8 ret(*this);             /// return string
 
/// exclude n chars from string
  uchar *p= (uchar *)ret.d;
  p+= ret.len- 1;                 /// set p as the last byte (not the terminator)
  

  for(uint a= 0; a< n; a++, p--)  /// decrease p, n times
    while(((*p)& 0xc0)== 0x80)    /// decrease p for every continuation byte in current char
      p--;

  *(p+ 1)= 0;                     // set terminator over this char = the string is cut n chars

/// update string length
  ret.len= strlen(ret.d);
  ret.nrchars-= n;

/// prepare to copy
  char *t= ret.d;                 /// t will be the old string
  ret.d= new char[ret.len+ 1];    /// create the new string
//copy
  for(uint a= 0; a< ret.len+ 1; a++)
    ret.d[a]= t[a];

  delete[] t;
  
  return ret;
}



///-----------///
// OPERATOR == //
///-----------///

bool string8::operator==(const char *s) const {
  if(!s) {
    if(!len)  return true;
    else      return false;
  }

  for(size_t a= 0; a< len; a++)
    if(d[a]!= s[a])
      return false;

  return true;
} /// checks if strings are identical (utf-8)

bool string8::operator==(char *s) const {
  if(!s) {
    if(!len)  return true;
    else      return false;
  }

  for(size_t a= 0; a< len; a++)
    if(d[a]!= s[a])
      return false;

  return true;
}


bool string8::operator==(const string8 &s) const {
  if(s.len!= len) return false;

  for(size_t a= 0; a< len; a++)
    if(d[a]!=s.d[a])
      return false;

  return true; // this point reached -> there is no difference between string8s
}


bool string8::operator==(culong *s) const {
  if(!s) {
    if(!d)  return true;
    else    return false;
  }
  string8 t(s);
  return (*this).operator==(t);
}


bool string8::operator==(cushort *s) const {
  if(!s) {
    if(!d)  return true;
    else    return false;
  }
  string8 t(s);
  return (*this).operator==(t);
}


bool string8::operator==(culong c) const {
  if(nrchars!= 1) return false;
  return (utf8to32(d) == c);
}




///-----------------------------------------///
// COMBining diacritical character functions //
///-----------------------------------------///

bool string8::isComb(culong c) {
/// 0300-036F Combining Diacritical Marks, since version 1.0, with modifications in subsequent versions down to 4.1
  if( (c>= 0x0300)&& (c<= 0x036f) ) return true;
/// 1DC0-1DFF Combining Diacritical Marks Supplement, versions 4.1 to 5.2
  if( (c>= 0x1dc0)&& (c<= 0x1dff) ) return true;
/// 20D0-20FF Combining Diacritical Marks for Symbols, since version 1.0, with modifications in subsequent versions down to 5.1
  if( (c>= 0x20d0)&& (c<= 0x20ff) ) return true;
/// FE20-FE2F Combining Half Marks, versions 1.0, updates in 5.2
  if( (c>= 0xfe20)&& (c<= 0xfe2f) ) return true;
  return false;   
}


void string8::clearComb() {
  if(!len) return;
  ulong *u= convert32();

///count comb's
  size_t n= 0;
  for(size_t a= 0; a< nrchars; a++)
    if(isComb(u[a]))
      n++;

/// if found combining diacriticals, recreate the string without them
  if(n) {
    ulong *t= new ulong[nrchars- n+ 1];
    for(size_t a= 0, b= 0; a< nrchars; a++)
      if(!isComb(u[a]))
        t[b++]= u[a];
    
    *this= t;     /// update internals
    delete[] t;
  } /// if found diacriticals
}



///----------------------------------------------///
// UTILITY FUNCTIONS, usually good for any string //
///----------------------------------------------///

void string8::updateLen() {
  if(!d) { len= 0; nrchars= 0; return; }
  len= strlen(d);
  nrchars= strchars(d);
}

/// returns string length in bytes
size_t string8::strlen(const char *s) {
  const char *p= s;
  while(*p++);
  return p- s- 1;
}

/// returns string length (with all combined chars) for a 32bit string
size_t string8::strlen32(const ulong *s) {
  const ulong *p= s;
  while(*p++);
  return p- s- 1;
}

/// returns string length for a 16bit string (windows compatibility) NOT UTF-16
size_t string8::strlenWin(const ushort *s) {
  const ushort *p= s;
  while(*p++);
  return p- s- 1;
}

// counts the number of characters in a utf-8 string8 (totals - both chars and combs)
size_t string8::strchars(const char *s) {
  const uchar *p= (const uchar *)s;
  size_t l= 0;
  
  while(*p++)
    if((*p& 0xc0)!= 0x80)   /// 0xc0= 11000000, first two bits  0x80= 10000000, test for 10xxxxxx the last 6 bits wont matter, as they are not tested so test to 10000000
      l++;
      
  return l;
}

// returns number of combined diacriticals in a utf-8 string
size_t string8::strcombs(cchar *s) {
  size_t ret= 0;
  cuchar *p= (cuchar *)s;
  ulong a;
  
  while(1) {                             /// while till terminator found
/// character is ascii 0-127
    if(*p < 128) {
      a= *p++;
/// character has 2 bytes
    } else if((*p& 0xe0) == 0xc0) {      /// test if first 3 bits (0xe0= 11100000) are 110xxxxx (0xc0= 11000000)
      a=  *p++ & 0x1f; a<<=6;
      a+= *p++ & 0x3f;
/// character has 3 bytes
    } else if((*p& 0xf0) == 0xe0) {      /// test if first 4 bits (0xf0= 11110000) are 1110xxxx (0xe0= 11100000)
      a=  *p++ & 0x0f; a<<= 6;
      a+= *p++ & 0x3f; a<<= 6;
      a+= *p++ & 0x3f;
/// character has 4 bytes
    } else if((*p& 0xf8) == 0xf0) {      /// test if first 5 bits (0xf8= 11111000) are 11110xxx (0xf0= 11110000)
      a=  *p++ & 0x07; a<<= 6;
      a+= *p++ & 0x3f; a<<= 6;
      a+= *p++ & 0x3f; a<<= 6;
      a+= *p++ & 0x3f;
// the last 2 bytes are not used, but avaible if in the future unicode will expand
/// character has 5 bytes
    } else if((*p& 0xfc) == 0xf8) {      /// test if first 6 bits (0xfc= 11111100) are 111110xx (0xf8= 11111000)
      a=  *p++ & 0x03;
      for(char b= 0; b< 4; b++)
        { a<<= 6; a+= *p++ & 0x3f; }
/// character has 6 bytes
    } else if((*p& 0xfe) == 0xfc) {      /// test if first 7 bits (0xfe= 11111110) are 1111110x (0xfc= 11111100)
      a=  *p++ & 0x01;
      for(char b= 0; b< 5; b++)
        { a<<= 6; a+= *p++ & 0x3f; }
    }
    if(!a) break;
    if(isComb(a))
      ret++;
    a= 0;
  }	/// while all characters
  return ret;
}


ulong string8::utf8to32(const char *s) {
  cuchar *p= (cuchar*)s;
  ulong ret= 0;

/// unpack character and return it
  if(*p < 128) {
    ret= *p++;
  } else if((*p& 0xe0) == 0xc0) {/// character has 2 bytes
    ret=  *p++ & 0x1f; ret<<=6;
    ret+= *p++ & 0x3f;
  } else if((*p& 0xf0) == 0xe0) {/// character has 3 bytes
    ret=  *p++ & 0x0f; ret<<= 6;
    ret+= *p++ & 0x3f; ret<<= 6;
    ret+= *p++ & 0x3f;
  } else if((*p& 0xf8) == 0xf0) {/// character has 4 bytes
    ret=  *p++ & 0x07; ret<<= 6;
    ret+= *p++ & 0x3f; ret<<= 6;
    ret+= *p++ & 0x3f; ret<<= 6;
    ret+= *p++ & 0x3f;
// the last 2 bytes are not used, but avaible if in the future unicode will expand
  } else if((*p& 0xfc) == 0xf8) {/// character has 5 bytes
    ret=  *p++ & 0x03;
    for(char b= 0; b< 4; b++)
      { ret<<= 6; ret+= *p++ & 0x3f; }
  } else if((*p& 0xfe) == 0xfc) {/// character has 6 bytes
    ret=  *p++ & 0x01;
    for(char b= 0; b< 5; b++)
      { ret<<= 6; ret+= *p++ & 0x3f; }
  }
  return ret;
}


/// gets n-th char in string8 s (format is utf-8)
ulong string8::utf8to32n(const char *s, int n) {
  cuchar *p= (cuchar*)s;
/// pass thru all characters, till n is reached
  for(size_t a= 0; a< n;)
    if((*p++ & 0xc0)!= 0x80)
      a++;

  return utf8to32((const char*)(p- 1));
}


size_t string8::strcmp(cchar *s1, cchar *s2) {
  size_t l1= strlen(s1);
  size_t l2= strlen(s2);
  /// if sizes are the same, proceed to test each character
  if(l1== l2) {
    for(size_t a= 0; a< l1; a++)
      if(s1[a]!= s2[a])
        return -1;

    return 0;
  } else
    /// sizes differ, return difference (if l1< l2, ret will be negative, and viceversa)
    return l1- l2;
}




///--------------------------------///
// UTF-8 SECURITY / READ FROM FILES // (any read from a file should be considered UNSAFE)
///--------------------------------///


// bad characters will be MARKED with 0xFFFD

// reading from UNSAFE SOURCES / FILES / INPUT is NOT SAFE. use secureUTF8() to validate a utf8 string8
string8 &string8::secureUTF8(const char *s) {
  delData();
  
/// length in characters
  const uchar *p= (const uchar*)s;

  // can't assume that every character is ok, there can be garbage @ every level
  while(*p++) {
    if(*p < 128) nrchars++;                   /// valid char if less than 128
    // search for headers, everything else is ignored.
    else if((*p& 0xe0) == 0xc0) nrchars++;    /// header for 2 chars
    else if((*p& 0xf0) == 0xe0) nrchars++;    /// header for 3 chars
    else if((*p& 0xf8) == 0xf0) nrchars++;    /// header for 4 chars
    else if((*p& 0xfc) == 0xf8) nrchars++;    /// header for 5 chars  marked as bad
    else if((*p& 0xfe) == 0xfc) nrchars++;    /// header for 6 chars  marked as bad
  }


/// copy to temporary unpacked string
  ulong *u= new ulong[nrchars+ 1];      /// UNPACKED STRING

  p= (const uchar*)s;
  size_t a= 0;
  while(a< nrchars) {                   /// for each (valid) character
/// character is ascii 0-127
    if(*p < 128) {                      /// 1 byte characters are safe
      u[a++]= *p++;
      continue;
/// character has 2 bytes
    } else if((*p& 0xe0) == 0xc0) {     /// header for 2 bytes? (0xe0= 11100000)
    // [BYTE 1] 
                     // test for overlong bytes 1100000x (10xxxxxx)
      if((*p& 0x1e) == 0) {             /// 1e= 00011110 if these 4 bits are 0, this is a overlong byte
        u[a++]= 0xFFFD;
        p++;
        continue;
      }
      u[a]= *p++& 0x1f;                 /// byte seems ok - copy from it (0x1f= 00011111)

    // [BYTE 2] 
      if((*p& 0xc0)!= 0x80) {           /// byte 2 has to be a continuation byte (start with 10xxxxxx)
        u[a++]= 0xFFFD;
        continue;                       /// don't increase p
      }
      u[a]<<=6; u[a]+= *p++ & 0x3f;     /// byte seems ok - copy from it (0x3f= 00111111)

/// character has 3 bytes
  } else if((*p& 0xf0) == 0xe0) {       /// header for 3 bytes ? (0xf0= 11110000)
    // [BYTE 1]
                     // test for overlong bytes 11100000 100xxxxx (10xxxxxx) 
      if(     ((*p& 0x0f) == 0) &&      ///  f= 00001111               
         ((*(p+ 1)& 0x20) == 0)) {      /// 20=          00100000 if these bits are 0, this is a overlong byte
        u[a++]= 0xFFFD;
        p++;
        continue;
      }
      u[a]= *p++& 0x0f;                 /// byte seems ok - copy from it (0x0f= 00001111)

    // [BYTE 2-3]
      for(short b= 0; b< 2; b++) {
        if((*p& 0xc0)!= 0x80) {         /// bytes 2-3 have to be continuation bytes (start with 10xxxxxx)
          u[a++]= 0xFFFD;
          goto cont;                    /// don't increase p
        }
        u[a]<<= 6; u[a]+= *p++& 0x3f;   /// byte seems ok - copy from it (0x3f= 00111111)
      }
/// character has 4 bytes
    } else if((*p& 0xf8) == 0xf0) {     /// header for 4 bytes ? (0xf8= 11111000)
    // [BYTE 1]
                     // test for overlong bytes 11110000 1000xxxx (10xxxxxx 10xxxxxx) 
      if(      ((*p& 0x7) == 0) &&      ///  7= 00000111               
         ((*(p+ 1)& 0x30) == 0)) {      /// 30=          00110000 if these bits are 0, this is a overlong byte
        u[a++]= 0xFFFD;
        p++;
        continue;
      }
      u[a]= *p++& 0x07;                 /// byte seems ok - copy from it (0x07= 00000111)

    // [BYTE 2-4]
      for(short b= 0; b< 3; b++) {
        if((*p& 0xc0)!= 0x80) {         /// bytes 2-4 have to be continuation bytes (start with 10xxxxxx)
          u[a++]= 0xFFFD;
          goto cont;                    /// don't increase p
        }
        u[a]<<= 6; u[a]+= *p++ & 0x3f;  /// byte seems ok - copy from it (0x3f= 00111111)
      }
// the last 2 bytes are not used, but avaible if in the future unicode will expand
/// character has 5 bytes
    } else if((*p& 0xfc) == 0xf8) {     /// header for 5 bytes ? (0xfc= 11111100)
      u[a++]= 0xFFFD;
      p++;
      continue;

/* >>>>>>>>>>>>>>>>>> UNCOMMENT THIS IF UTF-8 WILL USE 5 BYTES IN THE FUTURE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // [BYTE 1]
                     // test for overlong bytes 11111000 10000xxx (10xxxxxx 10xxxxxx 10xxxxxx) 
      if(     (*p& 0x3  == 0) &&        ///  3= 00000011               
         (*(p+ 1)& 0x38 == 0)) {        /// 38=          00111000 if these bits are 0, this is a overlong byte
        u[a++]= 0xFFFD;
        p++;
        continue;
      }
      u[a]= *p++& 0x03;                 /// byte seems ok - copy from it (0x03= 00000011)

    // [BYTE 2-5]
      for(short b= 0; b< 4; b++) {
        if((*p& 0xc0)!= 0x80) {         /// bytes 2-5 have to be continuation bytes (start with 10xxxxxx)
          u[a++]= 0xFFFD;
          goto cont;                    /// don't increase p
        }
        u[a]<<= 6; u[a]+= *p++ & 0x3f;  /// byte seems ok - copy from it (0x3f= 00111111)
      }
*/
    
/// character has 6 bytes
    } else if((*p& 0xfe) == 0xfc) {              /// header for 6 bytes ? (0xfe= 11111110)
      u[a++]= 0xFFFD;
      p++;
      continue;

/* >>>>>>>>>>>>>>>>>> UNCOMMENT THIS IF UTF-8 WILL USE 6 BYTES IN THE FUTURE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // [BYTE 1]
                     // test for overlong bytes 11111100 100000xx (10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx)
      if(     (*p& 0x1  == 0) &&        ///  1= 00000001               
         (*(p+ 1)& 0x3C == 0)) {        /// 3C=          00111100 if these bits are 0, this is a overlong byte
        u[a++]= 0xFFFD;
        p++;
        continue;
      }
      u[a]= *p++& 0x01;                 /// byte seems ok - copy from it (0x01= 00000001)

    // [BYTE 2-6]
      for(short b= 0; b< 5; b++) {
        if((*p& 0xc0)!= 0x80) {         /// bytes 2-6 have to be continuation bytes (start with 10xxxxxx)
          u[a++]= 0xFFFD;
          goto cont;                    /// don't increase p
        }
        u[a]<<= 6; u[a]+= *p++ & 0x3f;  /// byte seems ok - copy from it (0x3f= 00111111)
      }
*/
    } else {
      p++;                         // character unreadable
      continue;
    }
    
  /// test result character d[a]
    if(u[a]>= 0x10FFFF)                 // limit of Unicode <--------------------- (maybe this changes in the future)
      u[a]= 0xFFFD;

    if( (u[a]>= 0xD800) && (u[a]<= 0xDFFF) )  /// it can't be a utf-16 surrogate
      u[a]= 0xFFFD;

    // further tests can be done here. there are still some chars that should be marked as malformed (0xFFFD) <<<=========================


    a++;
cont:
    ;
  }	/// for each character

  u[a]= 0;                              /// terminator

/// compress unpacked string to *this
  *this= u;
  delete[] u;

  return *this;
}






// fopen knows of utf-8 but the win version wants to put a fukin BOM in the file, wich cause problems in linux, so file must be opened as pure binary

// read all file                   (validates each char)
void string8::readUTF8(FILE *f) {
/// read / ignore the BOM in an UTF file
  ushort bom;
  uint pos= ftell(f);
  if(fread(&bom, 2, 1, f)) {
    if(bom!= 0xFEFF)
      fseek(f, (long)pos, SEEK_SET);  /// go back if it is not the BOM
  } else
    fseek(f, pos, SEEK_SET);          /// maybe it read 1 byte? just go back anyways

/// determine remaining filesize in bytes (maybe it is not reading from the start)
	uint fs;
	fseek(f, 0L, SEEK_END);
	fs= ftell(f);
	fseek(f, (long)pos, SEEK_SET);
  fs-= pos;

  if(!fs)
    return;

/// read all file
  char *buffer= new char[fs+ 1];
  fread(buffer, 1, fs, f);
  buffer[fs]= 0;                      /// terminator

  secureUTF8(buffer);

  delete[] buffer;
}



// read n characters from file     (validates each char)
void string8::readUTF8n(FILE *f, size_t n) {
/// read / ignore the BOM in an UTF file
  ushort bom;
  uint pos= ftell(f);
  if(fread(&bom, 2, 1, f)) {
    if(bom!= 0xFEFF)
      fseek(f, (long)pos, SEEK_SET);  /// go back if it is not the BOM
  } else
    fseek(f, pos, SEEK_SET);          /// maybe it read 1 byte? just go back anyways

/// determine remaining filesize in bytes (maybe it is not reading from the start)
	uint fs;
	fseek(f, 0L, SEEK_END);
	fs= ftell(f);
	fseek(f, (long)pos, SEEK_SET);
  fs-= pos;

  if(!fs)                             /// remaining filesize is 0 ?
    return;
  
  if(n> fs)                           /// it will read what it can
    n= fs;

/// read n bytes from file
  char *buffer= new char[n+ 1];
  fread(buffer, 1, n, f);
  buffer[n]= 0;                       /// terminator

  secureUTF8(buffer);

  delete[] buffer;
}

/// read till end of line (or file) (validates each char)
void string8::readLineUTF8(FILE *f) {
/// read / ignore the BOM in an UTF file
  ushort bom;
  uint pos= ftell(f);
  if(fread(&bom, 2, 1, f)) {
    if(bom!= 0xFEFF)
      fseek(f, (long)pos, SEEK_SET);  /// go back if it is not the BOM
  } else
    fseek(f, pos, SEEK_SET);          /// maybe it read 1 byte? just go back anyways

/// compute line length
  uint l;
  char r;

  while(fread(&r, 1, 1, f))
    if(r == '\n')
      break;
      
  l= ftell(f);
  l-= pos;
  if(!l) return;

/// read line from file
  char *buffer= new char[l+ 1];
  fseek(f, pos, SEEK_SET);
  fread(buffer, 1, l, f);
  buffer[l]= 0;                     /// terminator

  secureUTF8(buffer);

  delete[] buffer;
}







