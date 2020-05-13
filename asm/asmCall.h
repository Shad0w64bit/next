#ifndef __ASM_CALL_H__
#define __ASM_CALL_H__

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
		return 6;
	}
	
	int write(std::ofstream& f, int offset)
	{
		BYTE op[2] = {0xFF, 0x15};
		f.write((char*) &op, 2);
		
		if (m_op1->type == AsmType::Library) {
			ImportFunc* lib = (ImportFunc*) m_op1->data;
			
			//std::cout << "Cmd: " << offset << "\nReserve: " << reserve() << "\nOffset: " << lib->offsetIAT << std::endl;
			
			
			int cmd_end = offset + reserve();
			int lib_offset = lib->offsetIAT - cmd_end;
			f.write( (char*) &lib_offset, sizeof(DWORD) );
		}

		return 6;
	}
};	

#endif