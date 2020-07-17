#ifndef __BUILDER_H__
#define __BUILDER_H__

#include <windows.h>
#include <vector>

#include "section.h"

class Builder
{
public:
	enum class AppType {
		App32,
		App64,
	};
	
	enum AppEnvironment {
		GUI = 2,
		Console = 3,
	};
	
	Builder(AppType type)
	{
		m_env = AppEnvironment::Console;
		m_type = type;
		m_dos_header.e_magic  = 0x5a4d;	// MZ
		m_dos_header.e_cblp = 0x80;
		m_dos_header.e_cp = 0x01;
		m_dos_header.e_cparhdr = 0x04;
		m_dos_header.e_minalloc = 0x10;
		m_dos_header.e_maxalloc = 0xFFFF;
		m_dos_header.e_sp = 0x0140;
		m_dos_header.e_lfarlc = 0x40;
		m_dos_header.e_lfanew = 0x80;		// Start PE-Header
		
		if (type == Builder::AppType::App64)
		{
			m_pe_header = (char*) malloc( sizeof(IMAGE_NT_HEADERS64) );
			memset((char*) m_pe_header, 0, sizeof(IMAGE_NT_HEADERS64));
			
			auto header = ((IMAGE_NT_HEADERS64*)m_pe_header);
			header->Signature = 0x4550;	// PE
			header->FileHeader.Machine = IMAGE_FILE_MACHINE_AMD64;
			header->FileHeader.TimeDateStamp = std::time(nullptr);
			header->FileHeader.SizeOfOptionalHeader = sizeof(IMAGE_OPTIONAL_HEADER64);
			header->FileHeader.Characteristics = ( IMAGE_FILE_RELOCS_STRIPPED |
													IMAGE_FILE_EXECUTABLE_IMAGE | // Executable file
													IMAGE_FILE_LINE_NUMS_STRIPPED |
													IMAGE_FILE_LOCAL_SYMS_STRIPPED |
													IMAGE_FILE_LARGE_ADDRESS_AWARE); 
			
			header->OptionalHeader.Magic = 0x020b; //64-bit optional header
			header->OptionalHeader.NumberOfRvaAndSizes = IMAGE_NUMBEROF_DIRECTORY_ENTRIES;
		} else {
			m_pe_header = (char*) malloc( sizeof(IMAGE_NT_HEADERS64) );
			
			auto header = ((IMAGE_NT_HEADERS32*)m_pe_header);
			header->Signature = 0x4550;	// PE
			header->FileHeader.Machine = IMAGE_FILE_MACHINE_I386;
			header->FileHeader.TimeDateStamp = std::time(nullptr);
			header->FileHeader.SizeOfOptionalHeader = sizeof(IMAGE_OPTIONAL_HEADER32);
			header->FileHeader.Characteristics = ( IMAGE_FILE_RELOCS_STRIPPED |
													IMAGE_FILE_EXECUTABLE_IMAGE | // Executable file
													IMAGE_FILE_LINE_NUMS_STRIPPED |
													IMAGE_FILE_LOCAL_SYMS_STRIPPED |
													IMAGE_FILE_32BIT_MACHINE); 
			
			header->OptionalHeader.Magic = 0x010b; // 32-bit optional header
			header->OptionalHeader.NumberOfRvaAndSizes = IMAGE_NUMBEROF_DIRECTORY_ENTRIES;
		}

		
	}

	~Builder()
	{
		for (auto it=m_sections.begin(); it != m_sections.end(); ++it)
		{
			delete (*it);
		}
		free(m_pe_header);
	}
	
	bool addSection(Section* s)
	{
		for (auto it=m_sections.begin(); it != m_sections.end(); ++it)
		{
			if (strcmp(s->name(), (*it)->name()) == 0) 
				return false;
		}
		
		m_sections.push_back(s);
		return true;
	}
	
	Section* getSection(const char* name)
	{
		for (auto it=m_sections.begin(); it != m_sections.end(); ++it)
		{
			if (strcmp(name, (*it)->name()) == 0) 
				return (*it);
		}
		return nullptr;
	}
	
	void build(const char* name)
	{
		if (m_type == Builder::AppType::App64)
			build64(name);
	}
	
	int align(int sz, int align)
	{
		return ((sz % align) == 0) ? sz :  ((sz / align + 1) * align);
	}
	
	int writeAlign(std::ofstream& file, const char* buf, int sz, int alignment)
	{
		int tail = (alignment > 0) ? (align(sz, alignment) - sz) : 0;
		file.write(buf, sz);
		
		if (tail > 0)
		{
			char zeros[tail] = {0};
			file.write((char*) &zeros, tail);
		}
		
		return sz + tail;
	}
	
	void setEnvironment(AppEnvironment env)
	{
		m_env = env;
	}

private:
	std::vector<Section*> m_sections;
	AppType m_type;
	AppEnvironment m_env;
	
