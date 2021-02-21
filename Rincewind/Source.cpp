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
    Generator.ProcessJumpStatements();
    std::string CodeGenerated = Generator.GenerateCode();

    std::ofstream o("dialogtest.dialog");
    o << CodeGenerated;

    std::cout << "Compilation was successfull\n";
    std::cout << "File has been generated\n";

    return 0;
}