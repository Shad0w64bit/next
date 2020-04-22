#ifndef __CODE_SECTION_H__
#define __CODE_SECTION_H__

#include "section.h"
#include "command.h"

class CodeSection: public Section
{
public:
	CodeSection(const char* name) : Section(name) {
		m_header.Characteristics = IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ;
		
	}
	~CodeSection() {
		for (auto it=m_commands.begin(); it!=m_commands.end(); ++it)
		{
			delete (*it);
		}
		m_commands.clear();
	}
	
	void add(Command* cmd)
	{
		m_rawSize += cmd->size();
		m_vaSize += cmd->size();
		m_commands.push_back(cmd);
	}
	
	void prepare() override
	{
		m_rawSize = 0;
		m_vaSize = 0;
		for (auto it=m_commands.begin(); it!=m_commands.end(); ++it)
		{
			m_rawSize += (*it)->size();
			m_vaSize += (*it)->size();
		}
	}
	
	void write(std::ofstream& f, int align) override
	{
		int offset = 0;
		for (auto it=m_commands.begin(); it!=m_commands.end(); ++it)
		{
			(*it)->write(f, m_header.VirtualAddress + offset);
			offset += (*it)->size();
		}
		
		{			
			int sz = ALIGN_UP(m_rawSize, align) - m_rawSize;
			char* tmp = (char*)malloc(sz);
			memset(tmp, 0, sz);
			f.write(tmp, sz);
		}
	}
	

private:
	std::vector<Command*> m_commands;
	
};

#endif