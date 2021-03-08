#ifndef __IMPORT_SECTION_H__
#define __IMPORT_SECTION_H__

#ifdef __unix__
#include <stdint.h>
#endif

#include <vector>

#include "section.h"

struct Func
{
	char* data;
	char* name;
};


struct ImportFunc
{
	Func func;
	uint64_t INT;
	uint32_t offsetIAT;
	uint64_t IAT;
};

class Library
{
public:
	Library(const char* name)
	{
		int sz = ALIGN_UP( (strlen(name) + 1), 4);
		m_name = (char*) malloc(sz);
		strcpy(m_name, name);
		m_header = new IMAGE_IMPORT_DESCRIPTOR;
		memset((char*)m_header, 0, sizeof(IMAGE_IMPORT_DESCRIPTOR));
	}
	~Library()
	{
		for(auto it=m_functions.begin(); it!=m_functions.end(); ++it)
		{
			free( (*it).func.data );
		}
		m_functions.clear();
		free(m_name);
		free(m_header);
	}
	
	const char* name() { return m_name; }
	bool add(const char* func) 
	{
		for(auto it=m_functions.begin(); it!=m_functions.end(); ++it)
		{
			if (strcmp(func, (*it).func.name) == 0)
				return false;
		}
		int sz = ALIGN_UP( (sizeof(uint16_t) + strlen(func) + 1), 4);
		char* d = (char*) malloc(sz);
		memset(d, 0, sz);
		strcpy(d+2, func);
		m_functions.push_back( { Func{d, d+2}, 0, 0, 0} );
		return true;
		
	}
	
	IMAGE_IMPORT_DESCRIPTOR* header() { return m_header; }
	
	std::vector<ImportFunc>* getFunctions() { return &m_functions; }
	
private:
	char* m_name;
	IMAGE_IMPORT_DESCRIPTOR* m_header;
	std::vector<ImportFunc> m_functions;
};

class ImportSection: public Section
{
public:
	ImportSection(const char* name) : Section(name) 
	{
		m_update = false;
		m_header.Characteristics = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ;
		
	}
	~ImportSection() {}
	
	ImportFunc* getFunc(const char* library, const char* function)
	{
		Library* lib = nullptr;		
		for (auto it=m_libraries.begin(); it!=m_libraries.end(); ++it)
		{
			if ( strcmp(library, (*it)->name()) == 0 )
			{
				lib = (*it);
				break;
			}
		}
		
		if (lib == nullptr) return nullptr;
		
		auto functions = lib->getFunctions();
		
		for (auto func=functions->begin(); func!=functions->end(); ++func)
		{
			if (strcmp( (*func).func.name, function) == 0)
				return &(*func);
		}
		return nullptr;
	}
	
	Library* add(const char* library, const char* function)
	{
		Library* lib = nullptr;		
		for (auto it=m_libraries.begin(); it!=m_libraries.end(); ++it)
		{
			if ( strcmp(library, (*it)->name()) == 0 )
			{
				lib = (*it);
				break;
			}
		}
		
		if (lib == nullptr)
		{
			m_libraries.push_back( new Library(library) );
			lib = m_libraries.back();
		}
		
		if (lib == nullptr)
			return nullptr;
		
		
		lib->add(function);
		return lib;
	}
	
	
	void prepare() override 
	{		
		int offset = (m_libraries.size() + 1) * sizeof(IMAGE_IMPORT_DESCRIPTOR);
		
		for (auto lib=m_libraries.begin(); lib!=m_libraries.end(); ++lib)
		{
			auto head = (*lib)->header();
			head->Name = offset;
			offset += ALIGN_UP( (strlen( (*lib)->name() ) + 1), 4);
		}
		
		for (auto lib=m_libraries.begin(); lib!=m_libraries.end(); ++lib)
		{
			auto head = (*lib)->header();
			head->OriginalFirstThunk = offset;
			
			auto functions = (*lib)->getFunctions();
			
			offset += (functions->size() + 1) * sizeof(uint64_t); // INT
			head->FirstThunk = offset;
			uint32_t baseOffset = offset;
			offset += (functions->size() + 1) * sizeof(uint64_t); // IAT
			
			// std::cout << "Lib: " << offset << std::endl;
			for (auto func=functions->begin(); func!=functions->end(); ++func)
			{
				(*func).offsetIAT = baseOffset;
				baseOffset += sizeof(uint64_t);
				
				(*func).INT = offset;
				(*func).IAT = offset;
				offset += ALIGN_UP( (strlen( (*func).func.name ) + 1 + sizeof(uint16_t)), 4);
			}			
		}
		
		m_rawSize = m_vaSize = offset;
		
	}
	
