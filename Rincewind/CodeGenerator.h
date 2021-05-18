#include <string>
#include <map>
#include "Types.h"

/**
 * Rincewind machine
 */

#define NO_JUMP "-2"

enum ERincewindRegisters {
	RR_FStringRegister,					// SR
	RR_FNameRegister,					// NR
	RR_IntRegister,						// IR
	RR_BoolRegister,					// BR

	// Memory
	RR_MemoryFStringRegister,			// MSR
	RR_MemoryFNameRegister,				// MNR
	RR_MemoryIntRegister				// MIR
};

enum ERincewindInstructionSet {
	RIS_None,
	// Register Instructions
	RIS_SetRegister,					// load XR X

	RIS_SetMemoryFStringRegister,		// load MSR X			pending    PROBABLY DO NOT NEED THIS
	RIS_SetMemoryFNameRegister,			// load MNR X			pending	   PROBABLY DO NOT NEED THIS
	RIS_SetMemoryIntRegister,			// load MIR X			pending	   PROBABLY DO NOT NEED THIS

	RIS_MoveMemoryIntRegisterToIntRegister, //  mov MIR IR

	// Process Instructions
	RIS_ProcessJump,					// jump IR
	RIS_ProcessJumpIf,					// jump IR BR			
	RIS_ProcessTitle,					// prc SR
	RIS_ProcessText,					// prc SR
	RIS_ProcessOption,					// prc SR IR
	RIS_WaitInteraction,				// wait
	RIS_WaitOptionSelection,			// wait
	RIS_CallFunction,					// call NR

	// Conditional Instructions
	RIS_LessThan,						// lt MIR IR			
	RIS_LessOrEqual,					// le MIR IR			
	RIS_GreatThan,						// gt MIR IR			
	RIS_GreatOrEqual,					// ge MIR IR			
	RIS_Equal,							// eq MIR IR			
	RIS_NotEqual,						// neq MIR IR			

	// Memory Instruccions
	RIS_LoadFString,					// load BR NR MSR		pending			BR true = global store
	RIS_LoadFName,						// load BR NR MNR		pending			BR true = global store
	RIS_LoadInt,						// load BR NR MIR		pending			BR true = global store
	RIS_LoadBool,						// load BR NR MBR		pending			BR true = global store

	RIS_StoreFString,					// store BR NR MSR 			pending	      BR true = global store
	RIS_StoreFName,						// store BR NR MSR			pending	      BR true = global store
	RIS_StoreInt,						// store BR NR MSR			pending	      BR true = global store
	RIS_StoreBool						// store BR NR MBR			pending	      BR true = global store
};


struct FRincewindStatement
{
	ERincewindInstructionSet Instruction;
	std::string FirstParam;
	std::string SecondParam;

	FRincewindStatement() : Instruction(ERincewindInstructionSet::RIS_None), FirstParam(""), SecondParam("") {}
	FRincewindStatement(ERincewindInstructionSet InInstruction, std::string InFirstParam = "", std::string InSecondParam = "") : Instruction(InInstruction), FirstParam(InFirstParam), SecondParam(InSecondParam) {}	

	std::string ToString()
	{		
		return std::to_string(Instruction) + " " + FirstParam + " " + SecondParam;
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

	std::string GenerateStoreContext(std::string& Alias);

	void ProcesCondition(FStatement& Statement);

	void ProcesFStatement(FStatement& Statement);	

	void GenerateCode(TStatements& Statemets);
	
public:
	void ProcessJumpsSecondPass();

	void GenerateCode();
};

