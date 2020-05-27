#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "..\asm\asmType.h"
#include "..\asm\asmTemplate.h"

#include "..\asm\asmMov.h"
#include "..\asm\asmPMov.h"
#include "..\asm\asmAdd.h"
#include "..\asm\asmSub.h"
#include "..\asm\asmCall.h"
#include "..\asm\asmCallPtr.h"
#include "..\asm\asmNop.h"
#include "..\asm\asmPush.h"
#include "..\asm\asmPop.h"
#include "..\asm\asmRet.h"
#include "..\asm\asmLea.h"


class AsmCommand
{
public:	
	AsmCommand(ASM_CMD cmd, Operand* op1 = nullptr, Operand* op2 = nullptr, Operand* op3 = nullptr, Operand* op4 = nullptr) {
		if (cmd == ASM_CMD::MOV) {
			m_cmd = new AsmMov(op1, op2);
		} else if (cmd == ASM_CMD::PMOV) {
			m_cmd = new AsmPMov(op1, op2, op3);
		} else if (cmd == ASM_CMD::NOP) {		
			m_cmd = (op1 == nullptr) ? (new AsmNop()) : (new AsmNop(op1));
		} else if (cmd == ASM_CMD::SUB) {		
			m_cmd = new AsmSub(op1, op2);
		} else if (cmd == ASM_CMD::ADD) {		
			m_cmd = new AsmAdd(op1, op2);
		} else if (cmd == ASM_CMD::CALLPTR) {		
			m_cmd = new AsmCallPtr(op1);
		} else if (cmd == ASM_CMD::CALL) {		
			m_cmd = new AsmCall(op1);
		} else if (cmd == ASM_CMD::PUSH) {		
			m_cmd = new AsmPush(op1);
		} else if (cmd == ASM_CMD::POP) {		
			m_cmd = new AsmPop(op1);
		} else if (cmd == ASM_CMD::RET) {		
			m_cmd = new AsmRet();
		} else if (cmd == ASM_CMD::LEA) {
			m_cmd = new AsmLea(op1, op2);
		} else {
			op1 = nullptr;
			op2 = op1;
			op3 = op2;
			op4 = op3;
			op1 = op4;
			m_cmd = nullptr;			
		}
	}

	~AsmCommand()
	{
		if (m_cmd != nullptr) 
			delete m_cmd;

	}
	
	int size()
	{		
		int res = 0;
		if (m_cmd != nullptr)
		{
			std::ofstream f ("file.tmp", std::ios::out | std::ios::binary);
			res = m_cmd->write(f, 1000, 1000);
			f.close();
		}
		
		return res;
		/*return (m_cmd != nullptr)
			? m_cmd->reserve()
			: 0;*/
	}
	
	int write(std::ofstream& f, int offset, int va)
	{
		return (m_cmd != nullptr)
			? m_cmd->write(f, offset, va)
			: 0;
	}
	
	ASM_CMD cmd()
	{
		return m_cmd->cmd();
	}
	
	
protected:
	AsmTemplate* m_cmd;
};

#endif