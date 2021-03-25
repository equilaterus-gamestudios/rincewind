#pragma once
#include <string>
#include <vector>
#include <map>
#include "Resources.h"

enum EStatementType
{
	ESTDialog,
	ESTDialogWithOptions,
	ESTOption,
	ESTCommand,
	ESTJump,
	ESTLabel,
	ESTCall,
	ESTCondition,
	ESTWaitOptionSelection,
	ESTWaitInteraction
};

enum EConditionalType
{
	ECTGreater,
	ECTLess,
	ECTGreaterOrEqual,
	ECTLessOrEqual,
	ECTEqual,
	ECTNotEqual
};

struct FStatement;

typedef std::vector<FStatement> TStatements;
typedef std::map<std::string, std::string> TParameters;
struct FStatement
{
	std::string Name;
	EStatementType Type;

	//Only use this for conditionals
	TStatements InternalStatement;

	TParameters Parameters;
	bool bByPass = false;

	FStatement() = default;
	FStatement(std::string InName, EStatementType InType)
	{
		Name = InName;
		Type = InType;
	}

	void AddParameter(std::string ParameterName, std::string ParameterValue)
	{
		Parameters[ParameterName] = ParameterValue;
	}
	void AddInternalStatement(FStatement& Statement)
	{
		InternalStatement.push_back(Statement);
	}

	static FStatement CreateDialog(FResources& Resources, std::string Title, std::string Text, bool hasOptions = false)
	{
		FStatement Statement = FStatement();
		Statement.Name = "SetDialog";
		Statement.Type = hasOptions ? EStatementType::ESTDialogWithOptions : EStatementType::ESTDialog;
		Statement.AddParameter("Title", Resources.InsertResource(Title, true));
		if (Text.size() > 0)
			Statement.AddParameter("Text", Resources.InsertResource(Text));
		Statement.bByPass = false;
		return Statement;
	}
	static FStatement CreateOption(FResources& Resources, std::string Text)
	{
		FStatement Statement = FStatement();
		Statement.Name = "SetOption";
		Statement.Type = EStatementType::ESTOption;		
		Statement.AddParameter("Text", Resources.InsertResource(Text));
		Statement.bByPass = false;
		return Statement;
	}
	static FStatement CreateCommand(std::string Variable, std::string Value)
	{
		FStatement Statement = FStatement();
		Statement.Name = "ExecuteCommand";
		Statement.Type = EStatementType::ESTCommand;
		std::string FixedVariable = Variable.substr(1);
		FixedVariable[0] = toupper(FixedVariable[0]);
		Statement.AddParameter("Variable", FixedVariable);
		Statement.AddParameter("Value", Value);
		Statement.bByPass = false;
		return Statement;
	}
	static FStatement CreateJump(std::string Text, std::string Label)
	{
		FStatement Statement = FStatement();
		Statement.Name = "JumpTo";
		Statement.Type = EStatementType::ESTJump;
		Statement.AddParameter("Text", Text);
		Statement.AddParameter("Line", Label);
		Statement.bByPass = false;
		return Statement;
	}
	static FStatement CreateLabel(std::string Label)
	{
		FStatement Statement = FStatement();
		Statement.Name = Label;
		Statement.Type = EStatementType::ESTLabel;		
		Statement.bByPass = false;
		return Statement;
	}
	static FStatement CreateCall(std::string FunctionName)
	{
		FStatement Statement = FStatement();
		Statement.Name = "Call";
		Statement.Type = EStatementType::ESTCall;
		Statement.AddParameter("Function", FunctionName);
		Statement.bByPass = false;
		return Statement;
	}
	static FStatement CreateCondition(EConditionalType ConditionType, std::string Alias, int NumConst)
	{
		FStatement Statement = FStatement("Condition", EStatementType::ESTCondition);
		Statement.AddParameter("ConditionType", std::to_string(ConditionType));
		Statement.AddParameter("FirstAlias", Alias);
		Statement.AddParameter("Literal", std::to_string(NumConst));

		return Statement;
	}
	static FStatement CreateCondition(EConditionalType ConditionType, std::string FirstAlias, std::string SecondAlias)
	{
		FStatement Statement = FStatement("Condition", EStatementType::ESTCondition);
		Statement.AddParameter("ConditionType", std::to_string(ConditionType));
		Statement.AddParameter("FirstAlias", FirstAlias);
		Statement.AddParameter("SecondAlias", SecondAlias);
		
		return Statement;
	}

	static FStatement CreateWaitOptionSelection()
	{
		FStatement Statement = FStatement("Wait", EStatementType::ESTWaitOptionSelection);

		return Statement;
	}

	static FStatement CreateOptionFromJump(FResources& Resources, FStatement& JumpStatement)
	{	
		//If is a condition, then, get the jump from the internal statements
		if (JumpStatement.Type == EStatementType::ESTCondition)
		{
			//JumpStatement is a condition
			//For now, lets get the first one
			FStatement& RealJumpStatement = JumpStatement.InternalStatement.front();			
			FStatement Option = CreateOption(Resources, RealJumpStatement.Parameters["Text"]);
			Option.AddParameter("Line", RealJumpStatement.Parameters["Line"]);
			JumpStatement.InternalStatement[0] = Option;
			return JumpStatement; //REVIEW THIS!!!!!!!!!!!!!!!!!!!!!!!!!!
		}		

		FStatement OptionStatement = CreateOption(Resources, JumpStatement.Parameters["Text"]);
		OptionStatement.AddParameter("Line", JumpStatement.Parameters["Line"]);

		return OptionStatement;
	}
	static FStatement CreateDialogFromJump(FResources& Resources, std::string Title, FStatement& JumpStatement)
	{
		//This should not happen!!!!!
		//If is a condition, then, get the jump from the internal statements
		if (JumpStatement.Type == EStatementType::ESTCondition)
		{
			//JumpStatement is a condition
			//For now, lets get the first one
			FStatement& RealJumpStatement = JumpStatement.InternalStatement.front();
			JumpStatement.InternalStatement[0] = CreateDialog(Resources, Title, RealJumpStatement.Parameters["Text"]);
			JumpStatement.InternalStatement[0].AddParameter("Line", RealJumpStatement.Parameters["Line"]);
			return JumpStatement; //REVIEW THIS!!!!!!!!!!!!!!!!!!!!!!!!!!
		}

		FStatement DialogStatement = CreateDialog(Resources, Title, JumpStatement.Parameters["Text"]);
		DialogStatement.AddParameter("Line", JumpStatement.Parameters["Line"]);

		return DialogStatement;
	}

	
	static TStatements JoinTwoStatements(FStatement& FirstStatement, FStatement& SecondStatement)
	{
		return TStatements{ FirstStatement, SecondStatement };
	}

	static void AtBegining(FStatement& Statement, TStatements& Statements)
	{
		Statements.emplace(Statements.begin(), Statement);
	}

	static void AtEnd(FStatement& Statement, TStatements& Statements)
	{
		Statements.push_back(Statement);
	}

	static TStatements JointTwoListOfStatements(TStatements& FirstStatements, TStatements& SecondStatements)
	{
		TStatements JoinedStatements;
		JoinedStatements.reserve(FirstStatements.size() + SecondStatements.size());// preallocate memory    
		
		JoinedStatements.insert(JoinedStatements.end(), FirstStatements.begin(), FirstStatements.end());
		JoinedStatements.insert(JoinedStatements.end(), SecondStatements.begin(), SecondStatements.end());

		return JoinedStatements;
	}
};