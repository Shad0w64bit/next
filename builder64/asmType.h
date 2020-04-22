#ifndef __ASM_TYPE_H__
#define __ASM_TYPE_H__

#include <map>

enum class Type
{
	Pointer,
	Reg,
	i32,
};

enum REX {
	None,
	Ex,
};

struct REG {
	REX rex;
	int len;
	int data;
};

enum RegEnum {

	// Byte Register
	AL,
	CL,
	DL,
	BL,
	SPL,
	BPL,
	SIL,
	DIL,
	R8L,
	R9L,
	R10L,
	R11L,
	R12L,
	R13L,
	R14L,
	R15L,
	
// Word Register

	AX,		
	CX,		
	DX,		
	BX,		
	SP,		
	BP,		
	SI,		
	DI,		
	R8W,	
	R9W,	
	R10W,	
	R11W,	
	R12W,	
	R13W,	
	R14W,	
	R15W,	

// Doubleword Register

	EAX,	
	ECX,	
	EDX,	
	EBX,	
	ESP,	
	EBP,	
	ESI,	
	EDI,	
	R8D,	
	R9D,	
	R10D,	
	R11D,	
	R12D,	
	R13D,	
	R14D,	
	R15D,	

// Quadword Register

	RAX,	
	RCX,	
	RDX,	
	RBX,	
	RSP,	
	RBP,	
	RSI,	
	RDI,	
	R8,		
	R9,		
	R10,	
	R11,	
	R12,	
	R13,	
	R14,	
	R15,	

};

class Reg {
public:
	
	REG* get(RegEnum r) 
	{
		auto res = registry.find(r);
		if (res != registry.end())
			return &((*res).second);
		return nullptr;
	}
	
private:
	std::map<RegEnum, REG> registry = {
		
		// Byte Register
		{RegEnum::AL, 	{REX::None, 1, 0}},
		{RegEnum::CL, 	{REX::None, 1, 1}},
		{RegEnum::DL, 	{REX::None, 1, 2}},
		{RegEnum::BL, 	{REX::None, 1, 3}},
		{RegEnum::SPL, 	{REX::None, 1, 4}},
		{RegEnum::BPL, 	{REX::None, 1, 5}},
		{RegEnum::SIL, 	{REX::None, 1, 6}},
		{RegEnum::DIL, 	{REX::None, 1, 7}},
		{RegEnum::R8L, 	{REX::Ex, 	1, 0}},
		{RegEnum::R9L, 	{REX::Ex, 	1, 1}},
		{RegEnum::R10L, {REX::Ex, 	1, 2}},
		{RegEnum::R11L, {REX::Ex, 	1, 3}},
		{RegEnum::R12L, {REX::Ex, 	1, 4}},
		{RegEnum::R13L, {REX::Ex, 	1, 5}},
		{RegEnum::R14L, {REX::Ex, 	1, 6}},
		{RegEnum::R15L, {REX::Ex, 	1, 7}},
		
		// Word Register
		{RegEnum::AX, 	{REX::None, 2, 0}},
		{RegEnum::CX, 	{REX::None, 2, 1}},
		{RegEnum::DX, 	{REX::None, 2, 2}},
		{RegEnum::BX, 	{REX::None, 2, 3}},
		{RegEnum::SP, 	{REX::None, 2, 4}},
		{RegEnum::BP, 	{REX::None, 2, 5}},
		{RegEnum::SI, 	{REX::None, 2, 6}},
		{RegEnum::DI, 	{REX::None, 2, 7}},
		{RegEnum::R8W, 	{REX::Ex, 	2, 0}},
		{RegEnum::R9W, 	{REX::Ex, 	2, 1}},
		{RegEnum::R10W, {REX::Ex, 	2, 2}},
		{RegEnum::R11W, {REX::Ex, 	2, 3}},
		{RegEnum::R12W, {REX::Ex, 	2, 4}},
		{RegEnum::R13W, {REX::Ex, 	2, 5}},
		{RegEnum::R14W, {REX::Ex, 	2, 6}},
		{RegEnum::R15W, {REX::Ex, 	2, 7}},
		
		// Doubleword Register
		{RegEnum::EAX,	{REX::None,	4, 0}},
		{RegEnum::ECX,	{REX::None,	4, 1}},
		{RegEnum::EDX,	{REX::None,	4, 2}},
		{RegEnum::EBX, 	{REX::None,	4, 3}},
		{RegEnum::ESP, 	{REX::None,	4, 4}},
		{RegEnum::EBP, 	{REX::None,	4, 5}},
		{RegEnum::ESI, 	{REX::None,	4, 6}},
		{RegEnum::EDI, 	{REX::None,	4, 7}},
		{RegEnum::R8D, 	{REX::Ex, 	4, 0}},
		{RegEnum::R9D, 	{REX::Ex, 	4, 1}},
		{RegEnum::R10D, {REX::Ex, 	4, 2}},
		{RegEnum::R11D, {REX::Ex, 	4, 3}},
		{RegEnum::R12D, {REX::Ex, 	4, 4}},
		{RegEnum::R13D, {REX::Ex, 	4, 5}},
		{RegEnum::R14D, {REX::Ex, 	4, 6}},
		{RegEnum::R15D, {REX::Ex, 	4, 7}},
		

		
		// Quadword Register
		{RegEnum::RAX, 	{REX::None,	8, 0}},
		{RegEnum::RCX, 	{REX::None,	8, 1}},
		{RegEnum::RDX, 	{REX::None,	8, 2}},
		{RegEnum::RBX, 	{REX::None,	8, 3}},
		{RegEnum::RSP, 	{REX::None,	8, 4}},
		{RegEnum::RBP, 	{REX::None,	8, 5}},
		{RegEnum::RSI, 	{REX::None,	8, 6}},
		{RegEnum::RDI, 	{REX::None,	8, 7}},
		{RegEnum::R8,	{REX::Ex, 	8, 0}},
		{RegEnum::R9,	{REX::Ex, 	8, 1}},
		{RegEnum::R10,	{REX::Ex, 	8, 2}},
		{RegEnum::R11,	{REX::Ex, 	8, 3}},
		{RegEnum::R12,	{REX::Ex, 	8, 4}},
		{RegEnum::R13,	{REX::Ex, 	8, 5}},
		{RegEnum::R14,	{REX::Ex, 	8, 6}},
		{RegEnum::R15,	{REX::Ex, 	8, 7}},
		
	};

};

struct Operand
{
	Type type;
	int len;
	union {
		char* data;
		DWORD val;
	};
};

enum class ASM
{
	Mov,
	Call,
	Sub,
	Add,
};

#endif