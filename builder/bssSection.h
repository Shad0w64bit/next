#ifndef __BSS_SECTION_H__
#define __BSS_SECTION_H__

#include "section.h"
#include "../core/variable.h"

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
	
	Variable* add(Type* t, const char* name)
	{
		m_variables.push_back( new Variable(name, t) );
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
	
	void preLoad(IMAGE_NT_HEADERS64* header) override
	{
		header->OptionalHeader.SizeOfUninitializedData = this->vaSize();
	}
	
private:
	std::vector<Variable*> m_variables;
};

#endif
