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
			
			total += 2;
			total += m_op2->len;
			total += m_op3->len;
		} else if ((m_op1->type == AsmType::Register) &&
		   (m_op2->type == AsmType::Register) &&
		   (m_op3->type == AsmType::Constant))		
		{
			Reg r;
			RegEnum reg = (RegEnum) m_op1->data;
			auto ex1 = r.get(reg);
		
			RegEnum reg2 = (RegEnum) m_op2->data;
			auto ex2 = r.get(reg2);
						
			if (ex2->len == 4) {
				total++;				
			}
			
			if (ex1->len == 2) {
				total++;
			}
			
			if ((ex1->rex == REX::Ex) || (ex2->rex == REX::Ex)) {
				total++;
			}
			total+=2;
			total+=m_op3->len;
		}
		
		return total;
	}
	
	int write(std::ofstream& f, int offset, int va) override
	{
		offset = offset + 1; // Nothing
		va = va + 1;
		auto start = f.tellp();
		
		if ((m_op1->type == AsmType::Register) &&
			(m_op2->type == AsmType::Offset) &&
			(m_op3->type == AsmType::Constant))
		{
			Reg r;
			RegEnum reg1 = (RegEnum) m_op1->data;
			auto ex1 = r.get(reg1);
			
			if (ex1->len == 4) {
				char pref32 = 0x67;
				f.write(&pref32, 1);
			}
			
			if (ex1->len == 2) {
				char pref16 = 0x66;
				f.write(&pref16, 1);
			}
			
			BYTE prefix = 0x40;
			if (ex1->len == 8) 			{ prefix |= (1 << 3); }
			if (ex1->rex == REX::Ex)	{ prefix |= 0x01; 	  }
			
			if ((ex1->len == 8) || (ex1->rex == REX::Ex)) {
				f.write((char*) &prefix, 1);
			}
			
			BYTE op[2] = {0xC7, 0x80};
			op[1] |= ex1->data;
			
			f.write((char*) &op, 2);
			
			if ( (reg1 == RegEnum::RSP) || (reg1 == RegEnum::ESP) ||
				 (reg1 == RegEnum::R12) || (reg1 == RegEnum::R12D) ) 
			{
				char addr = 0x24;
				f.write(&addr, 1);
			}
			
			f.write((char*) &m_op2->data, 4);
			f.write((char*) &m_op3->data, 4);
			
		}
		
		if ((m_op1->type == AsmType::Register) &&
			(m_op2->type == AsmType::Offset) &&
			(m_op3->type == AsmType::Register))
		{
			Reg r;
			RegEnum reg1 = (RegEnum) m_op1->data;
			auto ex1 = r.get(reg1);
		
			RegEnum reg3 = (RegEnum) m_op3->data;
			auto ex3 = r.get(reg3);
			
			if (ex1->len == 4) {
				char pref32 = 0x67;
				f.write(&pref32, 1);
			}
			
			if (ex3->len == 2) {
				char pref16 = 0x66;
				f.write(&pref16, 1);
			}
			
			BYTE prefix = 0x40;
			if (ex3->len == 8) 			{ prefix |= (1 << 3); }
			if (ex1->rex == REX::Ex)	{ prefix |= 0x01; 	  }
			if (ex3->rex == REX::Ex)	{ prefix |= (1 << 2); }
			
			
			if ((ex3->len == 8) || (ex1->rex == REX::Ex) || (ex3->rex == REX::Ex)) {
				f.write((char*) &prefix, 1);
			}
			
			BYTE op[2] = {0x89, 0x80};
			
			op[1] |= (ex3->data << 3);
			op[1] |= ex1->data;
			
			f.write((char*) &op, 2);
			
			if ( (reg1 == RegEnum::RSP) || (reg1 == RegEnum::ESP) ||
				 (reg1 == RegEnum::R12) || (reg1 == RegEnum::R12D) ) 
			{
				char addr = 0x24;
				f.write(&addr, 1);
			}
			
			f.write((char*) &m_op2->data, 4);			
		}
		
/*		if (m_op2->type == AsmType::Constant) {
			int val = (int)m_op2->data;
			if (val < 0xFF) {
				m_op2->len = 1;
			} else if (val < 0xFFFF) {
				m_op2->len = 2;
			}
		}
		
		if (m_op3->type == AsmType::Constant) {
			int val = (int)m_op3->data;
			if (val < 0xFF) {
				m_op3->len = 1;
			} else if (val < 0xFFFF) {
				m_op3->len = 2;
			}
		}		
	*/		
		
		/*if ((m_op1->type == AsmType::Register) &&
		   (m_op2->type == AsmType::Pointer))
		{
			Reg r;
			RegEnum reg = (RegEnum) m_op1->data;
			auto ex = r.get(reg);
		
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
			
			f.write((char*) &op, 2);
			
			// Address
			f.write((char*) &m_op2->data, m_op2->len);
			
			if (m_op3->type == AsmType::Constant) {			
				f.write((char*) &m_op3->data, m_op3->len);			
			} else if (m_op3->type == AsmType::RData) {			
				Data* d = (Data*)m_op3->data;
				QWORD off = d->vaOffset();
				f.write((char*) &off, sizeof(DWORD) );				
			}
		} else*/
		if ((m_op1->type == AsmType::Register) &&
		   (m_op2->type == AsmType::Register) &&
		   (m_op3->type == AsmType::Offset))		
		{
			Reg r;
			RegEnum reg1 = (RegEnum) m_op1->data;
			auto ex1 = r.get(reg1);
		
			RegEnum reg2 = (RegEnum) m_op2->data;
			auto ex2 = r.get(reg2);
			
			{ 
				if (ex2->len == 4) {
					char d4 = 0x67;
					f.write(&d4, 1);
				}
				
				if (ex1->len == 2) {
					char pref16 = 0x66;
					f.write(&pref16, 1);
				}
			}
			{
				char prefix = 0x40; 
				
				if (ex1->len == 8) {
					prefix |= (1 << 3);
				}
				
				if (ex1->rex == REX::Ex) {
					prefix |= (1 << 2);
				}
				
				if (ex2->rex == REX::Ex) {
					prefix |= 0x01;
				}
				
				if ((ex1->rex == REX::Ex) || (ex2->rex == REX::Ex)) {
					f.write(&prefix, 1);				
				}
			}			
			{
				BYTE op[2] = {0x8A, 0x40};
				
				if (m_op3->len == 4)
				{
					op[1] = 0x80;
				}
				
				if (ex1->len > 1) {
					op[0] |= 0x01;
				}				
				
				op[1] |= ex2->data;
				op[1] |= (ex1->data << 3);
				
				f.write((char*) &op, 2);
				
				if ( (reg1 == RegEnum::RSP) || (reg1 == RegEnum::ESP) ||
				     (reg1 == RegEnum::R12) || (reg1 == RegEnum::R12D) )
				{
					char addr = 0x24;
					f.write(&addr, 1);
				}
			}
			{
				f.write((char*) &m_op3->data, 4);
			}
		}
		
		auto stop = f.tellp();
		return (stop-start);	
	}
};
