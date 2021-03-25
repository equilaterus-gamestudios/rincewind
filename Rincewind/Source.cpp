#include <fstream>
#include <iostream>
#include "Context.h"
#include "CodeGenerator.h"
#include "parser.h"

extern FILE* yyin;

int main(int argc, char** argv)
{
    std::string filename = "dialogtest.md";
    FILE* a = fopen(filename.c_str(), "r");
    yyin = fopen(filename.c_str(), "r");
    Context ctx = Context();
    
    yy::parser parser(ctx);    
    parser.parse();

    if (ctx.Errors != 0)
    {
        std::cout << "Errors were found, fix the source file and recompile again to generate a target file.\n";

        return -1;
    }
    
    CodeGenerator Generator = CodeGenerator(&ctx);
    
    //ctx.PrintStatements();
    // Generate code
    Generator.GenerateCode();
    Generator.ProcessJumpsSecondPass();

    if (ctx.Errors != 0)
    {
        std::cout << "Errors were found while generating the code. Report this error.\n";

        return -1;
    }

    std::ofstream OutputCode("dialogtest.dialog");
    for (auto& Statement : Generator.RincewindCode)
    {
        OutputCode << Statement.ToString() << '\n';
    }
    OutputCode.close();

    //Generate resources
    std::ofstream OutputResources("dialogtest.csv");

    OutputResources << "Key,SourceString,Comment\n";

    for (auto& Resource : ctx.Resources.Texts)
    {
        OutputResources << "\"" << Resource.first << "\",\"" << Resource.second << "\",\"\"\n";
    }

    std::cout << "Compilation was successfull\n";
    std::cout << "File has been generated\n";

    return 0;
}