	void recalc() override
	{
		int VA = header()->VirtualAddress;
		for (auto lib=m_libraries.begin(); lib!=m_libraries.end(); ++lib)
		{
			auto head = (*lib)->header();
			head->OriginalFirstThunk = head->OriginalFirstThunk + VA;
			head->FirstThunk = head->FirstThunk + VA;
			head->Name = head->Name + VA;
			
			auto functions = (*lib)->getFunctions();
			for (auto func=functions->begin(); func!=functions->end(); ++func)
			{
				(*func).offsetIAT += VA;
				(*func).INT = (*func).INT + VA;
				(*func).IAT = (*func).IAT + VA;
			}
		}
	}
	
	void write(std::ofstream& f, int align) override
	{
		for (auto lib=m_libraries.begin(); lib!=m_libraries.end(); ++lib)
		{
			auto head = (*lib)->header();
			f.write((char*)head, sizeof(IMAGE_IMPORT_DESCRIPTOR));
		}

		{
			int sz = sizeof(IMAGE_IMPORT_DESCRIPTOR);
			char* tmp = (char*)malloc(sz);
			memset(tmp, 0, sz);
			f.write(tmp, sz);
			free(tmp);
		}

		for (auto lib=m_libraries.begin(); lib!=m_libraries.end(); ++lib)
		{
			f.write( (*lib)->name(), ALIGN_UP( (strlen((*lib)->name()) + 1), 4));
		}
		
		for (auto lib=m_libraries.begin(); lib!=m_libraries.end(); ++lib)
		{
			auto functions = (*lib)->getFunctions();
			uint64_t t = 0;
			for (auto func=functions->begin(); func!=functions->end(); ++func)
			{
				f.write((char*) &(*func).INT, sizeof(uint64_t));
			}
			
			f.write((char*) &t, sizeof(uint64_t));
			
			for (auto func=functions->begin(); func!=functions->end(); ++func)
			{
				f.write((char*) &(*func).IAT, sizeof(uint64_t));
			}
			
			f.write((char*)&t, sizeof(uint64_t));
			
			for (auto func=functions->begin(); func!=functions->end(); ++func)
			{
				f.write( (*func).func.data, ALIGN_UP( (strlen((*func).func.name) + 1 + sizeof(uint16_t)), 4));
			}
		}
		
		{
			int sz = ALIGN_UP(m_rawSize, align) - m_rawSize;
			char* tmp = (char*)malloc(sz);
			memset(tmp, 0, sz);
			f.write(tmp, sz);
			// std::cout << "Import size: " << m_size << std::endl;
		}
	}
	
	void preLoad(IMAGE_NT_HEADERS64* header) override
	{
		auto head = this->header();
		header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = head->VirtualAddress;
		header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = this->vaSize();
	}
	
//	int dataDirectory() override { return IMAGE_DIRECTORY_ENTRY_IMPORT; }
	
/*	void DataDirectory(IMAGE_DATA_DIRECTORY& dd)
	{
		dd[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = header()->VirtualAddress;
		dd[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = size();
	}*/
	
private:
	std::vector<Library*> m_libraries;
};

#endif
