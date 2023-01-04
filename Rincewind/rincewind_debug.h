#pragma once
#include <string>

///Project
#include "rincewind_globals.h"
#include "rincewind_statement.h"

static std::string StatementAsString[] = {
    "None",
	"Number",
	"String",
	"Label",
    "DefineLabel",
	"UniqueString", 
	"Identifier",
	"Call",
	"WaitOptionSelection",
	"WaitInteraction",
	"Dialog",
	"DialogWithOptions",
	"Option",
	"Command",
	"Jump",
    "IndirectJump",
	"Condition",
	"Code",
	"Equal",
	"NotEqual",
	"Greater",
	"GreaterOrEqual",
	"Less",
	"LessOrEqual"
};

internal std::string 
GetTabs(int tabs)
{
    std::string result = "";
    for(int i = 0; i < tabs; ++i)
        result += " ";
    return result;
}

function void 
PrintAST(const statement* Statement, int Tab = 0)
{
    printf("%s%s", GetTabs(Tab).c_str(), StatementAsString[(int)Statement->Type].c_str())    ;
    if (IsAtomStatement(Statement))
    {
        if (Statement->Type == statement_type::Number)
        {
            printf(": %f", Statement->NumericValue);
        }
        else 
        {
            //char a[1000];
            //for(int i = 0; i < Statement->StrValue.Size; ++i)
            //{
            //   a[i] = *(Statement->StrValue.Data + i);
            //}
            //a[Statement->StrValue.Size] = '\0';
            printf(": %.*s", Statement->StrValue.Size, Statement->StrValue.Data);
        }
    }
    printf("\n");
    for (auto param: Statement->Parameters)
    {
        PrintAST(&param, Tab + 1);
    }
}