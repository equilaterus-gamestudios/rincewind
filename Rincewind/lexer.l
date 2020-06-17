%option noyywrap 

%top{
	#pragma warning(disable: 4005)
}

%{
	#include <cstdlib>
	#include <iostream>
	#include "parser.h"
	#include "Context.h"
%}

digits		[0-9]+
identifier	_[a-zA-Z][a-zA-Z_0-9]*
text		[a-zA-Z][a-zA-Z ,.?!]*
ignored		[ \n\t]

%%
{identifier}	{ return yy::parser::make_IDENTIFIER	(yytext);		}
{digits}		{ return yy::parser::make_NUMCONST		(yytext);		}
{text}			{ return yy::parser::make_TEXT			(yytext);		}
"->"			{ return yy::parser::make_ARROW			();				}
"*"				{ return yy::parser::make_STAR			();				}
":"				{ return yy::parser::make_COLON			();				}	
"="				{ return yy::parser::make_EQUAL			();				}
"|"				{ return yy::parser::make_STICK			();				}
"$"				{ return yy::parser::make_DOLLAR		();				}
"@"				{ return yy::parser::make_AT			();				}
"?"				{ return yy::parser::make_QUESTION		();				}
"-"				{ return yy::parser::make_HYPHEN		();				}
{ignored}		{ return yylex(ctx);									}
<<EOF>>			{ return yy::parser::make_END			();				}
.				{ std::cout << yytext << '\n';							}
