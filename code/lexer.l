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
	#include <stdlib.h>
		
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

	#define PrintDebugToken\	
	printf("%s\n", yytext);
	#if defined(DEBUG) && defined(VERBOSE)
		#define PDT PrintDebugToken
	#else
		#define PDT
	#endif
	/*
	full_text	"`"{text}"`"

*/
%}

number		[0-9]+([.]?[0-9]+)
identifier	[$][_]?[a-zA-Z][0-9a-zA-Z_]*
label		[#]+[ ]*[0-9a-zA-Z]+
text		[0-9a-zA-Z_ñáéíóú',.¿?¡!]([0-9a-zA-Z_ñáéíóú',.¿?¡! ]*[0-9a-zA-Z_ñáéíóú',.¿?¡!])?
full_text	\{{text}\}
ignored		[ \n\t\r]

%%
{full_text}		{ PDT return yy::parser::make_TEXT				(AllocateString(Ctx->Arena, TrimRight(TrimLeft(yytext,'{'), '}')), Ctx->Location);		}
{label}			{ PDT return yy::parser::make_LABEL				(AllocateString(Ctx->Arena, TrimLeft(TrimLeft(yytext, '#'))), Ctx->Location);		}
{identifier}	{ PDT
					if (std::strcmp(yytext, "$call") == 0)
						return yy::parser::make_CALL		(Ctx->Location);	
					return yy::parser::make_IDENTIFIER		(AllocateString(Ctx->Arena, TrimLeft(TrimLeft(yytext, '$'))), Ctx->Location);				
				}
"```"			{ PDT return yy::parser::make_SCRIPT_BLOCK		(Ctx->Location);											}
"&&"			{ PDT return yy::parser::make_AND				(Ctx->Location);											}
"||"			{ PDT return yy::parser::make_OR				(Ctx->Location);											}
"*"				{ PDT return yy::parser::make_STAR				(Ctx->Location);											}
":"				{ PDT return yy::parser::make_COLON				(Ctx->Location);											}	
"="				{ PDT return yy::parser::make_EQUAL				(Ctx->Location);											}
"|"				{ PDT return yy::parser::make_STICK				(Ctx->Location);											}
"$"				{ PDT return yy::parser::make_DOLLAR			(Ctx->Location);											}
"-"				{ PDT return yy::parser::make_HYPHEN			(Ctx->Location);											}
"("				{ PDT return yy::parser::make_OPEN_PARENTH		(Ctx->Location);											}	
")"				{ PDT return yy::parser::make_CLOSE_PARENTH		(Ctx->Location);											}
"["				{ PDT return yy::parser::make_OPEN_BRACKET		(Ctx->Location);											}	
"]"				{ PDT return yy::parser::make_CLOSE_BRACKET		(Ctx->Location);											}	
">"				{ PDT return yy::parser::make_GREATER			(Ctx->Location);											}
"<"				{ PDT return yy::parser::make_LESS				(Ctx->Location);											}
"~"				{ PDT return yy::parser::make_NOT				(Ctx->Location);											}
"`"				{ PDT return yy::parser::make_HIGHLIGHT			(Ctx->Location);											}
{number}		{ PDT return yy::parser::make_NUMCONST			(strtof(yytext, 0), Ctx->Location);							}
{text}			{ PDT return yy::parser::make_TEXT				(AllocateString(Ctx->Arena, TrimRight(TrimLeft(yytext))), Ctx->Location);									}
{ignored}		{ }
<<EOF>>			{ PDT return yy::parser::make_END				(Ctx->Location);											}
.				{ printf("Unexpected token: %s in line: %d.\n", yytext, yylineno);											}
%%