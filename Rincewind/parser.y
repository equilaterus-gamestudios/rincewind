%language "c++"

%defines "parser.h"

%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.error verbose
%locations

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

%}

%token			END 0
				LABEL
				IDENTIFIER
				NUMCONST
				TEXT
%token
	ARROW			"->"
	STAR			"*"
	COLON			":"
	EQUAL			"="
	STICK			"|"
	DOLLAR			"$"
	AT				"@"
	QUESTION		"?"
	HYPHEN			"-"
	OPEN_PARENTH	"("
	CLOSE_PARENTH	")"
	OPEN_BRACKET	"["
	CLOSE_BRACKET	"]"
	HASH			"#"

%type<std::string>	LABEL	
%type<std::string>	IDENTIFIER
%type<std::string>	NUMCONST
%type<std::string>	TEXT

%type<SStatement> dialog option command jump
%type<TStatements> options

%start script

%%
script:			statements
;

statements:		statements statement
|				%empty
;

statement:		dialog							{ ctx.Statements.push_back($1); ctx.CurrentRow += 2;	}
|				command							{ ctx.Statements.push_back($1); ++ctx.CurrentRow;		}
|				jump							{ ctx.Statements.push_back($1); ++ctx.CurrentRow;		}
|				LABEL							{ ctx.AddIdentifier($1, ctx.CurrentRow);				}
|				error
;

dialog:			"-" TEXT ":" TEXT				{ $$ = SStatement::CreateDialog($2, $4); }
|				"-" TEXT ":" jump				{ auto Dialog = SStatement::CreateDialog($2, ""); Dialog.AddInternalStatement($4); $$ = Dialog;  }
|				"-" TEXT ":" options			{ auto Dialog = SStatement::CreateDialog($2, ""); Dialog.InternalStatement = $4; $$ = Dialog; }
;

options:		options option					{ $1.push_back($2); $$ = $1;}
|				option							{ auto ListStatements = TStatements(); ListStatements.push_back($1); $$ = ListStatements; }
;

option:			"*" jump						{ auto Option = SStatement::CreateOption(""); Option.AddInternalStatement($2); $$ = Option; ++ctx.CurrentRow; }
|				"*" TEXT						{ $$ = SStatement::CreateOption($2); ++ctx.CurrentRow; }
;

command:		"$" IDENTIFIER "=" NUMCONST		{ $$ = SStatement::CreateCommand($2, $4); }
;

jump:			"[" TEXT "]" "(" LABEL ")"		{ $$ = SStatement::CreateJump($2, $5); }
;
%%

void yy::parser::error (const location_type& loc, const std::string& msg)
{
	ctx.Errors++;
	std::cout << "Syntax Error: (" << loc.begin.line << ", " << loc.begin.column << ") - " << msg << '\n';	
}