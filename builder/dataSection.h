#ifndef __DATA_SECTION_H__
#define __DATA_SECTION_H__

#include "section.h"
#include "..\core\data.h"

class DataSection: public Section
{
public:
	DataSection(const char* name) : Section(name) 
	{ 
		m_header.Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;
	}
	
	~DataSection()
	{
		for (auto it=m_data.begin(); it!=m_data.end(); ++it)
		{
			delete (*it);
		}
		m_data.clear();
	}
	
	Data* add(Data::Type type, const char* val) 
	{
		m_data.push_back( new Data(type, nullptr, val) );
		return m_data.back();
	}
	
/*	Data* add(Data::Type type, const char* name, const char* val) 
	{
		for (auto it=m_data.begin(); it!=m_data.end(); ++it)
		{
			if (strcmp(name, (*it)->name()) == 0)
				return nullptr;
		}
		m_data.push_back( new Data(type, name, val) );

//		m_rawSize += strlen(val) + 1;
		return m_data.back();
	}*/
	
	void prepare() override
	{
		m_rawSize = m_vaSize = 0;
		for (auto it=m_data.begin(); it!=m_data.end(); ++it)
		{			
			(*it)->setOffset(m_rawSize);
			m_rawSize += (*it)->size();
			m_vaSize += (*it)->size();
		}
		
	}
	
	void recalc() override
	{
		for (auto it=m_data.begin(); it!=m_data.end(); ++it)
		{
			(*it)->setVA(m_header.VirtualAddress + m_imageBase);
		}
	}
	
	void write(std::ofstream& f, int align) override
	{
		for (auto it=m_data.begin(); it!=m_data.end(); ++it)
		{
			f.write( (*it)->value(), (*it)->size() );
		}
		
		{			
			int sz = ALIGN_UP(m_rawSize, align) - m_rawSize;
			char* tmp = (char*)malloc(sz);
			memset(tmp, 0, sz);
			f.write(tmp, sz);
		}
		
	}
	
	void preLoad(IMAGE_NT_HEADERS64* header) override
	{
		header->OptionalHeader.SizeOfInitializedData = this->vaSize();
	}

private:
	std::vector<Data*> m_data;
	
};

#endif