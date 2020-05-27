#ifndef __ASM_NOP_H__
#define __ASM_NOP_H__

#include "asmTemplate.h"
#include "asmType.h"

class AsmNop: public AsmTemplate
{
public:
	AsmNop()
		: AsmTemplate(ASM_CMD::NOP) {}
	AsmNop(Operand* op1)
		: AsmTemplate(ASM_CMD::NOP, op1) {}
	
	int reserve() override
	{
		return (m_op1 != nullptr) ? (int)m_op1->data : 1;
	}
	
	int write(std::ofstream& f, int offset, int va) override
	{
		offset = offset + 1; // Nothing
		va = va + 1;

		if (m_op1 == nullptr) {
			f.write("\x90", 1);
			return 1;
		}
		
		int c = (int)m_op1->data;
		for (int i=0; i<c; i++)
		{
			f.write("\x90", 1);
		}
		return c;
	}
};	

#endif