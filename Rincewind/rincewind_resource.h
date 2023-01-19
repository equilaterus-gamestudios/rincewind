#pragma once
///Project
#include "rincewind_globals.h"
#include "rincewind_common.h"
#include "rincewind_memory.h"
///STL
#include <assert.h>
#include <stdio.h>

// TODO(pipecaniza):check these values
#define MAX_LOCALIZATION_DATA 5000
#define MAX_NON_LOCALIZATION_DATA 5000
#define MAX_IMMEDIATE_STR_DATA 5000
#define MAX_IMMEDIATE_FLOAT_DATA 5000

//NOTE(pipecaniza): should contains only data that will be exported
//NOTE(pipecaniza): Should we store everything as unique to avoid data duplication?
//NOTE(pipecaniza): complete buffer of the region, follow by each data, in case of strings, int followed by char[]
struct resource_container
{
    string* LocalizationDataArray;
    string* NonLocalizationDataArray;
    string* ImmediateStringDataArray;
    float* ImmediateNumberDataArray;

    uint32 LocalizationDataIndex;
    uint32 NonLocalizationDataIndex;
    uint32 ImmediateNumberDataIndex;
    uint32 ImmediateStringDataIndex;

    hash_table LocalizationCache;
    hash_table NonLocalizationCache;
    hash_table ImmediateStringCache;
    hash_table ImmediateNumberCache;
};

inline function resource_container
MakeResourceContainer(arena* Arena)
{
    resource_container Result = {};

    Result.LocalizationDataArray = (string*)ReserveMemory(Arena, sizeof(string) * MAX_LOCALIZATION_DATA);
    Result.NonLocalizationDataArray = (string*)ReserveMemory(Arena, sizeof(string) * MAX_NON_LOCALIZATION_DATA);
    Result.ImmediateStringDataArray = (string*)ReserveMemory(Arena, sizeof(string) * MAX_IMMEDIATE_STR_DATA);
    Result.ImmediateNumberDataArray = (float*)ReserveMemory(Arena, sizeof(float) * MAX_IMMEDIATE_FLOAT_DATA);

    Result.LocalizationCache = MakeHashTable(Arena, MAX_LOCALIZATION_DATA);
    Result.NonLocalizationCache = MakeHashTable(Arena, MAX_NON_LOCALIZATION_DATA);
    Result.ImmediateStringCache = MakeHashTable(Arena, MAX_IMMEDIATE_STR_DATA);
    Result.ImmediateNumberCache = MakeHashTable(Arena, MAX_IMMEDIATE_FLOAT_DATA);
    return(Result);
}


#define InsertResource(Cache, TableData, DataIndex, Data, DataToHash, DataSizeInBytes) \
uint32 Hash =  GenerateHash(DataToHash, DataSizeInBytes);\
int16 Index = GetIndex(&Cache, Hash);\
if (Index == -1)\
{\
    AddElement(&Cache, Hash, DataIndex);\
    TableData[DataIndex] = Data;\
    return (DataIndex++);\
}\
return Cache.Values[Index];


function uint16
InsertLocalization(resource_container* Resource, const string& String)
{
    assert(Resource->LocalizationDataIndex < MAX_LOCALIZATION_DATA);
    InsertResource(Resource->LocalizationCache, Resource->LocalizationDataArray, Resource->LocalizationDataIndex, String, String.Data, String.Size)
}

function uint16
InsertNonLocalizationData(resource_container* Resource, const string& String)
{
    assert(Resource->NonLocalizationDataIndex < MAX_NON_LOCALIZATION_DATA);
    InsertResource(Resource->NonLocalizationCache, Resource->NonLocalizationDataArray, Resource->NonLocalizationDataIndex, String, String.Data, String.Size)
}
/*
function uint16
InsertImmediateStringData(resource_container* Resource, const string& String)
{
    assert(Resource->ImmediateStringDataIndex < MAX_IMMEDIATE_STR_DATA);
    InsertResource(Resource->ImmediateStringCache, Resource->ImmediateStringData, Resource->ImmediateStringDataIndex, String, String.Data, String.Size)
}*/

function uint16
InsertImmediateNumberData(resource_container* Resource, float Number)
{
    assert(Resource->ImmediateNumberDataIndex < MAX_IMMEDIATE_FLOAT_DATA);
    InsertResource(Resource->ImmediateNumberCache, Resource->ImmediateNumberDataArray, Resource->ImmediateNumberDataIndex, Number, &Number, sizeof(float))
}
#undef InsertResource


inline internal void
Write32(int Data, FILE* File)
{
    if (!IsLittleEndian())
    {
        Data = ChangeEndianness32(Data);
    }
    fwrite(&Data, sizeof(uint8), 4, File);
}

#define WriteString Write32(Struct->Size, File);\
fwrite(Struct->Data, sizeof(uint8), Struct->Size, File);

#define WriteFloat Write32(*(uint32*)(Struct), File);

#define DefineExportFunction(Type, Write)\
internal void ExportResourceAsBinary(int Index, Type* Ptr, FILE* File)\
{\
    fwrite(&Index, sizeof(Index), 1, File);\
    for (uint32 i = 0; i < Index; ++i) {\
        Type* Struct = Ptr++;\
        Write\
    }\
}

DefineExportFunction(string, WriteString)
DefineExportFunction(float, WriteFloat)

#undef WriteString
#undef WriteFloat
#undef DefineExportFunction

// TODO(pipecaniza): export audio files
function void
ExportLocationResource(resource_container* Resource, FILE* File)
{
    fprintf(File, "Key,SourceString,Audio\n");
    
    for (uint32 i = 0; i < Resource->LocalizationDataIndex; ++i)
    {
        const string& String = Resource->LocalizationDataArray[i];
        fprintf(File, "\"%d\",\"%.*s\",\"None\"\n", i, String.Size, String.Data);        
    }
}

//NOTE(pipecaniza): The resources as exported as little-endian
function void
ExportResources(resource_container* Resource, FILE* File)
{
    ExportResourceAsBinary(Resource->NonLocalizationDataIndex, Resource->NonLocalizationDataArray, File);
    ExportResourceAsBinary(Resource->ImmediateStringDataIndex, Resource->ImmediateStringDataArray, File);
    ExportResourceAsBinary(Resource->ImmediateNumberDataIndex, Resource->ImmediateNumberDataArray, File);
}
