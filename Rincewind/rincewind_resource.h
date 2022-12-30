#pragma once
///Project
#include "rincewind_globals.h"

///STL
#include <unordered_map>
#include <map>
#include <string>

#define MAX_DATA 5000

struct resource_container
{
    //std::map<std::string, std::string> Titles;

    std::string Data[MAX_DATA];
    std::unordered_map<std::string, int> UniqueData;

    int DataBufferSize;
};

function int
InsertResource(resource_container* Resource, const std::string& Data)
{
    Resource->Data[Resource->DataBufferSize] = Data;
    return (Resource->DataBufferSize++);
}

function int 
InsertUniqueResource(resource_container* Resource, const std::string& Data)
{
    if (!Resource->UniqueData.count(Data))
    {
        Resource->UniqueData.emplace(Data, Resource->DataBufferSize);
        return (InsertResource(Resource, Data));
    }
    return Resource->UniqueData[Data];
}
