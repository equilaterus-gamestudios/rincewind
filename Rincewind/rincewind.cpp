/// Project
//#include "rincewind.h"
#include <string.h>
#include <stdio.h>

#include "rincewind_globals.h"
#include "rincewind_memory.h"
#include "rincewind_structs.h"
#include "rincewind_resource.h"
#include "rincewind_context.h"
#include "rincewind_code_gen.h"
#ifdef DEBUG
#include "rincewind_debug.h"
#endif

#ifndef PROFILING
#include "generated/lex.yy.cpp"
#include "generated/parser.cpp"
#endif


struct compiler
{
    arena Arena;
    context Context;
    code_gen CodeGen;
    resource_container Resource;
#ifndef PROFILING
    yy::lexer Lexer;
    yy::parser Parser;
#endif

    compiler(FILE* File) 
#ifndef PROFILING
    : Lexer(&Context, File, NULL), Parser(Lexer)
#endif
    {
        Arena = MakeArena();
        Context.Arena = &Arena;
        Resource = MakeResourceContainer(&Arena);
        CodeGen = MakeCodeGen(&Context, &Resource, &Arena);
    }
};


int main(int argc, char** argv)
{
    char* InputFileLocation = argv[1];
    char* OutputPath = argv[2];
    char* OutputFileName = argv[3];

    char FilePath[500];

    strcpy(FilePath, InputFileLocation);
    
    FILE* InputFile = fopen(FilePath, "r");
    if (InputFile == NULL)
    {
        printf("File not found: %s.\n", FilePath);
        return -1;
    }

    compiler Compiler(InputFile);
#ifndef PROFILING
    Compiler.Parser.parse();
    #ifdef DEBUG
        PrintAST(&Compiler.Context.AbstractTree);
    #endif
#else
    Compiler.Context.AbstractTree = MakeAST();
#endif

    fclose(InputFile);

    if (Compiler.Context.ParsingErrors != 0)
    {
        printf("Errors were found while parsing the source code.\n");
        return -1;
    }

    GenerateCode(&Compiler.CodeGen);
    GenerateMachineCode(&Compiler.CodeGen);

    if (Compiler.Context.GeneratingErrors != 0)
    {
        printf("Errors were found while generating the code.\n");
        return -1;
    }

    strcpy(FilePath, OutputPath); strcat(FilePath, OutputFileName); strcat(FilePath, ".dialog");
    FILE* FileCode = fopen(FilePath, "wb");
    strcpy(FilePath, OutputPath); strcat(FilePath, OutputFileName); strcat(FilePath, ".csv");
    FILE* FileResources = fopen(FilePath, "w");

    ExportResources(&Compiler.Resource, FileCode);
    ExportCode(&Compiler.CodeGen, FileCode);
    ExportLocationResource(&Compiler.Resource, FileResources);

    fclose(FileCode);
    fclose(FileResources);

    return 0;
}
