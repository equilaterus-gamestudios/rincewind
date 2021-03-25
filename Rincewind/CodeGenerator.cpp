#include <iostream>

#include "CodeGenerator.h"
#include "Context.h"

#define CRS(s) RincewindCode.push_back(s)
#define REG(r) std::to_string(r)
#define FUNCTION Statement.Parameters["Function"]
#define LINE Statement.Parameters["Line"]
#define LINE_EXISTS Statement.Parameters.count("Line")
#define TITLE Statement.Parameters["Title"]
#define TEXT Statement.Parameters["Text"]
#define CONDITION_TYPE Statement.Parameters["ConditionType"]
#define ALIAS Statement.Parameters["FirstAlias"]
#define SECOND_ALIAS Statement.Parameters["SecondsAlias"]
#define SECOND_ALIAS_EXISTS Statement.Parameters.count("SecondsAlias")
#define LITERAL Statement.Parameters["Literal"]

CodeGenerator::CodeGenerator(Context* InContext)
{
	CodeContext = InContext;
	ConditionalSequence = 0;
}


void CodeGenerator::GenerateDialogCode(FStatement& Statement, bool bWithText = true)
{	
	CRS(FRincewindStatement(RIS_SetRegister, REG(RR_FStringRegister), TITLE));
	CRS(FRincewindStatement(RIS_ProcessTitle));
	if (bWithText)
	{
		CRS(FRincewindStatement(RIS_SetRegister, REG(RR_FStringRegister), TEXT));
		CRS(FRincewindStatement(RIS_ProcessText));
		CRS(FRincewindStatement(RIS_WaitInteraction));

		/* If the dialog has a jump, like this: 
		 * - Terry Pratchett:
         *   [Hello good sir, bye](#end)
		 */
		if (LINE_EXISTS)
		{
			LinesWithLabelProcessingPending.push_back(RincewindCode.size());
			CRS(FRincewindStatement(RIS_SetRegister, REG(RR_IntRegister), LINE));
			CRS(FRincewindStatement(RIS_ProcessJump));
		}		
	}
}

void CodeGenerator::GenerateOptionCode(FStatement& Statement)
{
	CRS(FRincewindStatement(RIS_SetRegister, REG(RR_FStringRegister), TEXT));
	if (LINE_EXISTS)
	{
		LinesWithLabelProcessingPending.push_back(RincewindCode.size());
		CRS(FRincewindStatement(RIS_SetRegister, REG(RR_IntRegister), LINE));
	}
	else
	{
		CRS(FRincewindStatement(RIS_SetRegister, REG(RR_IntRegister), NO_JUMP));
	}
	
	CRS(FRincewindStatement(RIS_ProcessOption));
}

void CodeGenerator::GenerateWaitForOptionCode()
{
	CRS(FRincewindStatement(RIS_WaitOptionSelection));
}

void CodeGenerator::GenerateJumpCode(FStatement& Statement)
{
	LinesWithLabelProcessingPending.push_back(RincewindCode.size());
	CRS(FRincewindStatement(RIS_SetRegister, REG(RR_IntRegister), LINE));	
	CRS(FRincewindStatement(RIS_ProcessJump));
}

void CodeGenerator::GenerateCallCode(FStatement& Statement)
{
	CRS(FRincewindStatement(RIS_SetRegister, REG(RR_FNameRegister), FUNCTION));
	CRS(FRincewindStatement(RIS_CallFunction));
}

void CodeGenerator::ProcessLabel(FStatement& Statement)
{
	std::string Label = Statement.Name;	
	if (CodeContext->Identifiers.count(Label))
	{
		++CodeContext->Errors;
		std::cout << Label << " identifier already exists.\n";
		return;
	}

	int Position = RincewindCode.size();
	CodeContext->AddIdentifier(Statement.Name, Position);	
}

std::string CodeGenerator::GenerateStoreContext(std::string& Alias)
{	
	bool bLocal = Alias[0] == '_';	
	if (bLocal)
	{
		CRS(FRincewindStatement(RIS_SetRegister, REG(RR_BoolRegister), "0"));
		return Alias.substr(1);
	}
	else
		CRS(FRincewindStatement(RIS_SetRegister, REG(RR_BoolRegister), "1"));

	return Alias;	
}

