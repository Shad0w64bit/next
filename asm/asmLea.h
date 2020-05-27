#ifndef __ASM_LEA_H__
#define __ASM_LEA_H__

#include "asmTemplate.h"
#include "asmType.h"
#include "../builder/dataSection.h"

class AsmLea: public AsmTemplate
{
public:
	AsmLea(Operand* op1, Operand* op2)
		: AsmTemplate(ASM_CMD::MOV, op1, op2) {}
	
	// Length of command
	int reserve() override 
	{
		return 0;
	}
	
	int write(std::ofstream& f, int offset, int va) override
	{
		return 0;
	}
	
};

#endif