#pragma once
#define wchar wchar_t

#define int8 char
#define int16 short
#define int32 long
#define int64 long long

#define byte unsigned char
#define word unsigned short
#define dword unsigned long

#define ulong unsigned long
#define ushort unsigned short
#define uchar unsigned char
#define uint unsigned int
#define uint8 unsigned char
#define uint16 unsigned short
#define uint32 unsigned long
#define uint64 unsigned long long


#define clong const long
#define cshort const short
#define cchar const char
#define cint const int
#define cint64 const long long

#define culong const unsigned long
#define cushort const unsigned short
#define cuchar const unsigned char
#define cuint const unsigned int
#define cuint64 const unsigned long long

#define cvoid const void

//#define string string8              // ??????????
#define cstring const string8       // ??????????

#define null NULL


#define MAKEUINT32(a,b,c,d) ((unsigned long)((((unsigned char)(d)|((unsigned short)((unsigned char)(c))<<8))|(((unsigned long)(unsigned char)(b))<<16))|(((unsigned long)(unsigned char)(a))<<24)))
#define GETBYTE4UINT32(a) ((unsigned char)(a))
#define GETBYTE3UINT32(a) ((unsigned char)((a)>> 8))
#define GETBYTE2UINT32(a) ((unsigned char)((a)>>16))
#define GETBYTE1UINT32(a) ((unsigned char)((a)>>24))