void CodeGenerator::ProcesCondition(FStatement& Statement)
{
	Statement.AddParameter("Line", "COND" + std::to_string(ConditionalSequence++));
	EConditionalType ConditionType = EConditionalType(std::stoi(CONDITION_TYPE));
	
	if (SECOND_ALIAS_EXISTS)
	{
		std::string Alias = GenerateStoreContext(SECOND_ALIAS);
		CRS(FRincewindStatement(RIS_SetRegister, REG(RR_FNameRegister), Alias));
		CRS(FRincewindStatement(RIS_LoadInt));
		CRS(FRincewindStatement(RIS_MoveMemoryIntRegisterToIntRegister));
	}
	else
	{
		CRS(FRincewindStatement(RIS_SetRegister, REG(RR_IntRegister), LITERAL));
	}

	std::string Alias = GenerateStoreContext(ALIAS);
	CRS(FRincewindStatement(RIS_SetRegister, REG(RR_FNameRegister), Alias));
	CRS(FRincewindStatement(RIS_LoadInt));

	switch (ConditionType)
	{
	case ECTGreater:
		CRS(FRincewindStatement(RIS_GreatThan));
		break;
	case ECTLess:
		CRS(FRincewindStatement(RIS_LessThan));
		break;
	case ECTGreaterOrEqual:
		CRS(FRincewindStatement(RIS_GreatOrEqual));
		break;
	case ECTLessOrEqual:
		CRS(FRincewindStatement(RIS_LessOrEqual));
		break;
	case ECTEqual:
		CRS(FRincewindStatement(RIS_Equal));
		break;
	case ECTNotEqual:
		CRS(FRincewindStatement(RIS_NotEqual));
		break;
	default:
		++CodeContext->Errors;
		std::cout << "The condition type does not have a valid type.\n";
	}

	LinesWithLabelProcessingPending.push_back(RincewindCode.size());
	CRS(FRincewindStatement(RIS_SetRegister, REG(RR_IntRegister), LINE));
	CRS(FRincewindStatement(RIS_ProcessJumpIf));
	Statement.Name = LINE;

	
}

void CodeGenerator::ProcesFStatement(FStatement& Statement)
{
	switch (Statement.Type)
	{
	case EStatementType::ESTDialog:
		GenerateDialogCode(Statement); break;
	case EStatementType::ESTDialogWithOptions:
		GenerateDialogCode(Statement, false); break;	
	case EStatementType::ESTOption:
		GenerateOptionCode(Statement); break;
	case EStatementType::ESTJump:
		GenerateJumpCode(Statement); break;
	case EStatementType::ESTCall:	
		GenerateCallCode(Statement); break;
	case EStatementType::ESTLabel:
		ProcessLabel(Statement); break;
	case EStatementType::ESTCondition:
		ProcesCondition(Statement);
		GenerateCode(Statement.InternalStatement);
		ProcessLabel(Statement);
		break;
	case EStatementType::ESTWaitOptionSelection:
		GenerateWaitForOptionCode(); break;
	default:
		++CodeContext->Errors;
		std::cout << "The statement does not have a valid type.\n";
		break;
	}
}



void CodeGenerator::GenerateCode(TStatements& Statements)
{
	for (auto& Statement : Statements)
	{
		ProcesFStatement(Statement);		
	}
}

void CodeGenerator::ProcessJumpsSecondPass()
{
	for (auto& Line : LinesWithLabelProcessingPending)
	{
		
		FRincewindStatement& Statement = RincewindCode[Line];
		
		if (Statement.Instruction != RIS_SetRegister && Statement.FirstParam == REG(RR_IntRegister))
		{
			++CodeContext->Errors;
			std::cout << "Line " << Line << " is not suitable to perform a label processing.\n";
			continue;
		}

		if (CodeContext->Identifiers.count(Statement.SecondParam) == 0)
		{
			++CodeContext->Errors;
			std::cout << Statement.SecondParam << " identifier does not have a definition.\n";
			continue;
		}

		
		Statement.SecondParam = std::to_string(CodeContext->Identifiers[Statement.SecondParam]);
	}
}

void CodeGenerator::GenerateCode()
{
	GenerateCode(CodeContext->Statements);
}