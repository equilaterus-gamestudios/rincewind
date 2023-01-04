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