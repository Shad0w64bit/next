#ifndef __FUNCTION_H__
#define __FUNCTION_H__

#include <vector>

#include "codeblock.h"
#include "variable.h"

class Function
{
public:
	Function(const char* name, Variable* result = nullptr) 
	{
		int sz = strlen(name) + 1;
		m_name = (char*) malloc(sz);
		memcpy(m_name, name, sz);		
		m_lib = nullptr;
//		m_func = nullptr;
		m_alias = nullptr;
		m_ret = result;
	}
	
	~Function()
	{
/*		for(auto it=m_args.begin(); it!=m_args.end(); ++it)
		{
			delete (*it);
		}*/
		free(m_name);
		if (m_alias != nullptr) { free(m_alias); }
		if (m_lib != nullptr) {	free(m_lib); }
//		if (m_func != nullptr) { free(m_func); }
	}
	
	void setAlias(const char* name)
	{
		if (m_alias != nullptr)
			free(m_alias);
	
		int sz = strlen(name) + 1;
		m_alias = (char*) malloc(sz);
		memcpy(m_alias, name, sz);
	}
	
	bool isImport()
	{
		return (m_lib != nullptr);
	}
	
	
	void import(const char* lib)
	{
		if (m_lib != nullptr) {	free(m_lib); }
//		if (m_func != nullptr) { free(m_func); }
		
		int sz = strlen(lib)+1;
		m_lib = (char*) malloc(sz);
		memcpy(m_lib, lib,sz);
		
	/*	
		sz = strlen(func)+1;
		m_func = (char*) malloc(sz);
		memcpy(m_func, func, sz);*/
	}
	
/*	bool addArgument(Variable* var)
	{
		for(auto it=m_args.begin(); it!=m_args.end(); ++it)
		{
			if (((*it)->name(), var->name()) == 0)
				return false; // Name already exist
		}
		
		m_args.push_back(var);
		return true;
	}*/
	
	bool addArgument(Variable* var)
	{
		if (!m_codeblock.hasArgument(var->name()))
		{
			m_codeblock.addArgument(var);
			return true;
		}
		return false;
	}
	
	Variable* getArgument(const char* name)
	{
		return m_codeblock.getArgument(name);
	}
	
	const char* name(bool original = false)
	{
		return (original || m_alias == nullptr) ? m_name : m_alias;	
	}
	
	//const char* name() { return m_name; }
	void setName(const char* name)
	{
		if (m_name != nullptr)
			free(m_name);
		
		int sz = strlen(name) + 1;
		m_name = (char*) malloc(sz);
		memcpy(m_name, name, sz);
	}
	
	Variable* getResult() { return m_ret; }
//	std::vector<Variable*>* getArguments() { return &m_args; }
	std::vector<Variable*>* getArguments() { return m_codeblock.getArguments(); }
	
	const char* getLibrary() { return m_lib; }
	Codeblock* getCodeblock() { return &m_codeblock; };
	
	DWORD address() { return m_address + m_offset; }
	void setVA(DWORD address) { m_address = address; }
	void setOffset(DWORD offset) { m_offset = offset; }
	
private:
	char* m_name;
	char* m_alias;
	char* m_lib;
	Codeblock m_codeblock;
//	char* m_func;
	Variable* m_ret;
	//std::vector<Variable*> m_args;
	DWORD m_offset = 0;
	DWORD m_address = 0;
};

#endif