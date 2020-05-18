#include "asmTemplate.h"
#include "asmType.h"
#include "../builder/dataSection.h"


class AsmPMov: public AsmTemplate
{
public:
	AsmPMov(Operand* op1, Operand* op2, Operand* op3)
		: AsmTemplate(ASM_CMD::PMOV, op1, op2, op3) {}
	
	int reserve() override
	{
		
		Reg r;
		RegEnum reg = (RegEnum) m_op1->data;
		auto ex = r.get(reg);
		int total = 0;
		
		if ((m_op1->type == AsmType::Register) &&
		   (m_op2->type == AsmType::Pointer))
		{					
			if (m_op3->len == 2)
				total++;
			
			if (ex->rex == REX::Ex) 
				total++;
			
			total += 3;
			total += m_op2->len;
			total += m_op3->len;
		}
		
		return total;
	}
	
	int write(std::ofstream& f, int offset)
	{
		offset = offset + 1; // Nothing
		auto start = f.tellp();
		
		Reg r;
		RegEnum reg = (RegEnum) m_op1->data;
		auto ex = r.get(reg);
			
		
		if ((m_op1->type == AsmType::Register) &&
		   (m_op2->type == AsmType::Pointer))
		{					
			BYTE op[3] = {0xC6, 0x40, 0x24};
		
			if (m_op2->len == 4)
				op[1] += 0x40;
		
			if (m_op3->len != 1)
				op[0] |= 1;
			
			op[1] |= ex->data;
			
			if (m_op3->len == 2)
			{
				char pref16 = 0x66;
				f.write((char*) &pref16, 1);
			}
			
			if (ex->rex == REX::Ex) 
			{
				char prefix = 0x41;
				f.write((char*) &prefix, 1);
			}
			
			f.write((char*) &op, 3);
			
			// Address
			f.write((char*) &m_op2->data, m_op2->len);
			
			if (m_op3->type == AsmType::Constant) {			
				f.write((char*) &m_op3->data, m_op3->len);			
			} else if (m_op3->type == AsmType::RData) {			
				Data* d = (Data*)m_op3->data;
				QWORD off = d->vaOffset();
				f.write((char*) &off, sizeof(DWORD) );				
			}
		}
		
		auto stop = f.tellp();
		return (stop-start);	
	}
};
