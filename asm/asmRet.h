#ifndef __ASM_RET_H__
#define __ASM_RET_H__

#include "asmTemplate.h"
#include "asmType.h"
#include "../builder/importSection.h"

class AsmRet: public AsmTemplate
{
public:
	AsmRet()
		: AsmTemplate(ASM_CMD::RET) {}

	int reserve() override
	{
		return 1;
	}

	int write(std::ofstream& f, int offset, int va) override
	{
		offset = offset + 1; // Nothing
		va = va + 1;
		char op = 0xC3;
		f.write(&op, 1);
		return 1;
	}

};

#endif
