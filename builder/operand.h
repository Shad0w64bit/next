#ifndef __OPERAND_H__
#define __OPERAND_H__

class Operand
{
public:
	enum Type 
	{
		Variable,
		Function,
	};
	Operand(Type t, char* data)
		: m_type(t), m_data(data) {}

private:
	Type m_type;
	char* m_data;
	
};

#endif