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

#include "generated/parser.cpp"



global_variable char FilePath[500];


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
    char* InputPath = argv[1];
    char* InputFileName = argv[2];
    char* OutputPath = argv[3];
    char* OutputFileName = argv[4];

    strcpy(FilePath, InputPath);
    strcat(FilePath, InputFileName);
    
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




/*
int main(int argc, char** argv)
{
    int a;
    std::cin >>a; 
    if (argc < 3)
    {
        std::cout << "Please, enter the path, input file and the output name\n";
        return -1;
    }

    char* Path = argv[1];
    char* Filename = argv[2];
    char* OutputFileName = argv[3];

    char* InputFile = (char*)malloc(strlen(Path) + strlen(Filename) + 1); InputFile[0] = '\0';
    strcat(InputFile, Path); strcat(InputFile, Filename);

    FILE* File = fopen(InputFile, "r");
    if (File == NULL)
    {
        std::cout << "Please, enter the existing input file\n";
        return -1;
    }

    Context ctx = Context();

    yy::Lexer lexer = yy::Lexer(&ctx, File, NULL);
    lexer.filename = Filename;
    
    yy::parser parser(lexer);
    parser.parse();

    if (ctx.Errors != 0)
    {
        std::cout << "Errors were found, fix the source file and recompile again to generate a target file.\n";

        return -1;
    }
    
    CodeGenerator Generator = CodeGenerator(&ctx);
    
    // ctx.PrintStatements();
    // Generate code
    Generator.GenerateCode();
    Generator.ProcessJumpsSecondPass();

    if (ctx.Errors != 0)
    {
        std::cout << "Errors were found while generating the code.\n";

        return -1;
    }

    const char* CodeExtention = ".dialog";
    const char* ResourcesExtention = ".csv";

    char* OutputCodeFile = (char*)malloc(strlen(Path) + strlen(OutputFileName) + strlen(CodeExtention) + 1); OutputCodeFile[0] = '\0';
    char* ResourcesFile = (char*)malloc(strlen(Path) + strlen(OutputFileName) + strlen(ResourcesExtention) + 1); ResourcesFile[0] = '\0';

    strcat(OutputCodeFile, Path); strcat(OutputCodeFile, OutputFileName); strcat(OutputCodeFile, CodeExtention);
    strcat(ResourcesFile, Path); strcat(ResourcesFile, OutputFileName); strcat(ResourcesFile, ResourcesExtention);

    std::ofstream OutputCode(OutputCodeFile);
    for (auto& Statement : Generator.RincewindCode)
    {
        OutputCode << Statement.ToString() << '\n';
    }
    OutputCode.close();

    //Generate resources
    std::ofstream OutputResources(ResourcesFile);

    OutputResources << "Key,SourceString,Comment\n";

    for (auto& Resource : ctx.Resources.Texts)
    {
        OutputResources << "\"" << Resource.first << "\",\"" << Resource.second << "\",\"\"\n";
    }

    std::cout << "Compilation was successful\n";
    std::cout << "Output has been generated\n";
    
    return 0;
}*/