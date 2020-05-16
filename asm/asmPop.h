#ifndef __ASM_POP_H__
#define __ASM_POP_H__

#include "asmTemplate.h"
#include "asmType.h"

class AsmPop: public AsmTemplate
{
public:
	AsmPop(Operand* op1)
		: AsmTemplate(ASM_CMD::POP, op1) {}
		
	int reserve() override
	{
		return 0;
	}
	
	int write(std::ofstream& f, int offset)
	{
		offset = offset + 1; // Nothing
		
		return 0;
	}
	
};

#endif