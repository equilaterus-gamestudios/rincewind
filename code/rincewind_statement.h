#pragma once
#include <vector>
#include "rincewind_globals.h"
#include "rincewind_common.h"

enum class statement_type
{
	None = 0,
	/*Atom types*/
	Number,	
	Label,
	DefineLabel,	
	NonLocalizationString, 
	Identifier,
	Call,
	WaitOptionSelection,
	WaitInteraction,
	Audio,
	/*Non-Termianl*/
	LocalizationString,	// NOTE(pipecaniza): this is a non-terminal because we can store audio inside.
	Dialog,
	DialogWithOptions,
	Option,
	Command,
	Jump,
	IndirectJump, 	//NOTE(pipecaniza): This is a jump that will be executed by the 
					//machine when a dialog is executed, so in the code generation won´t generate a real jump statement
	Condition,

	Code,
	
	/*comparison*/
	Equal,
	NotEqual,
	Greater,
	GreaterOrEqual,
	Less,
	LessOrEqual,

	/*logical*/
	And,
	Or
};

struct statement;
//TODO(pipecaniza): use a custom vector
typedef std::vector<statement> parameters;

//TODO(pipecaniza): find a way to move out values from statement.
struct statement
{
	statement_type Type;
	string StrValue;
	float NumericValue;

	parameters Parameters;
};

internal
statement MakeStatement(statement_type Type)
{
	statement Statement = {};
	Statement.Type = Type;
	return (Statement);
}

inline function bool
IsAtomStatement(const statement* Statement)
{
	if (Statement->Type == statement_type::LocalizationString || Statement->Type == statement_type::NonLocalizationString 
	|| Statement->Type == statement_type::Call    || Statement->Type == statement_type::Identifier 
	|| Statement->Type == statement_type::Label	  || Statement->Type == statement_type::Number
	|| Statement->Type == statement_type::DefineLabel || Statement->Type == statement_type::Audio)
		return true;
	return false;
}

inline function
statement CreateNonLocalizationString(string String)
{
	statement Statement = MakeStatement(statement_type::NonLocalizationString);
	Statement.StrValue = String;
	return (Statement);
}

inline function
statement CreateLocalizationString(string String, statement AudioStatement)
{
	statement Statement = MakeStatement(statement_type::LocalizationString);
	Statement.StrValue = String;
	if (AudioStatement.Type == statement_type::Audio)
		Statement.Parameters.push_back(AudioStatement);
	return (Statement);
}

inline function
statement CreateAudio(string String)
{
	statement Statement = MakeStatement(statement_type::Audio);
	Statement.StrValue = String;
	return (Statement);
}

inline function
statement CreateIdentifier(string String)
{
	statement Statement = MakeStatement(statement_type::Identifier);
	Statement.StrValue = String;
	return (Statement);
}

inline function
statement CreateNumber(float Number)
{
	statement Statement = MakeStatement(statement_type::Number);
	Statement.NumericValue = Number;
	return (Statement);
}

inline function
statement CreateDialog(statement TitleStatement, statement TextStatement)
{	
	statement Statement = MakeStatement(statement_type::Dialog);
	Statement.Parameters.push_back(TitleStatement);
	Statement.Parameters.push_back(TextStatement);
	return (Statement);
}

inline function
statement CreateDialogWithOptions(statement TitleStatement, const parameters& options)
{	
	statement Statement = MakeStatement(statement_type::DialogWithOptions);
	Statement.Parameters.push_back(TitleStatement);
	Statement.Parameters.insert(Statement.Parameters.end(), options.begin(), options.end());
	return (Statement);
}

inline function
statement CreateOption(statement TextStatement)
{
	statement Statement = MakeStatement(statement_type::Option);
	Statement.Parameters.push_back(TextStatement);
	return (Statement);
}

inline function
statement CreateCode(parameters CodeStatements)
{
	statement Statement = MakeStatement(statement_type::Code);
	Statement.Parameters = CodeStatements;
	return (Statement);
}

inline function
statement CreateJump(statement LabelStatement)
{
	statement Statement = MakeStatement(statement_type::Jump);
	Statement.Parameters.push_back(LabelStatement);
	return (Statement);
}

inline function
statement CreateIndirectJump(statement TextStatement, statement LabelStatement)
{
	statement Statement = MakeStatement(statement_type::IndirectJump);
	Statement.Parameters.push_back(TextStatement);
	Statement.Parameters.push_back(LabelStatement);
	return (Statement);
}

inline function
statement CreateLabel(string Label)
{
	statement Statement = MakeStatement(statement_type::Label);
	Statement.StrValue = Label;
	return (Statement);
}

inline function
statement CreateDefineLabel(string Label)
{
	statement Statement = MakeStatement(statement_type::DefineLabel);
	Statement.StrValue = Label;
	return (Statement);
}

inline function
statement CreateCall(string Function)
{
	statement Statement = MakeStatement(statement_type::Call);
	Statement.StrValue = Function;	
	return (Statement);
}


inline function 
statement CreateCondition(statement LogicalStatement, statement CodeStatement)
{
	statement Statement = MakeStatement(statement_type::Condition);	
	Statement.Parameters.push_back(LogicalStatement);	
	Statement.Parameters.push_back(CodeStatement);
	return (Statement);
}

inline function
statement CreateComparison(statement_type Type, statement A, statement B)
{
	statement Statement = MakeStatement(Type);
	Statement.Parameters.push_back(A);
	Statement.Parameters.push_back(B);

	return (Statement);
}

inline function statement
CreateLogicalOperation(statement_type Type, statement A, statement B)
{
	statement Statement = MakeStatement(Type);
	Statement.Parameters.push_back(A);
	Statement.Parameters.push_back(B);

	return (Statement);
}

inline function
statement CreateWaitOptionSelection()
{
	statement Statement = MakeStatement(statement_type::WaitOptionSelection);
	return (Statement);
}
