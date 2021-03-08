#ifndef __NAMESPACE_H__
#define __NAMESPACE_H__

#include "../core/variable.h"
#include "../core/function.h"
#include "../core/typedef.h"

class Namespace
{
public:
	Namespace(const char* name)
	{
		int sz = strlen(name)+1;
		m_name = (char*) malloc(sz);
		memcpy(m_name, name, sz);
	}
	~Namespace()
	{
		for( auto it=m_namespaces.begin(); it != m_namespaces.end(); ++it)
			delete (*it);
		
		free(m_name);
	}

	const char* name() { return m_name; }
	
	Namespace* getNamespace(const char* name) 
	{
		for( auto it=m_namespaces.begin(); it != m_namespaces.end(); ++it)
		{
			if (strcmp((*it)->name(), name) == 0) 
				return (*it);
		}
		return nullptr;
	}
	
	void addNamespace(Namespace* ns) { m_namespaces.push_back(ns); }
	
	std::vector<Namespace*>* getNamespaces() { return &m_namespaces; }
	std::vector<Variable*>* getVariables() { return &m_variables; }
	std::vector<Function*>* getFunctions() { return &m_functions; }

	bool addVariable(Variable* var)
	{
		for(auto it=m_variables.begin(); it!=m_variables.end(); ++it)
		{
			if ( strcmp((*it)->name(), var->name()) == 0 ) 
				return false;
		}
		m_variables.push_back(var);
		return true;
	}
	
	Variable* getVariable(const char* name)
	{
		for(auto it=m_variables.begin(); it!=m_variables.end(); ++it)
			if ( strcmp((*it)->name(), name) == 0 ) 
				return (*it);
		return nullptr;
	}
	
	bool hasVariable(const char* name)
	{
		for(auto it=m_variables.begin(); it!=m_variables.end(); ++it)
			if ( strcmp((*it)->name(), name) == 0 ) 
				return true;
		return false;
	}
	
	bool addFunction(Function* func) 
	{
		m_functions.push_back(func);
		return true;
	}
	
	Function* getFunction(const char* name)
	{
		for (auto it=m_functions.begin(); it!=m_functions.end(); ++it)
			if ( strcmp((*it)->name(), name) == 0 ) 
				return (*it);
		return nullptr;
	}
	
	Types* types() { return &m_types; }
	
private:
	char* m_name;
	std::vector<Namespace*> m_namespaces;
	std::vector<Variable*> m_variables;
	std::vector<Function*> m_functions;
	Types m_types;
};

#endif
