#pragma once
///Project
#include "rincewind_globals.h"
#include "rincewind_statement.h"
#include <cstdio>

static std::string StatementAsString[] = {
    "None",
	"Number",
	"String",
	"Label",
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
            printf(": %d", Statement->IntValue);
        }
        else 
        {
            printf(": %s", Statement->StrValue.c_str());
        }
    }
    printf("\n");
    for (auto param: Statement->Parameters)
    {
        PrintAST(&param, Tab + 1);
    }
}