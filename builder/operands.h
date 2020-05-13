#ifndef __OPERANDS_H__
#define __OPERANDS_H__

#include "operand.h"
#include "..\variable.h"
#include "..\function.h"

class VarOperand : public Operand
{
public:
	VarOperand(Variable* v) : Operand(Operand::Type::Variable, (char*) v) {}
};

class FuncOperand : public Operand
{
public:
	FuncOperand(Function* f) : Operand(Operand::Type::Function, (char*) f) {}
};

#endif