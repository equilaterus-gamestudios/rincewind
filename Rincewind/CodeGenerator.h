#include <string>
#include <map>
#include "Types.h"

class Context;

class CodeGenerator
{
public:
	CodeGenerator() {}
	CodeGenerator(Context* InContext);

	std::string GenerateCode();

	void ProcessJumpStatements();

private:
	static const std::string WAIT_INTERACTION;
	static const std::string WAIT_OPTION_SELECTION;

	Context* CodeContext;

	void ProcessJumpStatements(SStatement* ParentStatement, TStatements& Statemets);

	std::string GenerateCode(TStatements& Statemets);

	std::string GenerateCodeForStatement(SStatement& Statement);

	std::string GetFunctionName(std::string FunctionName);

	std::string GetParameters(TParameters& Parameters);
};

