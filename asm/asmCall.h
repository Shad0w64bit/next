#ifndef __ASM_CALL_H__
#define __ASM_CALL_H__

#ifdef __unix__
#include <stdint.h>
#elif _WIN32
#endif


#include "asmTemplate.h"
#include "asmType.h"
#include "../builder/importSection.h"

class AsmCall: public AsmTemplate
{
public:
	AsmCall(Operand* op1)
		: AsmTemplate(ASM_CMD::CALL, op1) {}
	
	int reserve() override
	{
		return 5;
	}
	
	int write(std::ofstream& f, int offset, int va) override
	{		
		va = va + 1;
		char op = 0xE8;
		f.write(&op, 1);
		
		/*if (m_op1->type == AsmType::Pointer) 
		{
			f.write( (char*) &m_op1->data, sizeof(DWORD) );
		} 
		else */
		if (m_op1->type == AsmType::Function) 
		{
			Function* func = (Function*) m_op1->data;
			
			int cmd_end = offset + reserve();
			int lib_offset = func->address() - cmd_end;
			f.write( (char*) &lib_offset, sizeof(uint32_t) );
		}

		return 5;
	}
};	

#endif
