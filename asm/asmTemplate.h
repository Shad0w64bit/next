#ifndef __ASM_TEMPLATE_H__
#define __ASM_TEMPLATE_H__

#include "asmType.h"

class AsmTemplate
{
public:
//	AsmTemplate() {}
	AsmTemplate(ASM_CMD cmd, Operand* op1 = nullptr, Operand* op2 = nullptr, Operand* op3 = nullptr, Operand* op4 = nullptr)
		: m_cmd(cmd), m_op1(op1), m_op2(op2), m_op3(op3), m_op4(op4) {}
	virtual ~AsmTemplate() {
		if (m_op1 != nullptr) delete m_op1;
		if (m_op2 != nullptr) delete m_op2;
		if (m_op3 != nullptr) delete m_op3;
		if (m_op4 != nullptr) delete m_op4;
	}
	
	virtual int reserve() { return 0; }
	virtual int write(std::ofstream& f, int offset) 
	{ 
		offset = offset + 1;	// Something
		f.tellp();				// Nothing
		return 0; 
	}
	
	ASM_CMD cmd() { return m_cmd; }
	
protected:
	ASM_CMD	 m_cmd;
	Operand* m_op1;
	Operand* m_op2;
	Operand* m_op3;
	Operand* m_op4;
};

#endif