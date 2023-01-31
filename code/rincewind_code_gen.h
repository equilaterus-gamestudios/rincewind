#pragma once

/// Project
#include "rincewind_globals.h"
#include "rincewind_context.h"
#include "rincewind_common.h"
#include "rincewind_memory.h"
#include "rincewind_resource.h"
#include "rincewind_statement.h"
#include <assert.h>

enum register_type {
	FStringRegister,								// SR	
	IntRegister,									// IR
	BoolRegister,									// BR
	FloatRegister,									// FR
	TextRegister,									// TR

	TypeRegister,									// TyR

	// Memory
	TemporalIntRegister,							// TIR remove it?
};

enum logic_ops {
	And,
	Or
};

enum comparison_ops {
	LessThan,
	LessOrEqual,
	GreatThan,
	GreatOrEqual,
	Equal,
	NotEqual
};


/*
maximum memory address: (2¹⁶) 65536\
_data:
character1 charisma 10
_start:
	lro $ST, 1	#load read only charisma
	lfs			#how to deal with state?
	ld $RT, 0	#load character1

	ld $RT, 3
	prcti
	lde $RT, 4	#load external
	prcitx
	wait
	set $RI, X
	jp 

	lfs $$, 1 #charisma
	lro 

	cargar en registro general
	mover al registro general temporal
	cargar en registro general
	comparar registro general con registro general temporal
	output en registro bool



	!=/= -> string, bool, float
	><(=) -> float
	

*/
//NOTE(pipecaniza): to simplify the code gen no Immediate supported
enum instruction_type {
	None,
	// Register Instructions
	SetRegister,					// set xR X				set a register with a 16bit value
	MoveToTemporal,					// mtt 					move the register of TypeReg to the temporal of that type
	MoveRegister,					// mov (T)xR (T)xR		move data from A to B reg /////remove it???

	// Memory Instructions
	LoadReadOnly,					// lro xR X				load the TyR
	LoadFromState,					// lfs    X				load the TyR, load the correct register
	LoadFromExternal,				// lfe xR X				load the TyR

	// Process Instructions
	ProcessJump,					// jump    X			jump to X
	ProcessJumpIf,					// jmpf    X			jump to X if BR
	ProcessTitle,					// prc IR				uses TR
	ProcessText,					// prc IR				uses TR
	ProcessOption,					// prc IR X             if(BR) use X
	WaitInteraction,				// wait					
	WaitOptionInteraction,			// wait					
	CallFunction,					// call SR

	// Conditional Instructions
	Compare,						// cmp auto X			uses TyR
	LogicOp,						// lg auto  X			x -> 0 and, x -> 1 or | temp CR X CTR

	StoreCompareResult,				// scr
	RestoreCompareResult			// rcr
};

struct instruction
{
	instruction_type Instruction;
	uint FirstParameter;
	uint SecondParameter;
};

inline internal
instruction MakeInstruction(instruction_type Instruction, int FirstParameter, int SecondParameter)
{
	instruction Result;
	Result.Instruction = Instruction;
	Result.FirstParameter = FirstParameter;
	Result.SecondParameter = SecondParameter;
	return(Result);
}

struct pending_label
{
	uint16 InstructionIndex;
	string Label;
};

inline internal pending_label
MakePendingLabel(int InstructionIndex, const string& Label)
{
	pending_label Result;
	Result.InstructionIndex = InstructionIndex;
	Result.Label = Label;
	return(Result);
}

struct machine_code 
{
	uint8 Byte1;
	uint8 Byte2;
	uint8 Byte3;
};

inline internal machine_code
MakeMachineCode(uint I32)
{
	machine_code Result;
	Result.Byte1 = 0xFF & (I32 >> 16);
	Result.Byte2 = 0xFF & (I32 >> 8);
	Result.Byte3 = 0xFF & I32;
	return Result;
}

//TODO(pipecaniza): set this define
#define MAX_CODE_SIZE 65436 //NOTE(pipecaniza): uint16 - 100
#define MAX_PENDING_LABELS 9999
#define MAX_AUTO_LABELS 20000
struct code_gen
{
	machine_code* MachineCode;
	uint16 MachineCodeSize;

	instruction* Code;
	uint16 CodeSize;

	context* Context;
	resource_container* Resource;

	// NOTE(pipecaniza): label hash, line code to jump
	hash_table CacheLabels;
	pending_label* PendingLabels;
	uint16 PendingLabelsSize;

