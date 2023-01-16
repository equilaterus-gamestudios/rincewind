#pragma once

/// Project
#include "rincewind_globals.h"
#include "rincewind_context.h"
#include "rincewind_common.h"
#include "rincewind_resource.h"
#include <cassert>

enum register_type {
	FStringRegister,								// SR	
	IntRegister,									// IR
	BoolRegister,									// BR
	FloatRegister,									
	TextRegister,

	TypeRegister,

	// Memory
	TemporalIntRegister,							// TIR
};

enum logic_ops {
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
	LoadReadOnly,					// lro xR X				load the TypeReg
	LoadFromState,					// lfs    X				load the TypeReg, load the correct register
	LoadFromExternal,				// lfe xR X				load the TypeReg

	// Process Instructions
	ProcessJump,					// jump    X			jump to X
	ProcessJumpIf,					// jmpf    X			jump to X if BR
	ProcessTitle,					// prc IR				uses TR
	ProcessText,					// prc IR				usesTR
	ProcessOption,					// prc IR X             if(BR) use X
	WaitInteraction,				// wait					
	WaitOptionInteraction,			// wait					
	CallFunction,					// call SR

	// Conditional Instructions
	Compare							// cmp auto X
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
	int InstructionIndex;
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
#define MAX_CODE_SIZE 10000
#define MAX_PENDING_LABELS 5000
#define MAX_AUTO_LABELS 5000
struct code_gen
{
	machine_code MachineCode[MAX_CODE_SIZE];
	int MachineCodeSize = 0;
	instruction Code[MAX_CODE_SIZE];
	int CodeSize = 0;
	context* Context;
	resource_container* Resource;

	std::unordered_map<size_t, int> CacheLabels;
	pending_label PendingLabels[MAX_PENDING_LABELS];
	int PendingLabelsSize;

	char AutoLabels[MAX_AUTO_LABELS];
	char* AutoLabelsPtr;
};


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
	const unsigned int Hash = GenerateHash(Label);
	if (CodeGen->CacheLabels.count(Hash))
	{
		++CodeGen->Context->GeneratingErrors;
		printf("Genereting code error: Label %.*s is duplicated.\n", Label.Size, Label.Data);
		return;
	}
	CodeGen->CacheLabels.emplace(Hash, CodeGen->CodeSize);
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
	assert(CodeGen->PendingLabelsSize < MAX_PENDING_LABELS);
	assert(CodeGen->AutoLabelsPtr < CodeGen->AutoLabels + MAX_AUTO_LABELS);
	string Result = MakeString(CodeGen->AutoLabelsPtr++, 1);
	CodeGen->PendingLabels[CodeGen->PendingLabelsSize++] = MakePendingLabel(CodeGen->CodeSize, Result);
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

inline internal int
AddAtomToResources(resource_container* Resource, const statement* AtomStatement)
{
	switch (AtomStatement->Type) 
	{
	case statement_type::LocalizationString:
		return InsertLocalization(Resource, AtomStatement->StrValue);
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
GetLogicalOp(const statement* Statement)
{
	switch (Statement->Type) 
	{
        case statement_type::Equal: return logic_ops::Equal;
        case statement_type::NotEqual: return logic_ops::NotEqual;
        case statement_type::Greater: return logic_ops::GreatThan;
        case statement_type::GreaterOrEqual: return logic_ops::GreatOrEqual;
        case statement_type::Less: return logic_ops::LessThan;
        case statement_type::LessOrEqual: return logic_ops::LessOrEqual;
		default: assert(0);
	}
}

//TODO(pipecaniza): support bools in readonly
#define NONE 0
#define PENDING 0
#define FALSE 0
#define TRUE 1
internal void
GenerateInstructions(code_gen* CodeGen, const statement* Statement)
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
			PushAndMakeInst(CodeGen, instruction_type::Compare, NONE, GetLogicalOp(Statement));
			break;
			default: return;
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
		const unsigned int Hash = GenerateHash(PendingLabel.Label);
		assert(PendingLabel.InstructionIndex < CodeGen->CodeSize);
		assert(CodeGen->CacheLabels.count(Hash));
		CodeGen->Code[PendingLabel.InstructionIndex].SecondParameter = CodeGen->CacheLabels[Hash];
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