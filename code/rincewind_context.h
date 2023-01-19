#pragma once
/// Generated
#ifndef PROFILING
#include "generated/parser.h"
#include "generated/location.hh"
#endif


#ifndef PROFILING
#define YY_DECL \
  yy::parser::symbol_type yylex (context& ctx)
// ... and declare it for the parser's sake.
YY_DECL;
#endif

struct context
{
    statement AbstractTree;
#ifndef PROFILING
    yy::location Location;
#endif
    arena* Arena;
    uint16 ParsingErrors = 0;
    uint16 GeneratingErrors = 0;
};

#ifdef PROFILING
function statement 
MakeAST()
{
	return CreateCode(parameters{
		CreateDefineLabel(BundleStringC("start")),
		CreateDialog(CreateNonLocalizationString(BundleStringC("character1")), 
					CreateLocalizationString(BundleStringC("Buenos días"))),
		CreateDialog(CreateNonLocalizationString(BundleStringC("character2")), 
					CreateLocalizationString(BundleStringC("Buenos días"))),
		CreateDialogWithOptions(CreateNonLocalizationString(BundleStringC("character1")),
			parameters{
				CreateOption(CreateLocalizationString(BundleStringC("quién es usted?"))),
				CreateOption(CreateLocalizationString(BundleStringC("soy character1, usted es?"))),			
				CreateOption(CreateIndirectJump(CreateLocalizationString(BundleStringC("adios")), CreateLabel(BundleStringC("end")))),
				CreateOption(CreateIndirectJump(CreateLocalizationString(BundleStringC("cómo van las cosas?")), CreateLabel(BundleStringC("cosas")))),
			}
		),
		CreateDialog(CreateNonLocalizationString(BundleStringC("character2")), 
					CreateLocalizationString(BundleStringC("soy character2"))),
		CreateJump(CreateLabel(BundleStringC("end"))),
		CreateDefineLabel(BundleStringC("cosas")),
		CreateDialog(CreateNonLocalizationString(BundleStringC("character2")), 
					CreateIndirectJump(CreateLocalizationString(BundleStringC("mal")), CreateLabel(BundleStringC("end")))),
		CreateDefineLabel(BundleStringC("end"))
		
	});
	
	
}
#endif