#include "Context.h"
#include <iostream>




Context::Context()
    :TraceParsing(false), TraceScanning(false), Consecutive(0), Errors(0)
{
    Identifiers["start"] = 0;
    Identifiers["end"] = -1;
}


void Context::Print()
{
    for (auto& Line : Statements)
    {
        std::cout << Line.Name << '\n';
    }
}

void Context::AddIdentifier(std::string& Identifier, int& Position)
{
    if (Identifiers.count(Identifier) > 0)
    {
        std::cout << Identifier << " is already defined\n";
        return;
    }

    Identifiers[Identifier] = Position;
}

void Context::InsertStatements(std::vector<FStatement> InStatements)
{
    Statements.reserve(Statements.size() + InStatements.size()); // preallocate memory    
    Statements.insert(Statements.end(), InStatements.begin(), InStatements.end());    
}

void Context::PrintStatements()
{
    PrintStatements(Statements);
}

void Context::PrintStatements(std::vector<FStatement>& Statements)
{
    for (auto& Statement : Statements)
    {
        std::cout << Statement.Type << " " << Statement.Name << " ---> ";
        for (auto& Param : Statement.Parameters)
        {
            std::cout << Param.first << ": " << Param.second << ", ";
        }
        std::cout << std::endl;
        if (Statement.InternalStatement.size() > 0)
        {
            PrintStatements(Statement.InternalStatement);
        }
    }
}