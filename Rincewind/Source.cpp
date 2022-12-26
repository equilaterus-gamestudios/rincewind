#pragma once
#include <fstream>
#include <iostream>
#include "Context.h"
#include "CodeGenerator.h"
#include "generated/lex.yy.h"

int main(int argc, char** argv)
{
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
}