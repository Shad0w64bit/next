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
		if (m_op1->type == AsmType::Register) {
			Reg r;
			RegEnum reg = (RegEnum) m_op1->data;
			auto ex = r.get(reg);
			return ((ex->len == 2) ? 1 : 0) + ((ex->rex == REX::Ex) ? 2 : 1);
		}
		
		return 0;
	}
	
	int write(std::ofstream& f, int offset, int va) override
	{
		offset = offset + 1; // Nothing
		va = va + 1;
		
		if (m_op1->type == AsmType::Register) 
		{
			Reg r;
			RegEnum reg = (RegEnum) m_op1->data;
			auto ex = r.get(reg);
			
			int total = 0;
			if (ex->len == 2) { 
				char c = 0x66;
				f.write(&c, 1);
				total++; 
			}
			
			if (ex->rex == REX::Ex) {
				char c = 0x41;
				f.write(&c, 1);
				total++;
			} 
			
			char op = 0x58;
			op |= ex->data;
			f.write(&op, 1);
			total++;			
			return total;
		}
		
		return 0;
	}
	
};

#endif