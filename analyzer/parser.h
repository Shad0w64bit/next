#ifndef __PARSER_H__
#define __PARSER_H__

#include <vector>

#include "..\core\namespace.h"
#include "..\core\typedef.h"
#include "..\core\operations.h"
#include "token.h"

class Parser
{
public:
	Parser(std::vector<Token*>& tokens) 
	{
		m_tokens = &tokens;
	}
	
	bool parse()
	{
//		Function* func = nullptr;
		Namespace* curNS = nullptr;

		for (auto it=m_tokens->begin(); it!=m_tokens->end(); ++it)
		{
			if (testNamespace(it))
			{
				curNS = parseNamespace(it);
			} else if (testVariable(it)) {
				it = parseVariables(it, curNS);
			} else if (testImportFunction(it)) {
				it = parseImportFunction(it, curNS);
			} else if (testFunction(it)) {
				it = parseFunction(it, curNS);
			} 

		}	
		
		
		return true;
	}
	
	bool testNamespace(std::vector<Token*>::iterator it)
	{
		if (strcmp( (*it)->get(), "namespace" ) == 0)
		{
			it++;
			if ((*it)->is(Token::Kind::Identifier))
				return true;
		}
		return false;
	}
	
	bool testVariable(std::vector<Token*>::iterator it)
	{
		if ((*it)->kind() == Token::Kind::Type)
		{
			it++;
			
			if ((*it)->kind() == Token::Kind::Identifier)
			{
				while ( ((*it)->kind() == Token::Kind::Keyword) ||
						((*it)->kind() == Token::Kind::Identifier) ||
						((*it)->kind() == Token::Kind::Comma) ||
						((*it)->kind() == Token::Kind::LeftSquare) )
				{
					if ((*it)->kind() == Token::Kind::LeftSquare) {
						it++;
						if ((*it)->kind() == Token::Kind::Number) {
							it++;
							if ((*it)->kind() == Token::Kind::RightSquare)
							{
								it++;
								continue;
							}
						}
						return false;
					}
					it++;
				}
				
				if (((*it)->kind() == Token::Kind::LineBreak) ||
					((*it)->kind() == Token::Kind::Semicolon) ||
					((*it)->kind() == Token::Kind::Assign))
				{
					return true;
				} else {
					return false;
				}

				return true;
			}
		}
		return false;
	}
	
	bool testImportFunction(std::vector<Token*>::iterator it)
	{
		if ( ((*it)->is(Token::Kind::Keyword) && (strcmp((*it)->get(), "func") == 0)) || 
			 (*it)->is(Token::Kind::Type) )
		{			
			it++;
			if ((*it)->is(Token::Kind::Identifier)) {
				
				it++;
				if ((*it)->is(Token::Kind::LeftParen)) { 
					it++;
					while (!(*it)->is(Token::Kind::RightParen) )
					{
						if (!((*it)->is(Token::Kind::Identifier) || (*it)->is(Token::Kind::Type) || (*it)->is(Token::Kind::Comma)) )
						{
							return false;
						}
						
						it++;
					}
					
					it++;
					
					if ((*it)->is(Token::Kind::Keyword) && (strcmp( (*it)->get(), "as") == 0))
					{
						it++; // Identifier
						it++;
					}
					
					if ((*it)->is(Token::Kind::LeftSquare))
					{
						it++;
						if ((*it)->is(Token::Kind::String)) {
							it++;
							if ((*it)->is(Token::Kind::RightSquare))
							{
								it++;
								if ( ((*it)->is(Token::Kind::LineBreak)) || ((*it)->is(Token::Kind::Semicolon)) )
								{
									return true;
								}
							}
						}
					}
				
				}
			}
			
		}
		return false;
	}
	
