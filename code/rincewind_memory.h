#pragma once
#include <stdlib.h>
#include <assert.h>
#include "rincewind_globals.h"
#include "rincewind_common.h"

#define ARENA_SIZE_IN_BYTES Megabytes(10)

//NOTE(pipecaniza): right now the arena is not capable to free resources(linear allocator) to avoid fragmentation
struct arena
{
    uint8* MemoryArena;
    uint32 BufferSize;
};

function arena
MakeArena()
{
    arena Result = {};
    Result.MemoryArena = (uint8*)calloc(1, ARENA_SIZE_IN_BYTES);
    return(Result);
}

function uint8*
PushToMemory(arena* Arena, void* Ptr, uint32 SizeInBytes)
{
    assert(Arena->BufferSize + SizeInBytes < ARENA_SIZE_IN_BYTES);

    uint8* BytePtr = (uint8*)Ptr;
    uint8* BaseAddress = Arena->MemoryArena + Arena->BufferSize;    
    for (uint32 i = 0; i < SizeInBytes; ++i) 
    {
        uint8* CurrentByte = BytePtr + i;
        Arena->MemoryArena[Arena->BufferSize++] = *CurrentByte;
    }
    return(BaseAddress);
}


function void
PushToMemoryAtLocation(arena* Arena, uint8* ArenaPointer, void* Ptr, uint32 SizeInBytes)
{
    // NOTE(pipecaniza): Check that ArenaPointer is inside the arena memory
    assert((ArenaPointer > Arena->MemoryArena && ArenaPointer < Arena->MemoryArena + ARENA_SIZE_IN_BYTES) ||
            (ArenaPointer < Arena->MemoryArena && ArenaPointer > Arena->MemoryArena + ARENA_SIZE_IN_BYTES));
    assert(Arena->BufferSize + SizeInBytes < ARENA_SIZE_IN_BYTES);

    uint8* BytePtr = (uint8*)Ptr;
    uint8* BaseAddress = ArenaPointer;    
    for (uint32 i = 0; i < SizeInBytes; ++i) 
    {
        uint8* CurrentByte = BytePtr + i;
        *(BaseAddress + i) = *CurrentByte;
    }
}


function uint8*
ReserveMemory(arena* Arena, uint32 SizeInBytes)
{
    assert(Arena->BufferSize + SizeInBytes < ARENA_SIZE_IN_BYTES);
    uint8* BaseAddress = Arena->MemoryArena + Arena->BufferSize;
    Arena->BufferSize += SizeInBytes;
    return(BaseAddress);
}


inline function string
AllocateString(arena* Arena, char* Data)
{
    uint8* Address = PushToMemory(Arena, Data, strlen(Data));
    return BundleString(Address);
}
