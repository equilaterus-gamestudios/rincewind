%option noyywrap bison
%option yylineno

%option bison-complete
%option bison-cc-namespace=yy
%option bison-cc-parser=parser
%option bison-locations

%option namespace=yy
%option lexer=lexer

%option unicode

%option ctorarg="context* InCtx"

%top{
	#include "parser.h"	
	#include "../rincewind_context.h"
	#include "../rincewind_memory.h"
	#undef internal
	#undef function
}

%class{
public:
	context* Ctx;
}

%init{
	Ctx = InCtx;
}


%{
	#include <cstdlib>
	#include <iostream>	
		
	#pragma warning(disable: 4005)
	#pragma GCC diagnostic ignored "-Wmacro-redefined"

	int yycolumn = 1;

	#define YY_USER_ACTION {									\
		Ctx->Location.end.line = Ctx->Location.begin.line;		\
		Ctx->Location.end.column = Ctx->Location.begin.column;	\
		yycolumn = Ctx->Location.end.line == yylineno			\
			? yycolumn											\
			: 1;												\
		Ctx->Location.begin.line = yylineno;					\
		Ctx->Location.begin.column = yycolumn;					\
		yycolumn += yyleng;							}

//TODO(pipecaniza): re-implement to not modify the char* content
//TODO(pipecaniza): use trim
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
	/*{label}			{ return yy::parser::make_LABEL				(AllocateString(&Ctx->Arena, TrimLeft(TrimLeft(yytext, '#'))), Ctx->Location);		}
{identifier}	{ 
					if (std::strcmp(yytext, "$call") == 0)
						return yy::parser::make_CALL		(Ctx->Location);	
					return yy::parser::make_IDENTIFIER		(AllocateString(&Ctx->Arena, TrimLeft(TrimLeft(yytext, '$'))), Ctx->Location);				
				}
{full_text}		{ return yy::parser::make_TEXT				(AllocateString(&Ctx->Arena, TrimRight(TrimLeft(yytext,'`'), '`')), Ctx->Location);		}*/
%}

digits		[0-9]+
identifier	[$][_]?[a-zA-Z][a-zA-Z_0-9]*
label		[#]+[ ]?[a-zA-Z][a-zA-Z_0-9]*
text		[a-zA-Z0-9ñáéíóú',.¿?¡!{}][a-zA-Z0-9 ñáéíóú',.¿?¡!{}]+
full_text	"`"{text}"`"

ignored		[ \n\t\r]

%%
{label}			{ return yy::parser::make_LABEL				(AllocateString(Ctx->Arena, TrimLeft(TrimLeft(yytext, '#'))), Ctx->Location);		}
{identifier}	{ 
					if (std::strcmp(yytext, "$call") == 0)
						return yy::parser::make_CALL		(Ctx->Location);	
					return yy::parser::make_IDENTIFIER		(AllocateString(Ctx->Arena, TrimLeft(TrimLeft(yytext, '$'))), Ctx->Location);				
				}
{full_text}		{ return yy::parser::make_TEXT				(AllocateString(Ctx->Arena, TrimRight(TrimLeft(yytext,'`'), '`')), Ctx->Location);		}
"```"			{ return yy::parser::make_SCRIPT_BLOCK		(Ctx->Location);											}
"*"				{ return yy::parser::make_STAR				(Ctx->Location);											}
":"				{ return yy::parser::make_COLON				(Ctx->Location);											}	
"="				{ return yy::parser::make_EQUAL				(Ctx->Location);											}
"|"				{ return yy::parser::make_STICK				(Ctx->Location);											}
"$"				{ return yy::parser::make_DOLLAR			(Ctx->Location);											}
"-"				{ return yy::parser::make_HYPHEN			(Ctx->Location);											}
"("				{ return yy::parser::make_OPEN_PARENTH		(Ctx->Location);											}	
")"				{ return yy::parser::make_CLOSE_PARENTH		(Ctx->Location);											}
"["				{ return yy::parser::make_OPEN_BRACKET		(Ctx->Location);											}	
"]"				{ return yy::parser::make_CLOSE_BRACKET		(Ctx->Location);											}	
">"				{ return yy::parser::make_GREATER			(Ctx->Location);											}
"<"				{ return yy::parser::make_LESS				(Ctx->Location);											}
"~"				{ return yy::parser::make_NOT				(Ctx->Location);											}
{digits}		{ return yy::parser::make_NUMCONST			(std::atoi(yytext), Ctx->Location);							}
{text}			{ return yy::parser::make_TEXT				(AllocateString(Ctx->Arena, TrimRight(TrimLeft(yytext))), Ctx->Location);									}
{ignored}		{ }
<<EOF>>			{ return yy::parser::make_END				(Ctx->Location);											}
.				{ std::cout << "Unexpected Token: " << yytext << ", Line: " << yylineno << '\n';						}
%%