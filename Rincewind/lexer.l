%option noyywrap bison
%option yylineno

%option bison-complete
%option bison-cc-namespace=yy
%option bison-cc-parser=parser
%option bison-locations

%option namespace=yy
%option lexer=Lexer

%option unicode

%option ctorarg="Context* InCtx"

%top{
	#include "parser.h"
	#include "Context.h"	
}

%class{
public:
	Context* Ctx;
}

%init{
	Ctx = InCtx;
}


%{
	#include <cstdlib>
	#include <iostream>
		
	#pragma warning(disable: 4005)

	int yycolumn = 1;

	#define YY_USER_ACTION {						\
		Ctx->loc.end.line = Ctx->loc.begin.line;		\
		Ctx->loc.end.column = Ctx->loc.begin.column;	\
		yycolumn = Ctx->loc.end.line == yylineno	\
			? yycolumn								\
			: 1;									\
		Ctx->loc.begin.line = yylineno;				\
		Ctx->loc.begin.column = yycolumn;			\
		yycolumn += yyleng;							}


	char* TrimLeft(char* text, char value = ' ')
	{
		while (text[0] == value)
		{
			++text;
		}
		return text;
	}

	char* TrimRight(char* text, char value = ' ')
	{
		int i = strlen(text) - 1;
		while (i >= 0 && value == text[i])
		{
			text[i] = '\0';
			i--;
		}
		return text;
	}
	
%}

digits		[0-9]+
identifier	[$][_]?[a-zA-Z][a-zA-Z_0-9]*
label		[#]+[ ]?[a-zA-Z][a-zA-Z_0-9]*
full_text	"`"[a-zA-Z0-9 ',.?!$%#*]+"`"
text		[a-zA-Zñáéíóú',.¿?!][a-zA-Z ñáéíóú',.?!]+
ignored		[ \n\t\r]

%%
{label}			{ return yy::parser::make_LABEL				(TrimLeft(TrimLeft(yytext, '#')), Ctx->loc);		}
{identifier}	{ 
					if (std::strcmp(yytext, "$call") == 0)
						return yy::parser::make_CALL		(Ctx->loc);	
					return yy::parser::make_IDENTIFIER		(TrimLeft(TrimLeft(yytext, '$')), Ctx->loc);				
				}
{digits}		{ return yy::parser::make_NUMCONST			(std::atoi(yytext), Ctx->loc);						}
{full_text}		{ return yy::parser::make_TEXT				(TrimRight(TrimLeft(yytext,'`'), '`'), Ctx->loc);	}
"```"			{ return yy::parser::make_SCRIPT_BLOCK		(Ctx->loc);											}
"*"				{ return yy::parser::make_STAR				(Ctx->loc);											}
":"				{ return yy::parser::make_COLON				(Ctx->loc);											}	
"="				{ return yy::parser::make_EQUAL				(Ctx->loc);											}
"|"				{ return yy::parser::make_STICK				(Ctx->loc);											}
"$"				{ return yy::parser::make_DOLLAR			(Ctx->loc);											}
"-"				{ return yy::parser::make_HYPHEN			(Ctx->loc);											}
"("				{ return yy::parser::make_OPEN_PARENTH		(Ctx->loc);											}	
")"				{ return yy::parser::make_CLOSE_PARENTH		(Ctx->loc);											}
"["				{ return yy::parser::make_OPEN_BRACKET		(Ctx->loc);											}	
"]"				{ return yy::parser::make_CLOSE_BRACKET		(Ctx->loc);											}	
">"				{ return yy::parser::make_GREATER			(Ctx->loc);											}
"<"				{ return yy::parser::make_LESS				(Ctx->loc);											}
"~"				{ return yy::parser::make_NOT				(Ctx->loc);											}
{text}			{ return yy::parser::make_TEXT				(yytext, Ctx->loc);									}
{ignored}		{}
<<EOF>>			{ return yy::parser::make_END				(Ctx->loc);											}
.				{ std::cout << "Unexpected Token: " << yytext << ", Line: " << yylineno << '\n';				}
%%