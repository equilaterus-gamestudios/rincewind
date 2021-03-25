#pragma once
#include <string>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include "parser.h"
#include "location.hh"
#include "Types.h"
#include "Resources.h"

#define YY_DECL \
  yy::parser::symbol_type yylex (Context& ctx)
// ... and declare it for the parser's sake.
YY_DECL;


class Context
{

    void PrintStatements(std::vector<FStatement>& Statements);
public:
    Context();
        
	std::vector<FStatement> Statements;

    std::map<std::string, int> Identifiers;

    FResources Resources;

    void Print();

    void AddIdentifier(std::string& Identifier, int& Position);

    // The name of the file being parsed.
    std::string File;
    // Whether to generate parser debug traces.
    bool TraceParsing;
    // Whether to generate scanner debug traces.
    bool TraceScanning;

    yy::location loc;

    int Consecutive;

    int Errors;

    void InsertStatements(std::vector<FStatement> InStatements);

    void PrintStatements();
    
};