	bool testFunction(std::vector<Token*>::iterator it)
	{
	if ( ((*it)->is(Token::Kind::Keyword) && (strcmp((*it)->get(), "func") == 0)) || 
			 (*it)->is(Token::Kind::Type) )
		{			
			it++;
			if ((*it)->is(Token::Kind::Identifier)) {
				
				it++;
				if ((*it)->is(Token::Kind::LeftParen)) { 
					it++;
					while (!(*it)->is(Token::Kind::RightParen) )
					{
						if (!((*it)->is(Token::Kind::Identifier) || (*it)->is(Token::Kind::Type) || (*it)->is(Token::Kind::Comma)) )
						{
							return false;
						}
						
						it++;
					}
					
					it++;
					
					if ((*it)->is(Token::Kind::Keyword) && (strcmp( (*it)->get(), "as") == 0))
					{
						return false;
					}
					
					if ((*it)->is(Token::Kind::LeftSquare))
					{
						return false;						
					}
					
					return true;				
				}
			}
			
		}
		return false;
	}
	
	std::vector<Token*>::iterator parseFunction(std::vector<Token*>::iterator it, Namespace* ns)
	{
		Type* ret = ns->types()->getTypeByName( (*it)->get() );
		it++;
		Function* f = new Function( (*it)->get(), (ret == nullptr) ? nullptr : new Variable(nullptr, ret) );
		it++;
		if ( (*it)->is(Token::Kind::LeftParen) )
		{
			while ( !((*it)->is(Token::Kind::RightParen) ))
			{
				if ((*it)->is(Token::Kind::Comma))
				{
					it++;
					continue;
				}
				
				auto next = it;
				next++;
				
				if ((*it)->is(Token::Kind::Type) && (*next)->is(Token::Kind::Identifier))
				{
					Type* t = ns->types()->getTypeByName( (*it)->get() );
					f->addArgument( new Variable( (*next)->get(), t ) );
				}
				
				it++;
			}
			it++;
			
			while (!(*it)->is(Token::Kind::LeftCurly)) { it++; }
			
//			it++;
			
			it = parseCodeblock(it, ns, f->getCodeblock());
			if (f->getCodeblock()->getMaxStackSize() > MAX_STACK_SIZE) 
			{					
				MAX_STACK_SIZE = f->getCodeblock()->getMaxStackSize();
			}
//			std::cout << "CB funcArgSize: " << f->getCodeblock()->funcArgSize() << std::endl;			
				
			//it++; // }+
		/*	
			while (!(*it)->is(Token::Kind::RightCurly)) { 
				
				it = parseCodeblock(it, ns, f);
				if ((*it)->is(Token::Kind::RightCurly))
					break;
				
				it++; 
			}*/
		}
		
		ns->addFunction(f);
		return it;
	}
	
	std::vector<Token*>::iterator parseCodeblock(std::vector<Token*>::iterator it, Namespace* ns, Codeblock* f)
	{
		it++; // First must be { 		
		while (!(*it)->is(Token::Kind::RightCurly)) { 
//			std::cout << (*it)->get() << std::endl;
			if ((*it)->is(Token::Kind::LeftCurly)) {
				Codeblock* l_cb = new Codeblock();
				l_cb->setParentCB(f);
				it = parseCodeblock(it, ns, l_cb);
				if (l_cb->getStackSize() > MAX_STACK_SIZE) 
				{					
					MAX_STACK_SIZE = l_cb->getStackSize();
					std::cout << "MAX_STACK_SIZE: " << MAX_STACK_SIZE << std::endl;
				}
				f->addOperation( new OpCodeblock(l_cb) );
				it++;
				continue;
				
			} else if (testVariable(it)) {
				//std::cout << "testVariable" << std::endl;
				it = parseLocalVariables(it, ns, f);
			} else if (testCallFunction(it)) {
				parseCallFunction(it, ns, f);
			}
			
			++it;
		}
		
		return it; // Last must be }
	}
	
	bool testCallFunction(std::vector<Token*>::iterator it)
	{		
		if ((*it)->is(Token::Kind::Identifier))
		{
//			std::cout << "testCallFunction! " << (*it)->get() << std::endl;
			it++;
			if ((*it)->is(Token::Kind::LeftParen))
			{
				it++;
				while (!(*it)->is(Token::Kind::RightParen))
				{
					if (!((*it)->is(Token::Kind::Identifier) || (*it)->is(Token::Kind::Comma) || (*it)->is(Token::Kind::Number) || (*it)->is(Token::Kind::String)))
					{
						return false;
					}
					it++;
				}
				return true;
			}
		}
		return false;
	}
	
