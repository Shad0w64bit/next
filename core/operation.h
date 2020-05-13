#ifndef __OPERATION_H__
#define __OPERATION_H__

#include "codeblock.h"

class Operation
{
public:
	enum Type
	{
		CodeBlock,
		CallFunction,
		Assign,
		Add,
		Sub,
		Mul,
		Div,
	};
	
	Operation(Type t, char* op1 = nullptr, char* op2 = nullptr, char* op3 = nullptr, char* op4 = nullptr)
		: m_type(t), m_op1(op1), m_op2(op2), m_op3(op3), m_op4(op4)  {}
	
	~Operation() {}
	
	Type type() { return m_type; }
	

protected:
	Type m_type;
	char* m_op1;
	char* m_op2;
	char* m_op3;
	char* m_op4;
	
};

#endif