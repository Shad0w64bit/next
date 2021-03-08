#ifndef __DATA_H__
#define __DATA_H__

#ifdef __unix__
#include <stdint.h>
#endif

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
			int sz;
			if (name != nullptr) {
				sz = strlen(name) + 1;
				m_name = (char*) malloc(sz);
				strcpy(m_name, name);
			} else {
				m_name = nullptr;
			}
			
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
	
//	const char* name() { return m_name; }
	const char* value() { return m_value; }
	
	int offset() { return m_offset; }
	void setOffset(int offset) { m_offset = offset; }
	uint64_t vaOffset() {		
		return m_va + m_offset;
	}
	
	int size() { return m_size; }
	void setVA(uint64_t va) { m_va = va; }

private:
	Type m_type;
	char* m_name;
	char* m_value;
	
	int m_offset;
	uint64_t m_va;
	int m_size;
};


#endif
