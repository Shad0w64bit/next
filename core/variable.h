#ifndef __VARIABLE_H__
#define __VARIABLE_H__

#include "windows.h"
#include "typedef.h"

class Variable
{
public:
	Variable(const char* name, Type* t)
	{
		m_type = t;
		m_size = 0;
		m_data = ((t != nullptr) && (t->size > 0)) ? (char*) malloc(t->size) : nullptr;
		
		if (name != nullptr) {
			int sz = strlen(name) + 1;
			m_name = (char*)malloc(sz);
			strcpy(m_name, name);
		} else {
			m_name = nullptr;
		}
	}
	
	~Variable()
	{
		if (m_name == nullptr)
			free(m_name);
		if (m_data != nullptr)
			free(m_data);
	}
	
	char* name() { return m_name; }
	char* data() { return m_data; }
	
	void setData(const char* data)
	{
		if (m_type->size == -1) {
			std::cout << "setData(data) only for strong length" << std::endl;
			return;
		}
//		if (m_data == nullptr) std::cout << "ALERT!!!!";

		memcpy(m_data, data, m_type->size);
	}
	
	void setData(const char* data, int sz)
	{
		if (m_type->size != -1) {
			std::cout << "setData(data, sz) only for anything length" << std::endl;
			return;
		}
		
		if (m_data != nullptr) { free(m_data); }
		
		m_data = (char*)malloc(sz);
		memcpy(m_data, data, sz);
		m_size = sz;
//		std::cout << "Variable: " << m_data << std::endl;
	}
	
	int offset() { return m_offset; }
	void setOffset(int offset) { m_offset = offset; }
	
	Type* type() { return m_type; }
	
	int size() {
		return (m_type->size > 0) ? m_type->size : m_size;
	}
	
private:
	char* m_name;
	char* m_data;
	int m_size;
	Type* m_type;
	int m_offset;
	
};

/*Variable::Type getTypeByName(const char* name)
{
	if (strcmp(name, "i32") == 0) { return Variable::Type::i32; }
	else if (strcmp(name, "u32") == 0) { return Variable::Type::u32; }
	else if (strcmp(name, "i16") == 0) { return Variable::Type::i16; }
	else if (strcmp(name, "u16") == 0) { return Variable::Type::u16; }
	else if (strcmp(name, "i8") == 0) { return Variable::Type::i8; }
	else if (strcmp(name, "u8") == 0) { return Variable::Type::u8; }
	
	return Variable::Type::Unknown;
}*/

#endif