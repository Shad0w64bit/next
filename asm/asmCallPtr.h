#ifndef __ASM_CALL_PTR_H__
#define __ASM_CALL_PTR_H__

#ifdef __unix__
#include <stdint.h>
#endif

#include "asmTemplate.h"
#include "asmType.h"
#include "../builder/importSection.h"

class AsmCallPtr: public AsmTemplate
{
public:
	AsmCallPtr(Operand* op1)
		: AsmTemplate(ASM_CMD::CALLPTR, op1) {}
	
	int reserve() override
	{
		return 6;
	}
	
	int write(std::ofstream& f, int offset, int va) override
	{
		uint8_t op[2] = {0xFF, 0x15};
		f.write((char*) &op, 2);
		
		if (m_op1->type == AsmType::Library) {
			ImportFunc* lib = (ImportFunc*) m_op1->data;
			
			//std::cout << "Cmd: " << offset << "\nReserve: " << reserve() << "\nOffset: " << lib->offsetIAT << std::endl;
			
			
			int cmd_end = va + offset + reserve();
			int lib_offset = lib->offsetIAT - cmd_end;
			f.write( (char*) &lib_offset, sizeof(uint32_t) );
		} 
		
		return 6;
	}
};	

#endif