	uint16* AutoLabels;
	uint16 AutoLabelsIndex;
};

inline function code_gen
MakeCodeGen(context* Context, resource_container* Resource, arena* Arena)
{
	code_gen Result = {};
	Result.Context = Context;
	Result.Resource = Resource;
	Result.MachineCode = (machine_code*)ReserveMemory(Arena, sizeof(machine_code) * MAX_CODE_SIZE);
	Result.Code = (instruction*)ReserveMemory(Arena, sizeof(instruction) * MAX_CODE_SIZE);
	Result.CacheLabels = MakeHashTable(Arena, MAX_PENDING_LABELS); //TODO(pipecaniza): check this size
	Result.AutoLabels = (uint16*)ReserveMemory(Arena, sizeof(uint16) * MAX_AUTO_LABELS);
	Result.PendingLabels = (pending_label*)ReserveMemory(Arena, sizeof(pending_label)* MAX_PENDING_LABELS);
	return(Result);
}


inline internal void
PushInstruction(code_gen* CodeGen, instruction&& Instruction)
{
	assert(CodeGen->CodeSize < MAX_CODE_SIZE);
	CodeGen->Code[CodeGen->CodeSize++] = Instruction;
}

#define PushAndMakeInst(CG, it, p1, p2) PushInstruction(CG, MakeInstruction(it, p1, p2))

inline internal void 
AddLabel(code_gen* CodeGen, const string& Label)
{
	uint32 Hash = GenerateHash(Label.Data, Label.Size);
	int16 Index = GetIndex(&CodeGen->CacheLabels, Hash);
	if (Index != -1)
	{
		++CodeGen->Context->GeneratingErrors;
		printf("Genereting code error: Label %.*s is duplicated.\n", Label.Size, Label.Data);
		return;
	}
	AddElement(&CodeGen->CacheLabels, Hash, CodeGen->CodeSize);
}

inline internal void
AddPendingLabel(code_gen* CodeGen, const string& Label)
{
	assert(CodeGen->PendingLabelsSize < MAX_PENDING_LABELS);
	CodeGen->PendingLabels[CodeGen->PendingLabelsSize++] = MakePendingLabel(CodeGen->CodeSize, Label);
}

inline internal string
AddPendingAutoLabel(code_gen* CodeGen)
{
	assert(CodeGen->AutoLabelsIndex < MAX_AUTO_LABELS);
	// NOTE(pipecaniza): We're generating autolabels with string of 2 bytes, from x0000 to xFFFF,
	// could this generate collisions with real labels? maybe only allow labels with length > 2bytes
	CodeGen->AutoLabels[CodeGen->AutoLabelsIndex] = CodeGen->AutoLabelsIndex + 1;
	string Result = MakeString((uint8*)(CodeGen->AutoLabels + CodeGen->AutoLabelsIndex), sizeof(uint16));
	++CodeGen->AutoLabelsIndex;
	AddPendingLabel(CodeGen, Result);	
	return(Result);
}

/*
NOTE(pipecaniza):
4- 5-bits instruction opcode. 4-bits registers....

	xxxx x|xxx x|xxx xxxx xxxx xxxx xxxx xxxx = 4 bytes

	4.194.304

5- How to deal with Immediate data?
	We can store all the atom types as resources. Yes, stream all Immediate data at the
	beginning of the compiled code.
6- How to handle audio files?

*/

// TODO(pipecaniza): separate immediate strings from nonlocalization?
inline internal int
AddAtomToResources(resource_container* Resource, statement* AtomStatement)
{
	switch (AtomStatement->Type) 
	{
	case statement_type::LocalizationString:
	{
		uint8 ParametersSize = AtomStatement->Parameters.size();
		assert(ParametersSize == 0 || ParametersSize == 1);
		string* AudioString = 0;
		// NOTE(pipecaniza): we have an audio statement		
		if (ParametersSize)
		{
			AudioString = &AtomStatement->Parameters[0].StrValue;
		}
		return InsertLocalization(Resource, &AtomStatement->StrValue, AudioString);
	}
		
	case statement_type::Number:
		return InsertImmediateNumberData(Resource, AtomStatement->NumericValue);	
	case statement_type::NonLocalizationString:		
	case statement_type::Identifier:
	case statement_type::Call:
		return InsertNonLocalizationData(Resource, AtomStatement->StrValue);
	default:
		return -1;
	}
}

