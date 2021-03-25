%option noyywrap 
%option yylineno

%top{
	#pragma warning(disable: 4005)

	int yycolumn = 1;

	#define YY_USER_ACTION {						\
		ctx.loc.end.line = ctx.loc.begin.line;		\
		ctx.loc.end.column = ctx.loc.begin.column;	\
		yycolumn = ctx.loc.end.line == yylineno	\
			? yycolumn								\
			: 1;									\
		ctx.loc.begin.line = yylineno;				\
		ctx.loc.begin.column = yycolumn;			\
		yycolumn += yyleng;							}
}

%{
	#include <cstdlib>
	#include <iostream>
	#include "parser.h"
	#include "Context.h"

	char* TrimLabel(char* text)
	{
		++text;
		return text[0] == ' '
			?	++text
			:	text;
	}
%}

digits		[0-9]+
identifier	[$]?[a-zA-Z][a-zA-Z_0-9]*
label		[#]+[ ]?[a-zA-Z][a-zA-Z_0-9]*
text		[a-zA-Z][a-zA-Z ,.?!]*
ignored		[ \n\t]

%%
{label}			{ return yy::parser::make_LABEL				(TrimLabel(yytext), ctx.loc);	}
{identifier}	{ 
					if (std::strcmp(yytext, "$call") == 0)
						return yy::parser::make_CALL		(ctx.loc);	
					return yy::parser::make_IDENTIFIER		(yytext, ctx.loc);				
				}
{digits}		{ return yy::parser::make_NUMCONST			(std::atoi(yytext), ctx.loc);				}
{text}			{ return yy::parser::make_TEXT				(yytext, ctx.loc);				}
"```"			{ return yy::parser::make_SCRIPT_BLOCK		(ctx.loc);						}
"->"			{ return yy::parser::make_ARROW				(ctx.loc);						}
"*"				{ return yy::parser::make_STAR				(ctx.loc);						}
":"				{ return yy::parser::make_COLON				(ctx.loc);						}	
"="				{ return yy::parser::make_EQUAL				(ctx.loc);						}
"|"				{ return yy::parser::make_STICK				(ctx.loc);						}
"$"				{ return yy::parser::make_DOLLAR			(ctx.loc);						}
"@"				{ return yy::parser::make_AT				(ctx.loc);						}
"?"				{ return yy::parser::make_QUESTION			(ctx.loc);						}
"-"				{ return yy::parser::make_HYPHEN			(ctx.loc);						}
"("				{ return yy::parser::make_OPEN_PARENTH		(ctx.loc);						}	
")"				{ return yy::parser::make_CLOSE_PARENTH		(ctx.loc);						}
"["				{ return yy::parser::make_OPEN_BRACKET		(ctx.loc);						}	
"]"				{ return yy::parser::make_CLOSE_BRACKET		(ctx.loc);						}	
"#"				{ return yy::parser::make_HASH				(ctx.loc);						}
">"				{ return yy::parser::make_GREATER			(ctx.loc);						}
"<"				{ return yy::parser::make_LESS				(ctx.loc);						}
"!"				{ return yy::parser::make_EXCLAMATION		(ctx.loc);						}
{ignored}		
<<EOF>>			{ return yy::parser::make_END				(ctx.loc);						}
.				{ std::cout << "Unexpected Token: " << yytext << ", Line: " << yylineno << '\n';				}
