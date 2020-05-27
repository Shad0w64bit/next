#ifndef __CODE_SECTION_H__
#define __CODE_SECTION_H__

#include "section.h"
#include "asmCommand.h"

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
	
	void add(AsmCommand* cmd)
	{
		int sz = cmd->size();
		m_rawSize += sz;
		m_vaSize += sz;
		m_commands.push_back(cmd);
	}
	
	DWORD curVaOffset()
	{
		return m_vaSize;
	}
	
	
	void prepare() override
	{
		m_rawSize = 0;
		m_vaSize = 0;
		for (auto it=m_commands.begin(); it!=m_commands.end(); ++it)
		{
			int rs = (*it)->size();
			m_rawSize += rs;
			m_vaSize += rs;
		}
	}
	
	void write(std::ofstream& f, int align) override
	{
		int offset = 0;
		for (auto it=m_commands.begin(); it!=m_commands.end(); ++it)
		{			
			auto start = f.tellp();
			int sz = (*it)->write(f, offset, m_header.VirtualAddress);
			auto stop = f.tellp();
			if ((sz != (*it)->size()) || (sz != (stop-start))) {
				std::cout <<  "Alert: " << (*it)->cmd() << "\tReserve: " << (*it)->size() << "\tWrite: " << sz << "\tFact: " << (stop-start) << std::endl;
			}
			// std::cout << (*it)->cmd() << "\t" << "offset: " << offset << "\t+\t" << sz << std::endl;
			offset += sz;
		}
		
//		std::cout << "CodeSection: " << m_rawSize << std::endl;
		{			
			int sz = ALIGN_UP(m_rawSize, align) - m_rawSize;
			char* tmp = (char*)malloc(sz);
			memset(tmp, 0, sz);
			f.write(tmp, sz);
		}
	}
	
	void preLoad(IMAGE_NT_HEADERS64* header) override
	{
		header->OptionalHeader.SizeOfCode = this->vaSize();
		header->OptionalHeader.BaseOfCode = this->header()->VirtualAddress;
		header->OptionalHeader.AddressOfEntryPoint = this->header()->VirtualAddress;
	}
	

private:
	std::vector<AsmCommand*> m_commands;
	
};

#endif