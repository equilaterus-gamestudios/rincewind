#include <iostream>

#include "CodeGenerator.h"
#include "Context.h"

#define CRS(s) RincewindCode.push_back(s)
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
	CRS(FRincewindStatement(ERincewindInstructionSet::RIS_SetFStringRegister, TITLE));
	CRS(FRincewindStatement(ERincewindInstructionSet::RIS_ProcessTitle));
	if (bWithText)
	{
		CRS(FRincewindStatement(ERincewindInstructionSet::RIS_SetFStringRegister, TEXT));
		CRS(FRincewindStatement(ERincewindInstructionSet::RIS_ProcessText));
		CRS(FRincewindStatement(ERincewindInstructionSet::RIS_WaitInteraction));

		/* If the dialog has a jump, like this: 
		 * - Terry Pratchett:
         *   [Hello good sir, bye](#end)
		 */
		if (LINE_EXISTS)
		{
			LinesWithLabelProcessingPending.push_back(RincewindCode.size());
			CRS(FRincewindStatement(ERincewindInstructionSet::RIS_SetIntRegister, LINE));
			CRS(FRincewindStatement(ERincewindInstructionSet::RIS_ProcessJump));
		}		
	}
}

void CodeGenerator::GenerateOptionCode(FStatement& Statement)
{
	CRS(FRincewindStatement(ERincewindInstructionSet::RIS_SetFStringRegister, TEXT));
	if (LINE_EXISTS)
	{
		LinesWithLabelProcessingPending.push_back(RincewindCode.size());
		CRS(FRincewindStatement(ERincewindInstructionSet::RIS_SetIntRegister, LINE));
	}
	else
	{
		CRS(FRincewindStatement(ERincewindInstructionSet::RIS_SetIntRegister, NO_JUMP));
	}
	
	CRS(FRincewindStatement(ERincewindInstructionSet::RIS_ProcessOption));
}

void CodeGenerator::GenerateWaitForOptionCode()
{
	CRS(FRincewindStatement(ERincewindInstructionSet::RIS_WaitOptionSelection));
}

void CodeGenerator::GenerateJumpCode(FStatement& Statement)
{
	LinesWithLabelProcessingPending.push_back(RincewindCode.size());
	CRS(FRincewindStatement(ERincewindInstructionSet::RIS_SetIntRegister, LINE));
	CRS(FRincewindStatement(ERincewindInstructionSet::RIS_ProcessJump));
}

void CodeGenerator::GenerateCallCode(FStatement& Statement)
{
	CRS(FRincewindStatement(ERincewindInstructionSet::RIS_SetFNameRegister, FUNCTION));
	CRS(FRincewindStatement(ERincewindInstructionSet::RIS_CallFunction));
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


void CodeGenerator::ProcesCondition(FStatement& Statement)
{
	Statement.AddParameter("Line", "COND" + std::to_string(ConditionalSequence++));
	EConditionalType ConditionType = EConditionalType(std::stoi(CONDITION_TYPE));
	
	if (SECOND_ALIAS_EXISTS)
	{
		CRS(FRincewindStatement(ERincewindInstructionSet::RIS_SetFNameRegister, SECOND_ALIAS));
		CRS(FRincewindStatement(ERincewindInstructionSet::RIS_LoadInt));
		CRS(FRincewindStatement(ERincewindInstructionSet::RIS_MoveMemoryIntRegisterToIntRegister));
	}
	else
	{
		CRS(FRincewindStatement(ERincewindInstructionSet::RIS_SetIntRegister, LITERAL));
	}

	CRS(FRincewindStatement(ERincewindInstructionSet::RIS_SetFNameRegister, ALIAS));
	CRS(FRincewindStatement(ERincewindInstructionSet::RIS_LoadInt));

	switch (ConditionType)
	{
	case ECTGreater:
		CRS(FRincewindStatement(ERincewindInstructionSet::RIS_GreatThan));
		break;
	case ECTLess:
		CRS(FRincewindStatement(ERincewindInstructionSet::RIS_LessThan));
		break;
	case ECTGreaterOrEqual:
		CRS(FRincewindStatement(ERincewindInstructionSet::RIS_GreatOrEqual));
		break;
	case ECTLessOrEqual:
		CRS(FRincewindStatement(ERincewindInstructionSet::RIS_LessOrEqual));
		break;
	case ECTEqual:
		CRS(FRincewindStatement(ERincewindInstructionSet::RIS_Equal));
		break;
	case ECTNotEqual:
		CRS(FRincewindStatement(ERincewindInstructionSet::RIS_NotEqual));
		break;
	default:
		++CodeContext->Errors;
		std::cout << "The condition type does not have a valid type.\n";
	}

	LinesWithLabelProcessingPending.push_back(RincewindCode.size());	
	CRS(FRincewindStatement(ERincewindInstructionSet::RIS_SetIntRegister, LINE));
	CRS(FRincewindStatement(ERincewindInstructionSet::RIS_ProcessJumpIf));
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
		
		if (Statement.Instruction != ERincewindInstructionSet::RIS_SetIntRegister)
		{
			++CodeContext->Errors;
			std::cout << "Line " << Line << " is not suitable to perform a label processing.\n";
			continue;
		}

		if (CodeContext->Identifiers.count(Statement.Value) == 0)
		{
			++CodeContext->Errors;
			std::cout << Statement.Value << " identifier does not have a definition.\n";
			continue;
		}

		std::cout << Statement.Instruction << " " << Statement.Value << '\n';
		Statement.Value = std::to_string(CodeContext->Identifiers[Statement.Value]);
		std::cout << Statement.Instruction << " " << Statement.Value << '\n';
	}
}

void CodeGenerator::GenerateCode()
{
	GenerateCode(CodeContext->Statements);
}