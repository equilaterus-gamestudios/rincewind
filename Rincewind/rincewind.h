#include "./generated/lex.yy.h"

#include "rincewind_globals.h"
#include "rincewind_memory.h"
#include "rincewind_resource.h"
#include "rincewind_context.h"
#include "rincewind_code_gen.h"

#ifdef DEBUG
#include "rincewind_debug.h"
#endif

//TODO(pipecaniza): change the bison to generate c code
struct compiler
{
    arena Arena;
    context Context;
    resource_container Resource;
    yy::lexer Lexer;
    yy::parser Parser;
    code_gen CodeGen;

    compiler(FILE* File) : Lexer(&Context, File, NULL), Parser(Lexer)
    {
        Context.Arena = &Arena;
        CodeGen.Context = &Context;
        CodeGen.Resource = &Resource;
    }
};

int main(int argc, char** argv);