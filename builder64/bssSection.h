#ifndef __BSS_SECTION_H__
#define __BSS_SECTION_H__

#include "section.h"

class Variable
{
public:
	enum class Type
	{
		i32,
		u32,
	};
	
	Variable(Type t, const char* name)
	{
		m_type = t;
		if ((t == Type::i32) || (t == Type::u32)) {
			m_data = (char*) malloc( sizeof(DWORD) );
		}
		int sz = strlen(name) + 1;
		m_name = (char*)malloc(sz);
		strcpy(m_name, name);
	}
	
	~Variable()
	{
		free(m_name);
		free(m_data);
	}
	
	char* name() { return m_name; }
	char* data() { return m_data; }
	
	int offset() { return m_offset; }
	void setOffset(int offset) { m_offset = offset; }
	
	int size() {
		if ((m_type == Type::i32) || (m_type == Type::u32))
		{
			return sizeof(DWORD);
		}
		return 0;
	}
	
private:
	char* m_name;
	char* m_data;
	Type m_type;
	int m_offset;
	
};

class BssSection: public Section
{
public:
	BssSection(const char* name) : Section(name) 
	{
		m_header.Characteristics = IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;
	}
	~BssSection()
	{
		for(auto it=m_variables.begin(); it!=m_variables.end(); ++it)
		{
			delete (*it);
		}
		
	}
	
	Variable* add(Variable::Type t, const char* name)
	{
		m_variables.push_back( new Variable(t, name) );
		return m_variables.back();
	}
	
	void prepare() override
	{
		m_vaSize = 0;
		for(auto it=m_variables.begin(); it!=m_variables.end(); ++it)
		{
			(*it)->setOffset(m_vaSize);
			m_vaSize += (*it)->size();
		}
	}
	
	void recalc() override
	{		
		for(auto it=m_variables.begin(); it!=m_variables.end(); ++it)
		{			
			(*it)->setOffset( (*it)->offset() +  m_header.VirtualAddress );
		}
	}
	
private:
	std::vector<Variable*> m_variables;
};

#endif