#pragma once
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "parser.h"
#include "location.hh"
#include "Types.h"

#define YY_DECL \
  yy::parser::symbol_type yylex (Context& ctx)
// ... and declare it for the parser's sake.
YY_DECL;

class Context
{
public:
    Context();

    int CurrentRow;

	std::vector<SStatement> Statements;

    std::map<std::string, int> Identifiers;

    void Print();

    void AddIdentifier(std::string& Identifier, int& Position);

    // The name of the file being parsed.
    std::string File;
    // Whether to generate parser debug traces.
    bool TraceParsing;
    // Whether to generate scanner debug traces.
    bool TraceScanning;

    yy::location loc;

    int Errors;
};