inline internal logic_ops
GetLogicOp(const statement* Statement)
{
	switch (Statement->Type) 
	{
        case statement_type::And: return logic_ops::And;
        case statement_type::Or:  return logic_ops::Or;
		default: assert(0);
	}
}

inline internal comparison_ops
GetComparisonOp(const statement* Statement)
{
	switch (Statement->Type) 
	{
        case statement_type::Equal: 		 return comparison_ops::Equal;
        case statement_type::NotEqual: 		 return comparison_ops::NotEqual;
        case statement_type::Greater: 		 return comparison_ops::GreatThan;
        case statement_type::GreaterOrEqual: return comparison_ops::GreatOrEqual;
        case statement_type::Less: 			 return comparison_ops::LessThan;
        case statement_type::LessOrEqual: 	 return comparison_ops::LessOrEqual;
		default: assert(0);
	}
}

//TODO(pipecaniza): support bools in readonly
#define NONE 0
#define PENDING 0
#define FALSE 0
#define TRUE 1
internal void
GenerateInstructions(code_gen* CodeGen, statement* Statement)
{
	switch (Statement->Type) 
	{
		case statement_type::DefineLabel: 
			AddLabel(CodeGen, Statement->StrValue);
			break;
		case statement_type::Label:
			AddPendingLabel(CodeGen, Statement->StrValue);
			break;
		case statement_type::LocalizationString:
			PushAndMakeInst(CodeGen, instruction_type::LoadFromExternal, NONE, AddAtomToResources(CodeGen->Resource, Statement));
			break;
		case statement_type::NonLocalizationString:
			PushAndMakeInst(CodeGen, instruction_type::LoadReadOnly, register_type::TextRegister, AddAtomToResources(CodeGen->Resource, Statement));
			break;
		case statement_type::Identifier:
			//Identifier name from read-only
			PushAndMakeInst(CodeGen, instruction_type::LoadReadOnly, register_type::FStringRegister, AddAtomToResources(CodeGen->Resource, Statement));
			//Get value to GPR from state
			PushAndMakeInst(CodeGen, instruction_type::LoadFromState, NONE, AddAtomToResources(CodeGen->Resource, Statement));			
			break;
		case statement_type::Number:
			PushAndMakeInst(CodeGen, instruction_type::LoadReadOnly, register_type::FloatRegister, AddAtomToResources(CodeGen->Resource, Statement));
			break;
        case statement_type::Call:			
			PushAndMakeInst(CodeGen, instruction_type::LoadReadOnly, register_type::FStringRegister, AddAtomToResources(CodeGen->Resource, Statement));
			PushAndMakeInst(CodeGen, instruction_type::CallFunction, NONE, NONE);
			break;
        case statement_type::Dialog:
			GenerateInstructions(CodeGen, &Statement->Parameters[0]);
			PushAndMakeInst(CodeGen, instruction_type::ProcessTitle, NONE, NONE);			
			GenerateInstructions(CodeGen, &Statement->Parameters[1]);
			PushAndMakeInst(CodeGen, instruction_type::ProcessText, NONE, NONE);
			PushAndMakeInst(CodeGen, instruction_type::WaitInteraction, NONE, NONE);		
			break;
        case statement_type::DialogWithOptions:
			GenerateInstructions(CodeGen, &Statement->Parameters[0]);
			PushAndMakeInst(CodeGen, instruction_type::ProcessTitle, NONE, NONE);
			for (int i = 1; i < Statement->Parameters.size(); ++i) {
				GenerateInstructions(CodeGen, &Statement->Parameters[i]);
			}			
			PushAndMakeInst(CodeGen, instruction_type::WaitOptionInteraction, NONE, NONE);
			break;
        case statement_type::Option:
			PushAndMakeInst(CodeGen, instruction_type::SetRegister, register_type::BoolRegister, FALSE);
			GenerateInstructions(CodeGen, &Statement->Parameters[0]);
			PushAndMakeInst(CodeGen, instruction_type::ProcessOption, NONE, NONE);   
			break;
        case statement_type::Jump:
			//Label				
			GenerateInstructions(CodeGen, &Statement->Parameters[0]);
			PushAndMakeInst(CodeGen, instruction_type::ProcessJump, NONE, PENDING);
			break;
		case statement_type::IndirectJump:
			//String
			GenerateInstructions(CodeGen, &Statement->Parameters[0]);
			//Label
			GenerateInstructions(CodeGen, &Statement->Parameters[1]);			
			PushAndMakeInst(CodeGen, instruction_type::SetRegister, register_type::IntRegister, PENDING);
			PushAndMakeInst(CodeGen, instruction_type::SetRegister, register_type::BoolRegister, TRUE);		
			break;
        case statement_type::Condition:			
		{
			GenerateInstructions(CodeGen, &Statement->Parameters[0]);
			string PendingAutoLabel = AddPendingAutoLabel(CodeGen);			
			PushAndMakeInst(CodeGen, instruction_type::ProcessJumpIf, NONE, PENDING);
			GenerateInstructions(CodeGen, &Statement->Parameters[1]);
			AddLabel(CodeGen, PendingAutoLabel);
			break;			
		}
        case statement_type::Code:
			for (auto& Param: Statement->Parameters) {
				GenerateInstructions(CodeGen, &Param);
			}
			break;
        case statement_type::Equal:	//TODO(pipecaniza): implement strings and bool comparison
        case statement_type::NotEqual:			
        case statement_type::Greater:
		case statement_type::GreaterOrEqual:
		case statement_type::Less:
		case statement_type::LessOrEqual:
			// Get second operant
			GenerateInstructions(CodeGen, &Statement->Parameters[1]);
			PushAndMakeInst(CodeGen, instruction_type::MoveToTemporal, NONE, NONE);
			//TODO(pipecaniza):check the types
			// Get first operant
			GenerateInstructions(CodeGen, &Statement->Parameters[0]);
			PushAndMakeInst(CodeGen, instruction_type::Compare, NONE, GetComparisonOp(Statement));
			break;
		case statement_type::And:
		case statement_type::Or:
			// Get first operant 	
			GenerateInstructions(CodeGen, &Statement->Parameters[0]);
			// Store first operant
			PushAndMakeInst(CodeGen, instruction_type::StoreCompareResult, NONE, NONE);
			// Get second operant	
			GenerateInstructions(CodeGen, &Statement->Parameters[1]);
			// Restore first operant in TCR
			PushAndMakeInst(CodeGen, instruction_type::RestoreCompareResult, NONE, NONE);
			PushAndMakeInst(CodeGen, instruction_type::LogicOp, NONE, GetLogicOp(Statement));
			break;
		default: 
			assert(0);
	}
}