	IMAGE_DOS_HEADER m_dos_header = {};
	char* m_pe_header;
	const char* m_dos_stub = "\x0E\x1F\xBA\x0E\x00\xB4\x09\xCD\x21\xB8\x01\x4C\xCD\x21"\
							 "This program cannot be run in DOS mode.\x0D\x0A\x24";
	
//	void build32(const char* name) {}
	void build64(const char* name) {
		std::ofstream f(name, std::ofstream::out | std::ofstream::binary);
		
		int header_offset = 0;		
		
		header_offset += writeAlign(f,(char*) &m_dos_header, sizeof(IMAGE_DOS_HEADER), 64);
		header_offset += writeAlign(f, m_dos_stub, 56, 0x40);		
		header_offset += (m_type == Builder::AppType::App64) ? sizeof(IMAGE_NT_HEADERS64) : sizeof(IMAGE_NT_HEADERS32);

		auto header = ((IMAGE_NT_HEADERS64*)m_pe_header);
		header->OptionalHeader.MajorSubsystemVersion = 5;
		header->OptionalHeader.MinorSubsystemVersion = 2;
		header->OptionalHeader.MajorOperatingSystemVersion = 5;
		header->OptionalHeader.MinorOperatingSystemVersion = 2;
		header->OptionalHeader.Subsystem = m_env; // 2 - Windows GUI; 3 - Console
		header->OptionalHeader.FileAlignment = 0x200;
		header->OptionalHeader.SectionAlignment = 0x1000;
		header->OptionalHeader.ImageBase = 0x400000;

//		std::cout << align(header_offset, header->OptionalHeader.FileAlignment) << std::endl;
		
		// размер всех заголовков (DOS, DOS-Stub, PE, Section) выравненный на FileAligment
		header->OptionalHeader.SizeOfHeaders = align(header_offset, header->OptionalHeader.FileAlignment); 			
		
		//writeAlign(f, m_pe_header, sizeof(IMAGE_NT_HEADERS64), header->OptionalHeader.FileAlignment);
		
		int offset = header->OptionalHeader.SizeOfHeaders;
		int VA = 0x1000;
		
		for (auto it=m_sections.begin(); it != m_sections.end(); ++it)
		{
			if ((*it)->vaSize() == 0) {
				it = m_sections.erase(it);
				it--;
			}			
		}
		
		header_offset += m_sections.size() * sizeof(IMAGE_SECTION_HEADER);		
		header->FileHeader.NumberOfSections = m_sections.size();
		
		
		for (auto it=m_sections.begin(); it != m_sections.end(); ++it)
		{
			auto head = (*it)->header();
			
			head->PointerToRawData = offset;
			head->SizeOfRawData = ALIGN_UP( (*it)->rawSize(), header->OptionalHeader.FileAlignment);
			head->VirtualAddress = VA;
			head->Misc.VirtualSize = (*it)->vaSize();
			
			(*it)->setImageBase(header->OptionalHeader.ImageBase);
			
//			(*it)->setVA(VA);
			(*it)->recalc();
			
			(*it)->preLoad(header);
			// f.write((char*)(*it)->header(), sizeof(IMAGE_SECTION_HEADER));			
			
//			std::cout <<  (*it)->name() << ":\t"<< (*it)->size() << std::endl;
			
			VA += ((*it)->rawSize() == 0) 
				? header->OptionalHeader.SectionAlignment 
				: ALIGN_UP( (*it)->vaSize(), header->OptionalHeader.SectionAlignment );
			offset += ALIGN_UP( (*it)->rawSize(), header->OptionalHeader.FileAlignment);
		}
		
		
		
		header->OptionalHeader.SizeOfImage = VA;
		
		// header->OptionalHeader.SizeOfStackReserve = 0x1000;
		// header->OptionalHeader.SizeOfStackCommit = 0x1000;
		header->OptionalHeader.SizeOfStackReserve = MAX_STACK_SIZE;
		header->OptionalHeader.SizeOfStackCommit = MAX_STACK_SIZE;
		header->OptionalHeader.SizeOfHeapReserve = 0x10000;
		header->OptionalHeader.SizeOfHeapCommit = 0;
		
/*		for (auto it=m_sections.begin(); it != m_sections.end(); ++it)
		{
			int dd = (*it)->dataDirectory();
			if ((dd > 0) && (dd < IMAGE_NUMBEROF_DIRECTORY_ENTRIES)) {				
				header->OptionalHeader.DataDirectory[dd].VirtualAddress = (*it)->header()->VirtualAddress;
				header->OptionalHeader.DataDirectory[dd].Size = (*it)->vaSize();
			}
		}
	*/	
		
		f.write((char*)m_pe_header, sizeof(IMAGE_NT_HEADERS64));
		
		for (auto it=m_sections.begin(); it != m_sections.end(); ++it)
		{
			f.write((char*)(*it)->header(), sizeof(IMAGE_SECTION_HEADER));
		}
		
		
		{
			int hs = header->OptionalHeader.SizeOfHeaders - header_offset;
			if (hs > 0) {
				char zeros [hs] = {0};
				f.write((char*)&zeros, hs);
			}
		}
		
		
		for (auto it=m_sections.begin(); it != m_sections.end(); ++it)
		{
			(*it)->write(f,  header->OptionalHeader.FileAlignment);
		}		
		
		
		f.close();
	}	
	
};

#endif