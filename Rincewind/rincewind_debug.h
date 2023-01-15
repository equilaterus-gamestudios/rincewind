#pragma once
#include <stdio.h>
#include <string>

///Project
#include "rincewind_globals.h"
#include "rincewind_statement.h"

static std::string StatementAsString[] = {
    "None",
	"Number",
	"Label",
	"DefineLabel",
	"LocalizationString",
	"NonLocalizationString",
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
	"LessOrEqual",
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
            printf(": %.*s", Statement->StrValue.Size, Statement->StrValue.Data);
        }
    }
    printf("\n");
    for (auto& param: Statement->Parameters)
    {
        PrintAST(&param, Tab + 1);
    }
}



/*
function void
PrintGeneratedCode(FILE* File)
{
    unsigned char a = fgetc(File);
    unsigned char b = fgetc(File);
    unsigned char c = fgetc(File);
    unsigned char d = fgetc(File);
    printf("%d\n", a);
    printf("%d\n", b);
    printf("%d\n", c);
    printf("%d\n", d);

    int i1 = (d<<24) | (c<<16) | (b<<8) | (a);
    int i2 = (a<<24) | (b<<16) | (c<<8) | (d);
    printf("%d\n", i1);
    printf("mix: %d\n", ChangeEndianness32(((a<<24) | (b<<16) | (c<<8) | (d))));

    int t;
    uint8* q = (uint8*)(&t);
    *q = a; q++;
    *q = b; q++;
    *q = c; q++;
    *q = d;
    printf("%d\n", t);
}*/