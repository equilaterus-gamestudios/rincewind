#pragma once
///Project
#include "rincewind_globals.h"
#include "rincewind_common.h"
///STL
#include <cassert>
#include <unordered_map>
#include <cstdio>


#define MAX_LOCALIZATION_DATA 5000
#define MAX_NON_LOCALIZATION_DATA 5000
#define MAX_IMMEDIATE_STR_DATA 5000
#define MAX_IMMEDIATE_FLOAT_DATA 5000

//NOTE(pipecaniza): should contains only data that will be exported
//NOTE(pipecaniza): Should we store everything as unique to avoid data duplication?
//NOTE(pipecaniza): complete buffer of the region, follow by each data, in case of strings, int followed by char[]
struct resource_container
{
    string LocalizationData[MAX_LOCALIZATION_DATA];
    string NonLocalizationData[MAX_NON_LOCALIZATION_DATA];
    string ImmediateStringData[MAX_IMMEDIATE_STR_DATA];
    float ImmediateNumberData[MAX_IMMEDIATE_STR_DATA];

    int LocalizationDataIndex = 0;
    int NonLocalizationDataIndex = 0;
    int ImmediateNumberDataIndex = 0;
    int ImmediateStringDataIndex = 0;

    //TODO(pipecaniza): check this, char* aren´t null terminal
    std::unordered_map<char*, int> UniqueLocalizationTable; 
    std::unordered_map<char*, int> UniqueNonLocalizationTable;
    std::unordered_map<char*, int> UniqueImmediateStringTable;
    std::unordered_map<float, int> UniqueImmediateNumberTable;
};

#define InsertResource(Cache, Table, Index, Data, DataCache) \
if (!Cache.count(DataCache))\
{\
    Cache.emplace(DataCache, Index);\
    Table[Index] = Data;\
    return (Index++);\
}\
return Cache[DataCache];


function int
InsertLocalization(resource_container* Resource, const string& String)
{
    assert(Resource->LocalizationDataIndex < MAX_LOCALIZATION_DATA);
    InsertResource(Resource->UniqueLocalizationTable, Resource->LocalizationData, Resource->LocalizationDataIndex, String, String.Data)
}

function int
InsertNonLocalizationData(resource_container* Resource, const string& String)
{
    assert(Resource->NonLocalizationDataIndex < MAX_NON_LOCALIZATION_DATA);
    InsertResource(Resource->UniqueNonLocalizationTable, Resource->NonLocalizationData, Resource->NonLocalizationDataIndex, String, String.Data)
}

function int
InsertImmediateStringData(resource_container* Resource, const string& String)
{
    assert(Resource->ImmediateStringDataIndex < MAX_IMMEDIATE_STR_DATA);
    InsertResource(Resource->UniqueImmediateStringTable, Resource->ImmediateStringData, Resource->ImmediateStringDataIndex, String, String.Data)
}

function int 
InsertImmediateNumberData(resource_container* Resource, float Number)
{
    assert(Resource->ImmediateNumberDataIndex < MAX_IMMEDIATE_FLOAT_DATA);
    InsertResource(Resource->UniqueImmediateNumberTable, Resource->ImmediateNumberData, Resource->ImmediateNumberDataIndex, Number, Number)
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

#define WriteFloat Write32(*reinterpret_cast<int*>(Struct), File);

#define DefineExportFunction(Type, Write)\
internal void ExportResourceAsBinary(int Index, Type* Ptr, FILE* File)\
{\
    fwrite(&Index, sizeof(Index), 1, File);\
    for (int i = 0; i < Index; ++i) {\
        Type* Struct = Ptr++;\
        Write\
    }\
}

DefineExportFunction(string, WriteString)
DefineExportFunction(float, WriteFloat)

#undef WriteString
#undef WriteFloat
#undef DefineExportFunction


function void
ExportLocationResource(resource_container* Resource, FILE* File)
{
    fprintf(File, "Key,SourceString,Comment\n");
    
    for (int i = 0; i < Resource->LocalizationDataIndex; ++i)
    {
        const string& String = Resource->LocalizationData[i];
        fprintf(File, "\"%d\",\"%.*s\",\"None\"\n", i, String.Size, String.Data);        
    }
}

function void
ExportResources(resource_container* Resource, FILE* File)
{
    ExportResourceAsBinary(Resource->NonLocalizationDataIndex, Resource->NonLocalizationData, File);
    ExportResourceAsBinary(Resource->ImmediateStringDataIndex, Resource->ImmediateStringData, File);
    ExportResourceAsBinary(Resource->ImmediateNumberDataIndex, Resource->ImmediateNumberData, File);
}
