%language "c++"

%defines "parser.h"

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
	#pragma warning(disable: 4065)	
	#include "Types.h"
	#include <string>
	
	class Context;
}

%param { Context& ctx }

%{
	#include <iostream>
	#include <cmath>	
	#include "Context.h"

	extern yy::parser::symbol_type yylex(Context& ctx);	
%}

%token			END 0
				IDENTIFIER
				NUMCONST
				TEXT
%token
	ARROW		"->"
	STAR		"*"
	COLON		":"
	EQUAL		"="
	STICK		"|"
	DOLLAR		"$"
	AT			"@"
	QUESTION	"?"
	HYPHEN		"-"
	
%type<std::string>	IDENTIFIER
%type<std::string>	NUMCONST
%type<std::string>	TEXT

%type<SStatement> dialog option command jump label
%type<TStatements> options

%start script

%%
script:			statements
;

statements:		statements statement
|				%empty
;

statement:		dialog							{ ctx.Statements.push_back($1); ctx.CurrentRow += 2; }
|				command							{ ctx.Statements.push_back($1); ++ctx.CurrentRow; }
|				jump							{ ctx.Statements.push_back($1); ++ctx.CurrentRow; }
|				label							{ ctx.AddIdentifier($1.Name, ctx.CurrentRow); }
;

dialog:			"-" TEXT ":" TEXT				{ $$ = SStatement::CreateDialog($2, $4); }
|				"-" TEXT ":" options			{ auto Dialog = SStatement::CreateDialog($2, ""); Dialog.InternalStatement = $4; $$ = Dialog; }
;

options:		options option					{ $1.push_back($2); $$ = $1;}
|				option							{ auto ListStatements = TStatements(); ListStatements.push_back($1); $$ = ListStatements; }
;

option:			"*" TEXT "|" jump				{ auto Option = SStatement::CreateOption($2); Option.AddInternalStatement($4); $$ = Option; ++ctx.CurrentRow; }
|				"*" TEXT						{ $$ = SStatement::CreateOption($2); ++ctx.CurrentRow; }
;

command:		"$" IDENTIFIER "=" NUMCONST		{ $$ = SStatement::CreateCommand($2, $4); }
;

jump:			"->" IDENTIFIER					{ $$ = SStatement::CreateJump($2); }
;

label:			"@" IDENTIFIER					{ $$ = SStatement::CreateLabel($2); }
%%

void yy::parser::error (const std::string& msg)
{
	std::cout << "Syntax Error: " << msg << '\n';	
}