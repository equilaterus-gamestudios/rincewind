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
	LocalizationString,
	NonLocalizationString, 
	Identifier,
	Call,
	WaitOptionSelection,
	WaitInteraction,
	/**/
	Dialog,
	DialogWithOptions,
	Option,
	Command,
	Jump,
	IndirectJump, 	//NOTE(pipecaniza): This is a jump that will be executed by the 
					//machine when a dialog is executed, so in the code generation won´t generate a real jump statement
	Condition,

	Code,
	
	/*logical*/
	Equal,
	NotEqual,
	Greater,
	GreaterOrEqual,
	Less,
	LessOrEqual
};

struct statement;
typedef std::vector<statement> parameters;

//TODO(pipecaniza): find a way to move out values from statement.
struct statement
{
	statement_type Type = statement_type::None;
	string StrValue;
	float NumericValue;

	parameters Parameters;
};

internal
statement MakeStatement(statement_type Type)
{
	statement Statement;	
	Statement.Type = Type;
	return (Statement);
}

inline function bool
IsAtomStatement(const statement* Statement)
{
	if (Statement->Type == statement_type::LocalizationString || Statement->Type == statement_type::NonLocalizationString 
	|| Statement->Type == statement_type::Call    || Statement->Type == statement_type::Identifier 
	|| Statement->Type == statement_type::Label	  || Statement->Type == statement_type::Number
	|| Statement->Type == statement_type::DefineLabel)
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
statement CreateLocalizationString(string String)
{
	statement Statement = MakeStatement(statement_type::LocalizationString);
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
/*
function
statement CreateCommand(std::string Variable, std::string Value)
{
	statement Statement = MakeStatement(EXECUTE_COMMAND, statement_type::Command);
	std::string FixedVariable = Variable.substr(1);
	FixedVariable[0] = toupper(FixedVariable[0]);
	AddParameter(&Statement, "Variable", FixedVariable);
	AddParameter(&Statement, "Value", Value);
	Statement.bByPass = false;
	return Statement;
}
*/
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
statement CreateLogicalOperation(statement_type Type, statement A, statement B)
{
	statement Statement = MakeStatement(Type);
	Statement.Parameters.push_back(A);
	Statement.Parameters.push_back(B);

	return (Statement);
}

/*
function
statement CreateCondition(conditional_type ConditionType, std::string FirstAlias, std::string SecondAlias)
{
	statement Statement = MakeStatement("Condition", statement_type::Condition);
	AddParameter(&Statement, "ConditionType", std::to_string(ConditionType));
	AddParameter(&Statement, "FirstAlias", FirstAlias);
	AddParameter(&Statement, "SecondAlias", SecondAlias);
	
	return (Statement);
}*/

inline function
statement CreateWaitOptionSelection()
{
	statement Statement = MakeStatement(statement_type::WaitOptionSelection);
	return (Statement);
}
/*
function
statement CreateOptionFromJump(resource* Resources, statement* JumpStatement)
{	
	//If is a condition, then, get the jump from the internal statements
	if (JumpStatement->Type == statement_type::Condition)
	{
		//JumpStatement is a condition
		//For now, lets get the first one
		statement* RealJumpStatement = &JumpStatement->InternalStatement.front();			
		statement Option = CreateOption(Resources, RealJumpStatement->Parameters["Text"]);
		AddParameter(&Option, "Line", RealJumpStatement->Parameters["Line"]);
		JumpStatement->InternalStatement[0] = Option;
		return (JumpStatement); //REVIEW THIS!!!!!!!!!!!!!!!!!!!!!!!!!!
	}		

	statement OptionStatement = CreateOption(Resources, JumpStatement->Parameters["Text"]);
	AddParameter(&OptionStatement, "Line", JumpStatement->Parameters["Line"]);

	return OptionStatement;
}

function
statement CreateDialogFromJump(resource* Resources, std::string Title, statement* JumpStatement)
{
	//This should not happen!!!!!
	//If is a condition, then, get the jump from the internal statements
	if (JumpStatement->Type == statement_type::Condition)
	{
		//JumpStatement is a condition
		//For now, lets get the first one
		statement* RealJumpStatement = &JumpStatement->InternalStatement.front();
		JumpStatement->InternalStatement[0] = CreateDialog(Resources, Title, RealJumpStatement.Parameters["Text"]);
		AddParameter(&JumpStatement->InternalStatement[0], "Line", RealJumpStatement.Parameters["Line"]);
		return (JumpStatement); //REVIEW THIS!!!!!!!!!!!!!!!!!!!!!!!!!!
	}

	statement DialogStatement = CreateDialog(Resources, Title, JumpStatement->Parameters["Text"]);
	AddParameter(DialogStatement, "Line", JumpStatement->Parameters["Line"]);

	return (DialogStatement);
}

*//*
function
statement JoinTwoStatements(FStatement& FirstStatement, FStatement& SecondStatement)
{
	return TStatements{ FirstStatement, SecondStatement };
}

function
void AtBegining(FStatement& Statement, TStatements& Statements)
{
	Statements.emplace(Statements.begin(), Statement);
}

function
void AtEnd(FStatement& Statement, TStatements& Statements)
{
	Statements.push_back(Statement);
}

function
TStatements JointTwoListOfStatements(TStatements& FirstStatements, TStatements& SecondStatements)
{
	TStatements JoinedStatements;
	JoinedStatements.reserve(FirstStatements.size() + SecondStatements.size());// preallocate memory    
	
	JoinedStatements.insert(JoinedStatements.end(), FirstStatements.begin(), FirstStatements.end());
	JoinedStatements.insert(JoinedStatements.end(), SecondStatements.begin(), SecondStatements.end());

	return JoinedStatements;
}*/