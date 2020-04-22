#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "asmType.h"

class Command
{
public:
	enum class Cmd
	{
		MOV,
		CALL,
		SUB,
		ADD,
	};
	
	Command(Cmd cmd, Operand* op1 = nullptr, Operand* op2 = nullptr): m_cmd(cmd), m_op1(op1), m_op2(op2) {}

	~Command()
	{
		if (m_op1 != nullptr)
			delete m_op1;
		if (m_op2 != nullptr)
			delete m_op2;
	}
	
	int size()
	{		
		if (m_cmd == Cmd::MOV) {
			if ((m_op1->type == Type::Pointer) && (m_op2->type == Type::i32))
				return 2 + 4 + 4;
			else if ((m_op1->type == Type::Reg) && (m_op2->type == Type::Pointer))
				return 2 + 1 + 4;
		}		
		return 0;
	}
	
	void write(std::ofstream& f, int offset)
	{
		if (m_cmd == Cmd::MOV) {
			if ((m_op1->type == Type::Pointer) && (m_op2->type == Type::i32))
			{
				f.write("\xC7\x05", 2);
				DWORD o1 = ((Variable*)m_op1->data)->offset() - (offset + size());
				f.write((char*) &o1, sizeof(o1));
				f.write((char*) &m_op2->data, m_op2->len);
			}
		}
	}
	
private:
	Cmd m_cmd;
	Operand* m_op1;
	Operand* m_op2;
};

#endif