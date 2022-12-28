#pragma once
#include <unordered_map>
#include <map>
#include <string>

#define MAX_DATA 5000
#define function static
#define internal static

struct resource
{
    //std::map<std::string, std::string> Titles;

    std::string Data[MAX_DATA];
    std::unordered_map<std::string, int> UniqueData;

    int DataBufferSize;
};

function
int InsertResource(resource* Resource, const std::string& Data)
{
    Resource->Data[Resource->DataBufferSize] = Data;
    return (Resource->DataBufferSize++);
}

function
int InsertUniqueResource(resource* Resource, const std::string& Data)
{
    if (!Resource->UniqueData.count(Data))
    {
        Resource->UniqueData.emplace(Data, Resource->DataBufferSize);
        return (InsertResource(Resource, Data));
    }
    return Resource->UniqueData[Data];
}
