#include <iostream>

#include "CodeGenerator.h"
#include "Context.h"


const std::string CodeGenerator::WAIT_INTERACTION = "{\"FunctionName\": \"WaitInteraction\"}";
const std::string CodeGenerator::WAIT_OPTION_SELECTION = "{\"FunctionName\": \"WaitOptionSelection\"}";

CodeGenerator::CodeGenerator(Context* InContext)
{
	CodeContext = InContext;
}

std::string CodeGenerator::GenerateCode()
{
	return "{\"Statements\":[" + GenerateCode(CodeContext->Statements) + "]}";
}


void CodeGenerator::ProcessJumpStatements()
{
	ProcessJumpStatements(nullptr, CodeContext->Statements);	
}

void CodeGenerator::ProcessJumpStatements(SStatement* ParentStatement, TStatements& Statements)
{
	for (auto& Statement : Statements)
	{
		if (Statement.Type == EStatementType::ESTJump)
		{
			std::string Label = Statement.Parameters["Line"];
			if (CodeContext->Identifiers.count(Label) == 0)
			{
				std::cout << Label << " identifier does not have a definition.\n";
				return;
			}
			int Row = CodeContext->Identifiers[Label];

			if (ParentStatement && ParentStatement->Type == EStatementType::ESTOption)
			{
				ParentStatement->AddParameter("JumpTo", std::to_string(Row));
				Statement.bByPass = true;
			}
			else
			{
				Statement.Parameters["Line"] = std::to_string(Row);
			}
		}
		
		if (!Statement.InternalStatement.empty())
		{
			ProcessJumpStatements(&Statement, Statement.InternalStatement);
		}
	}
}

std::string CodeGenerator::GenerateCode(TStatements& Statemets)
{
	std::string Code = "";
	for (auto& Statement : Statemets)
	{
		if (Statement.bByPass)
		{
			continue;
		}

		if (Code != "")
		{
			Code += ",";
		}

		Code += GenerateCodeForStatement(Statement);

		if (!Statement.InternalStatement.empty())
		{
			std::string InternalCode = GenerateCode(Statement.InternalStatement);
			if (InternalCode != "")
			{
				Code += "," + InternalCode;
			}
		}

		if (Statement.Type == EStatementType::ESTDialog)
		{
			Code += Statement.InternalStatement.empty()
				? "," + WAIT_INTERACTION
				: "," + WAIT_OPTION_SELECTION;
		}
	}
	return Code;
}

std::string CodeGenerator::GetFunctionName(std::string FunctionName)
{
	return "\"FunctionName\":\"" + FunctionName + "\"";
}

std::string CodeGenerator::GetParameters(TParameters& Parameters)
{
	std::string Code = "\"Parameters\":{";
	std::string CodeParameters = "";
	for (auto& Parameter : Parameters)
	{
		if (CodeParameters != "")
		{
			CodeParameters += ",";
		}
		CodeParameters += "\"" + Parameter.first + "\":\"" + Parameter.second + "\"";
	}

	return Code + CodeParameters + "}";
}


std::string CodeGenerator::GenerateCodeForStatement(SStatement& Statement)
{
	std::string Code = GetFunctionName(Statement.Name) + ",";	
	Code += GetParameters(Statement.Parameters);

	return "{" + Code + "}";
}