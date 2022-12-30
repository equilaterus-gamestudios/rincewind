#pragma once
//#include "rincewind_globals.h"
#include "rincewind_context.h"
#include "rincewind_resource.h"

/// Generated
#include "./generated/parser.h"
#include "./generated/lex.yy.h"


//TODO(pipecaniza): change the bison to generate c code
struct compiler
{
    context Context;
    yy::lexer Lexer;
    yy::parser Parser;
    //codegenerator pending

    compiler(FILE* File) : Lexer(&Context, File, NULL), Parser(Lexer)
    {
    }
};

// inline compiler
// MakeCompiler(FILE* File)
// {
//     compiler Compiler;
//     return(Compiler);
// }

int main(int argc, char** argv);