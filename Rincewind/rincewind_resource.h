#pragma once
///Project
#include "rincewind_globals.h"
#include "rincewind_common.h"
///STL
#include <unordered_map>
#include <cstdio>


#define MAX_LOCALIZATION_DATA 5000
#define MAX_NON_LOCALIZATION_DATA 5000
#define MAX_INMEDIATE_STR_DATA 5000
#define MAX_INMEDIATE_FLOAT_DATA 5000

//NOTE(pipecaniza): should contains only data that will be exported
//NOTE(pipecaniza): Should we store everything as unique to avoid data duplication?
//NOTE(pipecaniza): complete buffer of the region, follow by each data, in case of strings, int followed by char[]
struct resource_container
{
    //std::map<std::string, std::string> Titles;    
    
    //NOTE(pipecaniza): Localization data
    string LocalizationData[MAX_LOCALIZATION_DATA];
    string NonLocalizationData[MAX_NON_LOCALIZATION_DATA];
    string InmediateStringData[MAX_INMEDIATE_STR_DATA];
    float InmediateNumberData[MAX_INMEDIATE_STR_DATA];

    int LocalizationDataIndex = 0;
    int NonLocalizationDataIndex = 0;
    int InmediateNumberDataIndex = 0;
    int InmediateStringDataIndex = 0;

    //TODO(pipecaniza): check this, char* aren´t null terminal
    std::unordered_map<char*, int> UniqueLocalizationTable; 
    std::unordered_map<char*, int> UniqueNonLocalizationTable;
    std::unordered_map<char*, int> UniqueInmediateStringTable;
    std::unordered_map<float, int> UniqueInmediateNumberTable;
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
    InsertResource(Resource->UniqueLocalizationTable, Resource->LocalizationData, Resource->LocalizationDataIndex, String, String.Data)
}

function int
InsertNonLocalizationData(resource_container* Resource, const string& String)
{
    InsertResource(Resource->UniqueNonLocalizationTable, Resource->NonLocalizationData, Resource->NonLocalizationDataIndex, String, String.Data)
}

function int
InsertInmediateStringData(resource_container* Resource, const string& String)
{
    InsertResource(Resource->UniqueInmediateStringTable, Resource->InmediateStringData, Resource->InmediateStringDataIndex, String, String.Data)
}

function int 
InsertInmediateNumberData(resource_container* Resource, float Number)
{
    InsertResource(Resource->UniqueInmediateNumberTable, Resource->InmediateNumberData, Resource->InmediateNumberDataIndex, Number, Number)
}

//TODO(pipecaniza): set pointer as const?

#define WriteString fwrite(&Struct->Size, sizeof(Struct->Size), 1, File);\
fwrite(Struct->Data, sizeof(uint8), Struct->Size, File);

#define WriteFloat fwrite(Struct, sizeof(Struct), 1, File);

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

function void
ExportLocationResource(resource_container* Resource, FILE* File)
{
    fprintf(File, "Key,SourceString,Comment\n");
    
    for (int i = 0; i < Resource->LocalizationDataIndex; ++i)
    {
        const string& String = Resource->LocalizationData[i];
        fprintf(File, "%d,%.*s,None\n", i, String.Size, String.Data);        
    }
}

function void
ExportResources(resource_container* Resource, FILE* File)
{
    ExportResourceAsBinary(Resource->NonLocalizationDataIndex, Resource->NonLocalizationData, File);
    ExportResourceAsBinary(Resource->InmediateStringDataIndex, Resource->InmediateStringData, File);
    ExportResourceAsBinary(Resource->InmediateNumberDataIndex, Resource->InmediateNumberData, File);
}

function void
ImportResource(resource_container* Resource, FILE* File)
{
    while(!feof(File)){
        //string* CurrentString = &Resource->LocalizationData[i];
        //CurrentString->Data;
        //fwrite(&CurrentString->Size, sizeof(CurrentString->Size), 1, File);
        //fwrite(&CurrentString->Data, sizeof(uint8), CurrentString->Size, File);
        int size;
        char text[5000];
        fread(&size, sizeof(int), 1, File);
        int s = fread(text, sizeof(uint8), size, File);
        text[size] = '\0';
        printf("size: %d ", size);
        printf("text: %.*s\n", size, text);
    }
    
}