	std::vector<Token*>::iterator parseCallFunction(std::vector<Token*>::iterator it, Namespace* ns, Codeblock* cb)
	{
		const char* fname = (*it)->get();
		if ((*it)->is(Token::Kind::Identifier))
		{
			std::vector<Variable*> args;
//			std::cout << fname << std::endl;
			it++;
			if ((*it)->is(Token::Kind::LeftParen))
			{
				it++;
				while (!(*it)->is(Token::Kind::RightParen))
				{
					if ((*it)->is(Token::Kind::Identifier))
					{
						if (cb->hasVariable( (*it)->get() )) {
							//printf("test %s\n", (*it)->get());
							args.push_back( cb->getVariable((*it)->get()) );
						} else if (cb->hasArgument( (*it)->get() )) {
							args.push_back( cb->getArgument((*it)->get()) );
						} else if (ns->hasVariable( (*it)->get() )) {
							args.push_back( ns->getVariable((*it)->get()) );
						} else {
							std::cout << "Error Unknown function argument" << std::endl;
						}
					} else if ((*it)->is(Token::Kind::Number)) {
						Variable* var = new Variable(nullptr, ns->types()->getTypeByName("~ConstInt"));
						DWORD num = (DWORD) atoi( (*it)->get() );
						//std::cout << num;
						var->setData( (char*) &num );
						args.push_back( var );
					} else if ((*it)->is(Token::Kind::String)) {
						Variable* var = new Variable(nullptr, ns->types()->getTypeByName("~ConstString"));
						//std::cout << "Parser: " << (*it)->get() << std::endl;
						var->setData( (*it)->get(), strlen( (*it)->get() ) + 1 );
						args.push_back( var );
					}
					it++;
				}
				
				{
//					int ac = (args.size() - 4);
					if (cb->funcArgSize() < args.size()) {
//						std::cout << "setFuncArgSize: " << args.size() << std::endl;
						cb->setFuncArgSize( args.size() );
					}
				}
				
				// Find Function
				
				auto func = ns->getFunction(fname);
				if (func == nullptr) { std::cout << "Function "<< fname << " not found." << std::endl; }
				auto opFunc = new OpCallFunction(func);
				
				
				for(auto arg=args.begin(); arg!=args.end(); ++arg)
				{
					opFunc->addArgument( (*arg) );
				}
				
				cb->addOperation( opFunc );
				
//				std::cout << "Parse Function" << std::endl;
				// End Find Function
			
				
			}
		}
		return it;
	}
	
	std::vector<Token*>::iterator parseImportFunction(std::vector<Token*>::iterator it, Namespace* ns)
	{
		Type* ret = ns->types()->getTypeByName( (*it)->get() );
		it++;
		Function* f = new Function( (*it)->get(), (ret == nullptr) ? nullptr : new Variable("result", ret) );
		
		// Parse Arguments
		it++;
		if ( (*it)->is(Token::Kind::LeftParen) )
		{
			while ( !((*it)->is(Token::Kind::RightParen) ))
			{
				if ((*it)->is(Token::Kind::Comma))
				{
					it++;
					continue;
				}
				
				auto next = it;
				next++;
				
				if ((*it)->is(Token::Kind::Type) && (*next)->is(Token::Kind::Identifier))
				{
					Type* t = ns->types()->getTypeByName( (*it)->get() );
					f->addArgument( new Variable( (*next)->get(), t ) );
				}
				
				it++;
			}
			it++;
			
			if ((*it)->is(Token::Kind::Keyword) && (strcmp((*it)->get(), "as") == 0))
			{
				it++;
				f->setAlias( (*it)->get() );
				it++;
			}
			
			if ((*it)->is(Token::Kind::LeftSquare))
			{
				it++;
				f->import( (*it)->get() );
				
			}
			
		}
		
		ns->addFunction(f);
		return it;
	}
	
