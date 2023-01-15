#pragma once
#include "rincewind_globals.h"

struct buffer
{
    uint8* Data;
    int Size;
};
typedef buffer string;

#define BundleString(S) MakeString(S, strlen(S))
inline function string
MakeString(uint8* Address, int Size)
{
    string NewString;
    NewString.Data = Address;
    NewString.Size = Size;
    return(NewString);
}

inline internal unsigned int
GenerateHash(const buffer& Buffer)
{
	unsigned int Result = 0;
	const unsigned int Prime = 31;
	for(int i = 0; i < Buffer.Size; ++i)
	{
		Result = Buffer.Data[i] + (Result * Prime);
	}
	return(Result);
}

inline function bool
IsLittleEndian()
{
    int One = 1;
    return (*((uint8*)&One) == 1);
}

#define ChangeEndianness16(n) n<<8 | n >>8;
#define ChangeEndianness32(n) ((n>>24)&0xff) | ((n<<8)&0xff0000) | ((n>>8)&0xff00) | ((n<<24)&0xff000000)