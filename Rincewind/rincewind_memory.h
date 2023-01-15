#pragma once
#include <assert.h>
#include <string.h>
#include "rincewind_globals.h"
#include "rincewind_common.h"


//TODO(pipecaniza): Define the arena size
//TODO(pipecaniza): Move arena to heap
#define ARENA_SIZE 5000000

struct arena
{
    uint8 MemoryArena[ARENA_SIZE];
    int BufferSize = 0;
};


function uint8*
PushToMemory(arena* Arena, void* Ptr, int SizeInBytes)
{
    assert(Arena->BufferSize + SizeInBytes < ARENA_SIZE);

    uint8* BytePtr = (uint8*)Ptr;
    uint8* BaseAddress = Arena->MemoryArena + Arena->BufferSize;    
    for (int i = 0; i < SizeInBytes; ++i) 
    {
        uint8* CurrentByte = BytePtr + i;
        Arena->MemoryArena[Arena->BufferSize++] = *CurrentByte;
    }
    return(BaseAddress);
}


inline function string
AllocateString(arena* Arena, char* Data)
{
    uint8* Address = PushToMemory(Arena, Data, strlen(Data));
    return BundleString(Address);
}
