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
	CALL			"call"
	SCRIPT_BLOCK	"```"
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
	GREATER			">"
	LESS			"<"
	EXCLAMATION		"!"

%type<std::string>	LABEL	
%type<std::string>	IDENTIFIER
%type<int>	NUMCONST
%type<std::string>	TEXT
%type<EConditionalType> condition_sign

%type<FStatement>  script_statement condition jump option
%type<TStatements> options script script_body dialog 

%start screenplay

%%
screenplay:		LABEL statements LABEL
;

statements:		statements statement
|				%empty
;

statement:		dialog							{ ctx.InsertStatements($1);										}
|				jump							{ ctx.Statements.push_back($1);									}
|				script							{ ctx.InsertStatements($1);										}
|				LABEL							{ ctx.Statements.push_back(FStatement::CreateLabel($1));		}
|				error
;

dialog:			"-" TEXT ":" TEXT				{ $$ = TStatements{ FStatement::CreateDialog(ctx.Resources, $2, $4) };			}
|				"-" TEXT ":" jump				{ $$ = TStatements{ FStatement::CreateDialogFromJump(ctx.Resources, $2, $4)};	}
|				"-" TEXT ":" options			{ 
													auto Dialog = FStatement::CreateDialog(ctx.Resources, $2, "", true); 
													FStatement::AtBegining(Dialog, $4); 
													auto Wait = FStatement::CreateWaitOptionSelection(); 
													FStatement::AtEnd(Wait, $4); 
													$$ = $4;		
												}
;

options:		options option					{ FStatement::AtEnd($2, $1); $$ = $1; }
|				option							{ $$ = TStatements{$1};	}
;

option:			"*" jump						{ $$ = FStatement::CreateOptionFromJump(ctx.Resources, $2);	}
|				"*" TEXT						{ $$ = FStatement::CreateOption(ctx.Resources, $2);			}
;


jump:			"[" TEXT "]" "(" LABEL ")"				{ $$ = FStatement::CreateJump($2, $5); }
|				"[" TEXT "]" "(" LABEL ")" condition	{ auto Jump = FStatement::CreateJump($2, $5); $7.AddInternalStatement(Jump); $$ = $7; }
;

script:			"```" script_body "```"			{ $$ = $2; }
;

script_body:	script_body script_statement	{ $1.push_back($2); $$ = $1; }
|				script_statement				{ auto Script = TStatements(); Script.push_back($1); $$ = Script; }	
;

script_statement:	CALL IDENTIFIER				{ $$ = FStatement::CreateCall($2); }
;

condition:		"|" IDENTIFIER									{ $$ = FStatement::CreateCondition(EConditionalType::ECTEqual, $2, 1); }
|				"|" "!" IDENTIFIER								{ $$ = FStatement::CreateCondition(EConditionalType::ECTEqual, $3, 0); }
|				"|" IDENTIFIER condition_sign NUMCONST			{ $$ = FStatement::CreateCondition($3, $2, $4); }
|				"|" IDENTIFIER condition_sign IDENTIFIER		{ $$ = FStatement::CreateCondition($3, $2, $4); }
;

condition_sign:	">"								{ $$ = EConditionalType::ECTGreater; }
|				"<"								{ $$ = EConditionalType::ECTLess; }
|				">" "="							{ $$ = EConditionalType::ECTGreaterOrEqual; }
|				"<" "="							{ $$ = EConditionalType::ECTLessOrEqual; }
|				"="								{ $$ = EConditionalType::ECTEqual; }
|				"!" "="							{ $$ = EConditionalType::ECTNotEqual; }
;
%%

void yy::parser::error (const location_type& loc, const std::string& msg)
{
	ctx.Errors++;
	std::cout << "Syntax Error: (" << loc.begin.line << ", " << loc.begin.column << ") - " << msg << '\n';	
}
