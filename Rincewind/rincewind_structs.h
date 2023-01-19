#pragma once

#define MAX_MAP_SIZE 65535
struct hash_table
{
    //TODO(pipecaniza): move hash_table alloc from stack to arena
    uint32* Keys;
    uint16* Values;

    uint16 MaxHops;
    uint16 Size;
};


inline function hash_table
MakeHashTable(arena* Arena, uint16 Size)
{
    hash_table Result = {};
    assert(Size < MAX_MAP_SIZE);
    Result.Keys = (uint32*)ReserveMemory(Arena, Size * sizeof(uint32));
    Result.Values = (uint16*)ReserveMemory(Arena, Size * sizeof(uint16));
    Result.Size = Size;
    return(Result);
}

//TODO(pipecaniza): store value
function void
AddElement(hash_table* HashTable, uint32 Hash, uint16 Value)
{
    uint16 Index = Mod(Hash, HashTable->Size);
    uint16 CurrentHops = 0;
    
    while (uint32 CurrentKey = HashTable->Keys[Index] != 0)
    {
        assert(CurrentKey != Hash); //NOTE(pipecaniza): duplicated hash
        assert(CurrentHops < HashTable->Size); // NOTE(pipecaniza): hash table is full
        ++CurrentHops; ++Index;
        Index = UnlikelyMod(Index, HashTable->Size);
    }
    HashTable->Keys[Index] = Hash;
    HashTable->Values[Index] = Value;
    HashTable->MaxHops = Max(HashTable->MaxHops, CurrentHops);
}

function int16
GetIndex(hash_table* HashTable, uint32 Hash)
{
    uint16 Index = Mod(Hash, HashTable->Size);
    for (
        uint16 CurrentHops = 0;
        CurrentHops <= HashTable->MaxHops && HashTable->Keys[Index] != 0;
        ++CurrentHops
    )
    {
        if (HashTable->Keys[Index] == Hash)
        {
            return(Index);
        }
        Index = Mod((Index + 1), HashTable->Size);
    }

    return(-1);
}
