#ifndef __ASM_MOV_H__
#define __ASM_MOV_H__

#include "asmTemplate.h"
#include "asmType.h"
#include "../builder/dataSection.h"

class AsmMov: public AsmTemplate
{
public:
	AsmMov(Operand* op1, Operand* op2)
		: AsmTemplate(ASM_CMD::MOV, op1, op2) {}
	
	// Length of command
	int reserve() override {
		// Variable < Constant
		if ((m_op1->type == AsmType::Bss) && (m_op2->type == AsmType::Constant))		  
		{
			//auto v = (Variable*) m_op1->data;
			//auto v = (Data*) m_op1->data;
			return 2 + ((m_op2->len == 2) ? 1 : 0)
				+ sizeof(DWORD) + m_op2->len;
			
		}
		
		// Register < Constant
		// Register < Variable
		// Register < RData
		if (m_op1->type == AsmType::Register)
		{
			Reg r;
			RegEnum reg = (RegEnum) m_op1->data;
			auto ex = r.get(reg);
			if (ex->len == 8) {
				return 2 + ((m_op2->len != 8) ? 1 : 0) + m_op2->len;
			} else {
				int special = (((ex->len == 1) && ((reg == RegEnum::SPL) || (reg == RegEnum::BPL) || (reg == RegEnum::SIL) || (reg == RegEnum::DIL))) ? 1 : 0);
				int word = ((ex->len == 2) ? 1 : 0);
				int ext = ((ex->rex == REX::Ex) ? 1 : 0);
				return 1 + special + word + ext;
			}
		}
/*		if ((m_op1->type == Type::Register) && (m_op2->type == Type::Variable))
		{
			Reg r;
			RegEnum reg = (RegEnum) m_op1->data;
			auto ex = r.get(reg);
			//auto var = (Variable*) m_op2->data;
			
			
			
		}
		
		if ((m_op1->type == Type::Register) && (m_op2->type == Type::RData))
		{
			Reg r;
			RegEnum reg = (RegEnum) m_op1->data;
			auto ex = r.get(reg);
			
		}
		*/
		return 0;		
	}
	
	// Return count of write bytes
	int write(std::ofstream& f, int offset)
	{
		auto start = f.tellp();
		if ((m_op1->type == AsmType::Bss) && (m_op2->type == AsmType::Constant))		
		{
			auto var = (Variable*) m_op1->data;
			// (offset + reserve())
			int vs = var->size();
			if (vs == 1) { f.write("\xC6\x05", 2); } else 
			if (vs == 2) { f.write("\x66\xC7\x05", 3); } else
			if (vs == 4) { f.write("\xC7\x05", 2); }
			
//			std::cout << reserve() << std::endl;
			int cmd_end = offset + reserve();
			int var_offset = var->offset() - cmd_end;
			f.write( (char*) &var_offset, sizeof(DWORD) );
			
			if (vs == 1) { f.write( (char*) &m_op2->data, 1); } else 
			if (vs == 2) { f.write( (char*) &m_op2->data, 2); } else 
			if (vs == 4) { f.write( (char*) &m_op2->data, 4); }
		}
		
		if (m_op1->type == AsmType::Register)
		{
			Reg r;
			RegEnum reg = (RegEnum) m_op1->data;
			auto ex = r.get(reg);
			
			if (m_op2->type == AsmType::Constant)
			{
				
				
				if (m_op2->len > 4)
				{
					BYTE op[2] = {0x48, 0xB8};
					
					if (ex->rex == REX::Ex)	{ op[0] |= 0x01; }
					op[1] |= ex->data;
					
					f.write((char*)&op, 2);					
					f.write((char*) &m_op2->data, sizeof(QWORD) );
				} else {
					BYTE op[3] = {0x48, 0xC7, 0xC0};
					
					if (ex->rex == REX::Ex)	{ op[0] |= 0x01; }
					op[2] |= ex->data;
					DWORD i = m_op2->data;
//					std::cout << "Core: " << i << std::endl;
					f.write((char*)&op, 3);
					f.write((char*) &i, sizeof(DWORD) );
				}				
			} 
			else if (m_op2->type == AsmType::RData)
			{
				Data* d = (Data*)m_op2->data;
				QWORD off = d->vaOffset();
				if (off > 0xFFFFFFFF)
				{
					BYTE op[2] = {0x48, 0xBA};
					if (ex->rex == REX::Ex)	{ op[0] |= 0x01; }
					op[1] |= ex->data;
					
					f.write((char*)&op, 3);
					f.write((char*) &off, sizeof(QWORD) );					
				} else {				
					BYTE op[3] = {0x48, 0xC7, 0xC0};
					if (ex->rex == REX::Ex)	{ op[0] |= 0x01; }
					op[2] |= ex->data;
					
					f.write((char*)&op, 3);
					f.write((char*) &off, sizeof(DWORD) );
				}
			}
			else if (m_op2->type == AsmType::Bss)
			{
				if (ex->len == 8) {
					if (ex->rex == REX::Ex) {
						f.write("\x44", 1);
					}
					BYTE op[2] = {0x8B, 0x05};
					op[1] |= (ex->data << 3);
					
					f.write((char*)&op, 2);
					
					auto var = (Variable*) m_op2->data;
					int cmd_end = offset + reserve();
					int var_offset = var->offset() - cmd_end;
					f.write( (char*) &var_offset, sizeof(DWORD) );
				}
				
				
			}
		}
		
		auto stop = f.tellp();
		return (stop-start);	
	}

protected:
	
};

#endif