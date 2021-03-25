#include <string>
#include <map>
#include "Types.h"

/**
 * Rincewind machine
 */

#define NO_JUMP "-2"

enum ERincewindInstructionSet {
	RIS_None,
	// Register Instructions
	RIS_SetFStringRegister,				// mov SR X
	RIS_SetFNameRegister,				// mov NR X
	RIS_SetIntRegister,					// mov IR X
	RIS_SetBoolRegister,				// mov BR X				pending
	RIS_SetMemoryRegister,				// mov MR X				pending

	RIS_SetMemoryFStringRegister,		// load MSR X			pending    PROBABLY DO NOT NEED THIS
	RIS_SetMemoryFNameRegister,			// load MNR X			pending	   PROBABLY DO NOT NEED THIS
	RIS_SetMemoryIntRegister,			// load MIR X			pending	   PROBABLY DO NOT NEED THIS

	RIS_MoveMemoryIntRegisterToIntRegister, //  mov MIR IR

	// Process Instructions
	RIS_ProcessJump,					// jump IR
	RIS_ProcessJumpIf,					// jump IR BR			pending
	RIS_ProcessTitle,					// prc SR
	RIS_ProcessText,					// prc SR
	RIS_ProcessOption,					// prc SR IR
	RIS_WaitInteraction,				// wait
	RIS_WaitOptionSelection,			// wait
	RIS_CallFunction,					// call NR

	// Conditional Instructions
	RIS_LessThan,						// lt MIR IR			pending			if (MIR < IR)
	RIS_LessOrEqual,					// le MIR IR			pending			if (MIR <= IR)
	RIS_GreatThan,						// gt MIR IR			pending			if (MIR > IR)
	RIS_GreatOrEqual,					// ge MIR IR			pending			if (MIR >= IR)
	RIS_Equal,							// eq MIR IR			pending			if (MIR == IR)
	RIS_NotEqual,						// neq MIR IR			pending			if (MIR != IR)

	// Memory Instruccions
	RIS_LoadFString,					// load MSR				pending
	RIS_LoadFName,						// load MNR				pending
	RIS_LoadInt,						// load MIR				pending
	RIS_LoadBool,						// load MBR				pending

	RIS_StoreFString,					// store BR NR MSR 			pending	      BR true = global store
	RIS_StoreFName,						// store BR NR MSR			pending	      BR true = global store
	RIS_StoreInt,						// store BR NR MSR			pending	      BR true = global store
	RIS_StoreBool						// store BR NR MBR			pending	      BR true = global store
};


struct FRincewindStatement
{
	ERincewindInstructionSet Instruction;
	std::string Value;

	FRincewindStatement() : Instruction(ERincewindInstructionSet::RIS_None), Value("") {}
	FRincewindStatement(ERincewindInstructionSet InInstruction, std::string InValue = "") : Instruction(InInstruction), Value(InValue) {}

	std::string ToString()
	{		
		return std::to_string(Instruction) + " " + Value;
	}
};

class Context;

//TODO: Implement store and load
//TODO: Implement local and global storage!
class CodeGenerator
{
public:
	CodeGenerator() {}
	CodeGenerator(Context* InContext);

	std::vector<FRincewindStatement> RincewindCode; //:)


private:

	int ConditionalSequence;

	std::vector<int> LinesWithLabelProcessingPending;

	Context* CodeContext;

	void GenerateDialogCode(FStatement& Statement, bool bWithText);

	void GenerateOptionCode(FStatement& Statement);

	void GenerateWaitForOptionCode();

	void GenerateJumpCode(FStatement& Statement);

	void GenerateCallCode(FStatement& Statement);

	void ProcessLabel(FStatement& Statement);

	void ProcesCondition(FStatement& Statement);

	void ProcesFStatement(FStatement& Statement);	

	void GenerateCode(TStatements& Statemets);
	
public:
	void ProcessJumpsSecondPass();

	void GenerateCode();
};

