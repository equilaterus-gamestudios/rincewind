#pragma once
#include "rincewind_memory.h"

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
    arena* Arena;
    uint ParsingErrors = 0;
    uint GeneratingErrors = 0;
};
