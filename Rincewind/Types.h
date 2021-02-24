#pragma once
#include <string>
#include <vector>
#include <map>

enum class EStatementType
{
	ESTDialog,
	ESTDialogWithOptions,
	ESTOption,
	ESTCommand,
	ESTJump,
	ESTLabel
};

struct SStatement;

typedef std::vector<SStatement> TStatements;
typedef std::map<std::string, std::string> TParameters;
struct SStatement
{
	std::string Name;
	EStatementType Type;
	TStatements InternalStatement;
	TParameters Parameters;
	bool bByPass;

	void AddParameter(std::string ParameterName, std::string ParameterValue)
	{
		Parameters[ParameterName] = ParameterValue;
	}
	void AddInternalStatement(SStatement& Statement)
	{
		InternalStatement.push_back(Statement);
	}

	static SStatement CreateDialog(std::string Title, std::string Text, bool hasOptions = false)
	{
		SStatement Statement = SStatement();
		Statement.Name = "SetDialog";
		Statement.Type = hasOptions ? EStatementType::ESTDialogWithOptions : EStatementType::ESTDialog;
		Statement.AddParameter("Title", Title);
		Statement.AddParameter("Text", Text);
		Statement.bByPass = false;
		return Statement;
	}
	static SStatement CreateOption(std::string Text)
	{
		SStatement Statement = SStatement();
		Statement.Name = "SetOption";
		Statement.Type = EStatementType::ESTOption;		
		Statement.AddParameter("Text", Text);
		Statement.bByPass = false;
		return Statement;
	}
	static SStatement CreateCommand(std::string Variable, std::string Value)
	{
		SStatement Statement = SStatement();
		Statement.Name = "ExecuteCommand";
		Statement.Type = EStatementType::ESTCommand;
		std::string FixedVariable = Variable.substr(1);
		FixedVariable[0] = toupper(FixedVariable[0]);
		Statement.AddParameter("Variable", FixedVariable);
		Statement.AddParameter("Value", Value);
		Statement.bByPass = false;
		return Statement;
	}
	static SStatement CreateJump(std::string Text, std::string Label)
	{
		SStatement Statement = SStatement();
		Statement.Name = "JumpTo";
		Statement.Type = EStatementType::ESTJump;
		Statement.AddParameter("Text", Text);
		Statement.AddParameter("Line", Label);
		Statement.bByPass = false;
		return Statement;
	}
	static SStatement CreateLabel(std::string Label)
	{
		SStatement Statement = SStatement();
		Statement.Name = Label;
		Statement.Type = EStatementType::ESTLabel;		
		Statement.bByPass = false;
		return Statement;
	}
};