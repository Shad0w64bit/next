#ifndef __CODEBLOCK_H__
#define __CODEBLOCK_H__

#include <vector>

#include "operation.h"
#include "variable.h"

class Codeblock
{
public:
	~Codeblock()
	{
		for (auto it=m_arguments.begin(); it!=m_arguments.end(); ++it) { delete (*it); }
		for (auto it=m_variables.begin(); it!=m_variables.end(); ++it) { delete (*it); }
		for (auto it=m_operations.begin(); it!=m_operations.end(); ++it) { delete (*it); }
	}

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
	
	bool addArgument(Variable* arg)
	{
		for(auto it=m_arguments.begin(); it!=m_arguments.end(); ++it)
		{
			if ( strcmp((*it)->name(), arg->name()) == 0 ) 
				return false;
		}
		m_arguments.push_back(arg);
		return true;
	}
	
	bool addOperation(Operation* op)
	{
		m_operations.push_back(op);
		return true;
	}
	
	Variable* getVariable(const char* name)
	{
		for(auto it=m_variables.begin(); it!=m_variables.end(); ++it)
			if ( strcmp((*it)->name(), name) == 0 ) 
				return (*it);
		return nullptr;
	}
	
	Variable* getArgument(const char* name)
	{
		for(auto it=m_arguments.begin(); it!=m_arguments.end(); ++it)
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
	
	bool hasArgument(const char* name)
	{
		for(auto it=m_arguments.begin(); it!=m_arguments.end(); ++it)
			if ( strcmp((*it)->name(), name) == 0 ) 
				return true;
		return false;
	}
	
	std::vector<Variable*>* getArguments() { return &m_arguments; }
	std::vector<Variable*>* getVariables() { return &m_variables; }
	std::vector<Operation*>* getOperations() { return &m_operations; }
	
	
	
	
private:
	std::vector<Variable*> m_variables;
	std::vector<Variable*> m_arguments;
	std::vector<Operation*> m_operations;
};

#endif