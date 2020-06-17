#include "Context.h"
#include <iostream>


Context::Context()
    :TraceParsing(false), TraceScanning(false), CurrentRow(0)
{
    Identifiers["_end"] = -1;
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
