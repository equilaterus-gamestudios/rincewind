/// Project
//#include "rincewind.h"
#include <cstring>
#include <stdio.h>

#include "rincewind_globals.h"
#include "rincewind_memory.h"
#include "rincewind_resource.h"
#include "rincewind_context.h"
#include "rincewind_code_gen.h"
#ifdef DEBUG
#include "rincewind_debug.h"
#endif

#include "generated/lex.yy.cpp"
#include "generated/parser.cpp"


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
    Compiler.Parser.parse();
    fclose(InputFile);
#ifdef DEBUG
    PrintAST(&Compiler.Context.AbstractTree);
#endif

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
