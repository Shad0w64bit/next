#ifndef __ASM_MOV_H__
#define __ASM_MOV_H__

#ifdef __unix__
#include <stdint.h>
#endif

#include "asmTemplate.h"
#include "asmType.h"
#include "../builder/dataSection.h"

class AsmMov: public AsmTemplate
{
public:
	AsmMov(Operand* op1, Operand* op2)
		: AsmTemplate(ASM_CMD::MOV, op1, op2) {}
	
	// Length of command
	int reserve() override 
	{
		int	total = 0;
		if ((m_op1->type == AsmType::Bss) && (m_op2->type == AsmType::Constant))		
		{
			auto var = (Variable*) m_op1->data;
			// (offset + reserve())
			int vs = var->size();
			if (vs == 1) { total = 2; } else 
			if (vs == 2) { total = 3; } else
			if (vs == 4) { total = 2; }
			
//			std::cout << reserve() << std::endl;
			total += sizeof(uint32_t);
			
			if (vs == 1) { total += 1; } else 
			if (vs == 2) { total += 2; } else 
			if (vs == 4) { total += 4; }
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
					total = 2 + sizeof(uint64_t);
				} else {					
					total = 3 + sizeof(uint32_t);
				}				
			} 
			else if (m_op2->type == AsmType::RData)
			{				
				Data* d = (Data*)m_op2->data;
				uint64_t off = d->vaOffset();
				if (off > 0xFFFFFFFF)
				{
					total = 3 + sizeof(uint64_t);
				} else {				
					total = 3 + sizeof(uint32_t);
				}
			}
			else if (m_op2->type == AsmType::Bss)
			{
				if (ex->len == 8) {
					if (ex->rex == REX::Ex) {
						total++;
					}
					total += 2 + sizeof(uint32_t);					
				}
			} 
			else if (m_op2->type == AsmType::Register)
			{
				Reg r;
				RegEnum reg1 = (RegEnum) m_op1->data;
				auto ex1 = r.get(reg1);
				RegEnum reg2 = (RegEnum) m_op2->data;
				auto ex2 = r.get(reg2);
					
				// Prefix 16-bit
				if (ex1->len == 2)
					total += 1;
				
				// Prefix for Extra Registers
				if ( !((ex1->rex == REX::None) && (ex2->rex == REX::None))
					|| (ex1->len == 8)
					|| (ex1->len == 1 && (ex1->data == RegEnum::SPL || ex1->data == RegEnum::BPL || ex1->data == RegEnum::DIL || ex1->data == RegEnum::SIL )) 
					|| (ex2->len == 1 && (ex2->data == RegEnum::SPL || ex2->data == RegEnum::BPL || ex2->data == RegEnum::DIL || ex2->data == RegEnum::SIL ))
				)
					total += 1;
				
				// Some opcode
				total += 2;
			}
			return total;
		}
		
		
		
		/*
		// Variable < Constant
		if ((m_op1->type == AsmType::Bss) && (m_op2->type == AsmType::Constant))		  
		{
			//auto v = (Variable*) m_op1->data;
			//auto v = (Data*) m_op1->data;
			return 2 + ((m_op2->len == 2) ? 1 : 0)
				+ sizeof(uint32_t) + m_op2->len;
			
		}
		
		// Register < Register
		if ((m_op1->type == AsmType::Register) && (m_op2->type == AsmType::Register))
		{
			Reg r;
			RegEnum reg1 = (RegEnum) m_op1->data;
			auto ex1 = r.get(reg1);
			RegEnum reg2 = (RegEnum) m_op2->data;
			auto ex2 = r.get(reg2);
			
			int total = 0;
			
			if (ex1->len == 2)
				total++;
			
			if ( !((ex1->rex == REX::None) && (ex2->rex == REX::None))
					|| (ex1->len == 8)
					|| (ex1->len == 1 && (ex1->data == RegEnum::SPL || ex1->data == RegEnum::BPL || ex1->data == RegEnum::DIL || ex1->data == RegEnum::SIL )) 
					|| (ex2->len == 1 && (ex2->data == RegEnum::SPL || ex2->data == RegEnum::BPL || ex2->data == RegEnum::DIL || ex2->data == RegEnum::SIL ))
				)
				total++;
							
			return total + 2;
		}
		
		// Register < Constant
		if ((m_op1->type == AsmType::Register) && (m_op1->type == AsmType::Constant))
		{			
			if (m_op2->len > 4)
			{
				return 2 + sizeof(uint64_t);				
			} else {
				return 3 + sizeof(uint32_t);
			}				
		}
		
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
				int res =  1 + special + word + ext + m_op2->len;
				std::cout << "Special " << res << std::endl;
				return res;
			}
		}*/
		
		return 0;		
	}
	
	// Return count of write bytes
	int write(std::ofstream& f, int offset, int va) override
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
			int cmd_end = offset + reserve() + va;
			int var_offset = var->offset() - cmd_end;
			f.write( (char*) &var_offset, sizeof(uint32_t) );
			
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
					uint8_t op[2] = {0x48, 0xB8};
					
					if (ex->rex == REX::Ex)	{ op[0] |= 0x01; }
					op[1] |= ex->data;
					
					f.write((char*)&op, 2);					
					f.write((char*) &m_op2->data, sizeof(uint64_t) );
				} else {
					uint8_t op[3] = {0x48, 0xC7, 0xC0};
					
					if (ex->rex == REX::Ex)	{ op[0] |= 0x01; }
					op[2] |= ex->data;
					uint32_t i = m_op2->data;
//					std::cout << "Core: " << i << std::endl;
					f.write((char*)&op, 3);
					f.write((char*) &i, sizeof(uint32_t) );
				}				
			} 
			else if (m_op2->type == AsmType::RData)
			{
				Data* d = (Data*)m_op2->data;
				uint64_t off = d->vaOffset();
				if (off > 0xFFFFFFFF)
				{
					uint8_t op[2] = {0x48, 0xBA};
					if (ex->rex == REX::Ex)	{ op[0] |= 0x01; }
					op[1] |= ex->data;
					
					f.write((char*)&op, 3);
					f.write((char*) &off, sizeof(uint64_t) );					
				} else {				
					uint8_t op[3] = {0x48, 0xC7, 0xC0};
					if (ex->rex == REX::Ex)	{ op[0] |= 0x01; }
					op[2] |= ex->data;
					
					f.write((char*)&op, 3);
					f.write((char*) &off, sizeof(uint32_t) );
				}
			}
			else if (m_op2->type == AsmType::Bss)
			{
				if (ex->len == 8) {
					if (ex->rex == REX::Ex) {
						f.write("\x44", 1);
					}
					uint8_t op[2] = {0x8B, 0x05};
					op[1] |= (ex->data << 3);
					
					f.write((char*)&op, 2);
					
					auto var = (Variable*) m_op2->data;
					int cmd_end = offset + reserve() + va;
					int var_offset = var->offset() - cmd_end;
					f.write( (char*) &var_offset, sizeof(uint32_t) );
				}
				
				
			} 
			else if (m_op2->type == AsmType::Register)
			{
				Reg r;
				RegEnum reg1 = (RegEnum) m_op1->data;
				auto ex1 = r.get(reg1);
				RegEnum reg2 = (RegEnum) m_op2->data;
				auto ex2 = r.get(reg2);
					
				char pref16 = 0x66;
				char prefix = (ex1->len == 8) ? 0x48 : 0x40;
				uint8_t op[2] = {0x88, 0xC0};					
				
				op[1] |= ex1->data;
				if (ex1->rex == REX::Ex)
					prefix |= 1;
				if (ex1->len != 1)
					op[0] |= 1;
				
					
				
				op[1] |= (ex2->data << 3);
				if (ex2->rex == REX::Ex) 
					prefix |= (1 << 2);
				
				// Prefix 16-bit
				if (ex1->len == 2)
					f.write(&pref16, 1);
				
				// Prefix for Extra Registers
				if ( !((ex1->rex == REX::None) && (ex2->rex == REX::None))
					|| (ex1->len == 8)
					|| (ex1->len == 1 && (ex1->data == RegEnum::SPL || ex1->data == RegEnum::BPL || ex1->data == RegEnum::DIL || ex1->data == RegEnum::SIL )) 
					|| (ex2->len == 1 && (ex2->data == RegEnum::SPL || ex2->data == RegEnum::BPL || ex2->data == RegEnum::DIL || ex2->data == RegEnum::SIL ))
				)
					f.write(&prefix, 1);
				
				// Some opcode
				f.write((char*) &op, 2);		
			}
		}
		
		auto stop = f.tellp();
		return (stop-start);	
	}

protected:
	
};

#endif