#undef PushAndMakeInst
#undef NONE
#undef PENDING
#undef FALSE
#undef TRUE

internal void
ProcessLabels(code_gen* CodeGen)
{
	for (int i = 0; i < CodeGen->PendingLabelsSize; ++i) 
	{		
		const pending_label& PendingLabel = CodeGen->PendingLabels[i];
		const uint32 Hash = GenerateHash(PendingLabel.Label.Data, PendingLabel.Label.Size);
		assert(PendingLabel.InstructionIndex < CodeGen->CodeSize);
		int16 Index = GetIndex(&CodeGen->CacheLabels, Hash);
		if (Index == -1)
		{
			++CodeGen->Context->GeneratingErrors;
			printf("Genereting code error: Label %.*s is not defined.\n", 
				PendingLabel.Label.Size, PendingLabel.Label.Data);
		}
		CodeGen->Code[PendingLabel.InstructionIndex].SecondParameter = CodeGen->CacheLabels.Values[Index];
	}
}

function void
GenerateCode (code_gen* CodeGen)
{
	GenerateInstructions(CodeGen, &CodeGen->Context->AbstractTree);
	ProcessLabels(CodeGen);
}

//NOTE(pipecaniza): max memory pos 2²³
// xxxx xxxx xxxx xxxx xxxx xxxx
// 5 bits opcode, 4 bits reg, other param

// TODO(pipecaniza): check if is better to leave padding in the instruction to complete 4Bytes
function void
GenerateMachineCode(code_gen* CodeGen)
{
	for (int i = 0; i < CodeGen->CodeSize; ++i) 
	{
		assert(CodeGen->MachineCodeSize < MAX_CODE_SIZE);
		const instruction& Instruction = CodeGen->Code[i];
		uint MachineInstruction = ((uint)Instruction.Instruction & 0x1f) << 19 | 
								(Instruction.FirstParameter & 0x7) << 16 |
								(Instruction.SecondParameter & 0x7fffff);
		CodeGen->MachineCode[CodeGen->MachineCodeSize++] = MakeMachineCode(MachineInstruction);
	}
}

inline function void
ExportCode(code_gen* CodeGen, FILE* File)
{
	fwrite(CodeGen->MachineCode, sizeof(machine_code), CodeGen->MachineCodeSize, File);
}