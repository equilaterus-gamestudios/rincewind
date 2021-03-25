#pragma once
#include <unordered_map>
#include <map>
#include <string>

struct FResources
{
    FResources() : Consecutive(0) {}

    std::map<std::string, std::string> Titles;

    std::unordered_map<std::string, std::string> Texts;

    int Consecutive;
    
    std::string InsertResource(std::string Resource, bool bIsTitle = false)
    {
        if (bIsTitle && Titles.count(Resource) != 0)
        {
            return Titles[Resource];
        }

        std::string Id = "LOR" + std::to_string(Consecutive++);
        Texts[Id] = Resource;

        if (bIsTitle)
        {
            Titles[Resource] = Id;
        }

        return Id;
    }
};
