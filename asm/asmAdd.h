#ifndef __ASM_ADD_H__
#define __ASM_ADD_H__

#include "asmTemplate.h"
#include "asmType.h"

class AsmAdd: public AsmTemplate
{
public:
	AsmAdd(Operand* op1, Operand* op2)
		: AsmTemplate(ASM_CMD::ADD, op1, op2) {}
	
	int reserve() override
	{
		if ((m_op1->type == AsmType::Register) && (m_op2->type == AsmType::Constant)) {
			Reg r;
			RegEnum reg = (RegEnum) m_op1->data;
			auto ex = r.get(reg);
			if ((ex->len == 8) && (m_op2->len == 1)) { return 4; }
		}
		return 0;
	}
	
	int write(std::ofstream& f, int offset, int va) override
	{
		offset = offset + 1; // Nothing
		va = va + 1;

		if ((m_op1->type == AsmType::Register) && (m_op2->type == AsmType::Constant)) 
		{
			Reg r;
			RegEnum reg = (RegEnum) m_op1->data;
			auto ex = r.get(reg);
			if ((ex->len == 8) && (m_op2->len == 1))
			{
				BYTE op[3] = {0x48, 0x83, 0xC0};
				if (ex->rex == REX::Ex)	{ op[0] |= 0x01; }
				op[2] |= ex->data;
				
				f.write((char*) &op, 3);
				f.write((char*) &m_op2->data, 1);
				return 4;
			}
		
		}

		return 0;
	}
};	

#endif