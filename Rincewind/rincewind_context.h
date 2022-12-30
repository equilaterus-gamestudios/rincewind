#pragma once
/// Project
#include "rincewind_statement.h"
#include "rincewind_globals.h"
#include "rincewind_resource.h"

/// Generated
#include "generated/parser.h"
#include "generated/location.hh"


#define YY_DECL \
  yy::parser::symbol_type yylex (context& ctx)
// ... and declare it for the parser's sake.
YY_DECL;


struct context
{
    statement AbstractTree;
    yy::location Location;
    int Errors = 0;
};




/*
class Context
{

    //void PrintStatements(std::vector<FStatement>& Statements);
public:
    Context();
        
	statement AbstractTree;

    std::map<std::string, int> Identifiers;

    //FResources Resources;

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

    //void InsertStatements(std::vector<FStatement> InStatements);

    void PrintStatements();
    
};*/
