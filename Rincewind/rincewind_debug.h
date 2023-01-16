#pragma once
#include <stdio.h>

///Project
#include "rincewind_common.h"
#include "rincewind_globals.h"
#include "rincewind_statement.h"

global_variable string StatementAsString[] = {
    BundleStringC("None"),
	BundleStringC("Number"),
	BundleStringC("Label"),
	BundleStringC("DefineLabel"),
	BundleStringC("LocalizationString"),
	BundleStringC("NonLocalizationString"),
	BundleStringC("Identifier"),
	BundleStringC("Call"),
	BundleStringC("WaitOptionSelection"),
	BundleStringC("WaitInteraction"),
	BundleStringC("Dialog"),
	BundleStringC("DialogWithOptions"),
	BundleStringC("Option"),
	BundleStringC("Command"),
	BundleStringC("Jump"),
	BundleStringC("IndirectJump"),
	BundleStringC("Condition"),
	BundleStringC("Code"),
	BundleStringC("Equal"),
	BundleStringC("NotEqual"),
	BundleStringC("Greater"),
	BundleStringC("GreaterOrEqual"),
	BundleStringC("Less"),
	BundleStringC("LessOrEqual"),
};

global_variable char StrTabs[30];
internal string
GetTabs(int Tabs)
{
    int Index = 0;
    for(; Index < Tabs; ++Index)
    {
        assert(Index < 30);
        StrTabs[Index] = ' ';
    }
    
    return MakeString(StrTabs, Index);;
}

function void 
PrintAST(const statement* Statement, int Tab = 0)
{    
    string StrTabResult = GetTabs(Tab);
    string StrStatement = StatementAsString[(int)Statement->Type];
    printf("%.*s%.*s", StrTabResult.Size, StrTabResult.Data, StrStatement.Size, StrStatement.Data);
    if (IsAtomStatement(Statement))
    {
        if (Statement->Type == statement_type::Number)
        {
            printf(": %f", Statement->NumericValue);
        }
        else 
        {
            printf(": %.*s", Statement->StrValue.Size, Statement->StrValue.Data);
        }
    }
    printf("\n");
    for (auto& param: Statement->Parameters)
    {
        PrintAST(&param, Tab + 1);
    }
}
