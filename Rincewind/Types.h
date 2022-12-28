#pragma once
#include <string>
#include <vector>
#include <map>
#include "Resources.h"

enum statement_type
{
	Dialog,
	DialogWithOptions,
	Option,
	Command,
	Jump,
	Label,
	Call,
	Condition,
	WaitOptionSelection,
	WaitInteraction
};

enum conditional_type
{
	Greater,
	Less,
	GreaterOrEqual,
	LessOrEqual,
	Equal,
	NotEqual
};

struct statement;

typedef std::vector<statement> TStatements;
typedef std::map<std::string, std::string> TParameters; // Check value

struct statement
{
	statement_type Type;
	std::string StrValue;

	//Only use this for conditionals
	// Check type
	TStatements InternalStatement;

	TParameters Parameters;
	bool bByPass = false;
};

internal
statement MakeStatement(const std::string& Name, statement_type Type)
{
	statement Statement;
	Statement.Name = Name;
	Statement.Type = Type;
	return (Statement);
}

internal
void AddParameter(statement* Statement, const std::string& ParameterName, int ParameterValue)
{
	Statement->Parameters[ParameterName] = ParameterValue;
}

internal 
void AddInternalStatement(statement* Statement, statement* InternalStatement)
{	
	Statement->InternalStatement.push_back(*InternalStatement);
}

#define SET_DIALOG "SetDialog"
#define SET_OPTION "SetOption"
#define EXECUTE_COMMAND "ExecuteCommand"
#define JUMP_TO "JumpTo"
#define TITLE "Title"
#define TEXT "Text"

function
statement CreateDialog(resource* Resources, std::string Title, std::string Text, bool hasOptions = false)
{
	statement_type type = hasOptions ? statement_type::DialogWithOptions : statement_type::Dialog;
	statement Statement = MakeStatement(SET_DIALOG, type);
	Statement.bByPass = false;

	AddParameter(&Statement, TITLE, InsertUniqueResource(Resources, Title));		
	if (Text.size() > 0)
		AddParameter(&Statement, TEXT, InsertResource(Resources, Title));
	
	return (Statement);
}


function
statement CreateOption(resource* Resources, std::string Text)
{
	statement Statement = MakeStatement(SET_OPTION, statement_type::Option);
	Statement.bByPass = false;
	AddParameter(&Statement, TEXT, InsertResource(Resources, Text));	
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
function
statement CreateJump(std::string Text, std::string Label)
{
	statement Statement = MakeStatement(JUMP_TO, statement_type::Jump);	
	AddParameter(&Statement, "Text", Text); // why not storing this in resources
	AddParameter(&Statement, "Line", Label);
	Statement.bByPass = false;
	return (Statement);
}

function
statement CreateLabel(std::string Label)
{
	statement Statement = MakeStatement(Label, statement_type::Label);	
	Statement.bByPass = false;
	return (Statement);
}

function
statement CreateCall(std::string FunctionName)
{
	statement Statement = MakeStatement("Call", statement_type::Call);	
	AddParameter(&Statement, "Function", FunctionName);
	Statement.bByPass = false;
	return (Statement);
}

function 
statement CreateCondition(conditional_type ConditionType, std::string Alias, int NumConst)
{
	statement Statement = MakeStatement("Condition", statement_type::Condition);
	AddParameter(&Statement, "ConditionType", std::to_string(ConditionType));
	AddParameter(&Statement, "FirstAlias", Alias);
	AddParameter(&Statement, "Literal", std::to_string(NumConst));

	return (Statement);
}

function
statement CreateCondition(conditional_type ConditionType, std::string FirstAlias, std::string SecondAlias)
{
	statement Statement = MakeStatement("Condition", statement_type::Condition);
	AddParameter(&Statement, "ConditionType", std::to_string(ConditionType));
	AddParameter(&Statement, "FirstAlias", FirstAlias);
	AddParameter(&Statement, "SecondAlias", SecondAlias);
	
	return (Statement);
}

function
statement CreateWaitOptionSelection()
{
	statement Statement = MakeStatement("Wait", statement_type::WaitOptionSelection);

	return (Statement);
}

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
}