	std::vector<Token*>::iterator parseVariables(std::vector<Token*>::iterator it, Namespace* ns)
	{
		Type* t = ns->types()->getTypeByName( (*it)->get() );
		
		it++; 
		
		if (t != nullptr)
		{
			while ( !((*it)->is(Token::Kind::LineBreak) ||
					(*it)->is(Token::Kind::Semicolon)/* ||
					(*it)->is(Token::Kind::Assign)*/) )
			{
				Variable* var = nullptr;
				if ((*it)->is(Token::Kind::Identifier))
				{
					var = new Variable((*it)->get(), t);
//					std::cout << "Variable " << var->name() << std::endl;
					ns->addVariable( var );
				}
				
				//printf("Variable %s", var->name());
/*				
				it++;				
				if ((var != nullptr) && (!(*it)->is(Token::Kind::Assign)))
					continue;
				
				it++;
				
				if ((*it)->is(Token::Kind::Number)) {
					int val = atoi((*it)->get());
					//printf("Assign %s = %d", var->name(), val);
					var->setData( (char*) &val );
				}
				it++;*/
			}
		}
		return it;
	}
	
	std::vector<Token*>::iterator parseLocalVariables(std::vector<Token*>::iterator it, Namespace* ns, Codeblock* cb)
	{
		Type* t = ns->types()->getTypeByName( (*it)->get() );
		
		it++; 
		
		if (t != nullptr)
		{
			while ( !((*it)->is(Token::Kind::LineBreak) ||
					(*it)->is(Token::Kind::Semicolon)/* ||
					(*it)->is(Token::Kind::Assign)*/) )
			{
				Variable* var = nullptr;
				if ((*it)->is(Token::Kind::Identifier))
				{					
					var = new Variable((*it)->get(), t);
					//std::cout << "Local: " << t->name() << "\t" << (*it)->get() << std::endl;
					cb->addVariable( var );
					
					it++;
					
					if ((var != nullptr) && (!(*it)->is(Token::Kind::Assign)))
						continue;
					
					it++;
				
					if ((*it)->is(Token::Kind::Number)) {
						int val = atoi((*it)->get());
//						printf("Assign %s = %d\n", var->name(), val);
						
						var->setData( (char*) &val );
					} else if ((*it)->is(Token::Kind::String)) {						
						//var->setData( (char*) (*it)->get(), strlen( (*it)->get() ) );
					}
				}			
				++it;					
			}			
		}
		return it;
	}
	
	Namespace* parseNamespace(std::vector<Token*>::iterator it)
	{
		Namespace* curNS = nullptr;
		if (strcmp( (*it)->get(), "namespace" ) == 0)
		{
			it++;
			while ( !( (*it)->is(Token::Kind::LineBreak) || (*it)->is(Token::Kind::Semicolon) ) )
			{
				if ((*it)->is(Token::Kind::Identifier))
				{
					if (curNS == nullptr) {
						for (auto ns=m_namespaces.begin(); ns!=m_namespaces.end(); ++ns)
						{
							if ( strcmp( (*it)->get(), (*ns)->name()) == 0 )
							{
								curNS = (*ns);
								break;
							}
						}
						
						curNS = new Namespace((*it)->get());
						m_namespaces.push_back(curNS);
					} else {
						auto tmpNS = curNS->getNamespace((*it)->get());
						
						if (tmpNS == nullptr)
						{
							tmpNS = new Namespace( (*it)->get() );
							curNS->addNamespace( tmpNS );
							curNS = tmpNS;
						} else {
							curNS = tmpNS;						
						}
					}
				}
				
				it++;
			}
			
		}
		return curNS;
	}

	
	std::vector<Namespace*>* getNamespaces() { return &m_namespaces; }
	
	Namespace* getNamespace(const char* name)
	{
		for (auto it=m_namespaces.begin(); it!=m_namespaces.end(); ++it)
			if (strcmp((*it)->name(), name) == 0)
				return (*it);
		return nullptr;
	}
	
private:
	std::vector<Token*>* m_tokens;
	std::vector<Namespace*> m_namespaces;
	
};

#endif