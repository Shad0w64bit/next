#ifndef __ASM_PUSH_H__
#define __ASM_PUSH_H__

#include "asmTemplate.h"
#include "asmType.h"

class AsmPush: public AsmTemplate
{
public:
	AsmPush(Operand* op1)
		: AsmTemplate(ASM_CMD::PUSH, op1) {}
	
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