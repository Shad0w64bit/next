#ifndef __DATA_SECTION_H__
#define __DATA_SECTION_H__

#include "section.h"

class Data
{
public:
	enum class Type
	{
		ConstString,
	};
	
	Data(Type type, const char* name, const char* value)
	{
//		m_offset = offset;
		if (type == Type::ConstString)
		{
			int sz = strlen(name) + 1;
			m_name = (char*) malloc(sz);
			strcpy(m_name, name);
			
			sz = strlen(value) + 1;
			m_value = (char*) malloc(sz);
			strcpy(m_value, value);
			m_size = sz;
		}
	}
	
	~Data()
	{
		free(m_value);
		free(m_name);
	}
	
	const char* name() { return m_name; }
	const char* value() { return m_value; }
	
	int offset() { return m_offset; }
	void setOffset(int offset) { m_offset = offset; }
	
	int size() { return m_size; }

private:
	Type m_type;
	char* m_name;
	char* m_value;
	
	int m_offset;
	int m_size;
};

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
	
	bool add(Data::Type type, const char* name, const char* val) 
	{
		for (auto it=m_data.begin(); it!=m_data.end(); ++it)
		{
			if (strcmp(name, (*it)->name()) == 0)
				return false;
		}
		m_data.push_back( new Data(type, name, val) );

//		m_rawSize += strlen(val) + 1;
		return true;
	}
	
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

private:
	std::vector<Data*> m_data;
	
};

#endif