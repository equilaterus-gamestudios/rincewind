#pragma once
#include "rincewind_globals.h"
#include "rincewind_memory.h"
#include <string.h>
#include <assert.h>

struct buffer
{
    uint8* Data;
    int32 Size;
};
typedef buffer string;

#define BundleStringC(S) MakeString((uint8*)S, strlen(S))
#define BundleString(S) MakeString(S, strlen((char*)S))
inline function string
MakeString(uint8* Address, int32 Size)
{
    string NewString;
    NewString.Data = Address;
    NewString.Size = Size;
    return(NewString);
}

inline internal uint32
GenerateHash(void* MemoryBlock, uint32 SizeInBytes)
{
    uint8* Ptr = (uint8*)MemoryBlock;
    uint32 Result = 0;
    for(int i = 0; i < SizeInBytes; ++i)
	{
		Result = Ptr[i] + (Result * 31);
	}
    return(Result);
}

inline function bool
IsLittleEndian()
{
    int32 One = 1;
    return (*((uint8*)&One) == 1);
}

#define ChangeEndianness16(n) n<<8 | n >>8;
#define ChangeEndianness32(n) ((n>>24)&0xff) | ((n<<8)&0xff0000) | ((n>>8)&0xff00) | ((n<<24)&0xff000000)

#define UNLIKELY(x) __builtin_expect((bool)x, 0)
#define Max(a,b) a < b ? b : a
#define Mod(a,b) a >= b ? a % b : a
#define UnlikelyMod(a,b) UNLIKELY((a >= b)) ? a % b : a
