#ifndef __ASM_SUB_H__
#define __ASM_SUB_H__

#include "asmTemplate.h"
#include "asmType.h"

class AsmSub: public AsmTemplate
{
public:
	AsmSub(Operand* op1, Operand* op2)
		: AsmTemplate(ASM_CMD::SUB, op1, op2) {}
	
	int reserve() override
	{
		if ((m_op1->type == Type::Register) && (m_op2->type == Type::Constant)) {
			Reg r;
			RegEnum reg = (RegEnum) m_op1->data;
			auto ex = r.get(reg);
			if ((ex->len == 8) && (m_op2->len == 1)) { return 4; }
		}
		return 0;
	}
	
	int write(std::ofstream& f, int offset)
	{
		offset = offset + 1; // Nothing

		if ((m_op1->type == Type::Register) && (m_op2->type == Type::Constant)) 
		{
			Reg r;
			RegEnum reg = (RegEnum) m_op1->data;
			auto ex = r.get(reg);
			if ((ex->len == 8) && (m_op2->len == 1))
			{
				BYTE op[3] = {0x48, 0x83, 0xE8};
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