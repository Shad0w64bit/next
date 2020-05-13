#ifndef _UTILS
#define _UTILS

#include <io.h>

bool FileExists(const char *fname)
{
	return access(fname, 0) != -1;
}

std::ostream& operator<<(std::ostream& os, const Token::Kind& kind) {
  static const char* const names[]{
		"Identifier",
		"Keyword\t",
		"Number\t",
		"Assign\t",
		
		"LeftParen",
		"RightParen",
		"LeftSquare",
		"RightSquare",
		"LeftCurly",
		"RightCurly",
		
		"Plus\t",
		"Minus\t",
		"Asterisk",
		"Slash\t",
		"Backslash",
		"Equal\t",
		"NotEqual",
		
		"Comma\t",
		"Colon\t",
		"Semicolon",
		"SingleQuote",
		"DoubleQuote",
		
		"String\t",
		"Comment\t",
		
		"Unexpected",
		"End\t",
		"LineBreak",
		// Служебный
		"Start\t",
		"Type\t",
		
		"Constant",
		"NewType",
		"NewClass",
		"NewObject",
		"Modificator",
  };
  return os << names[static_cast<int>(kind)];
}
/*
std::ostream& operator<<(std::ostream& os, const Variable::Type& type) {
  static const char* const names[]{
		"i32",
		"u32",
		"i16",
		"u16",
		"i8",
		"u8",
		"Char",
		"Unknown",
  };
  return os << names[static_cast<int>(type)];
}
*/
void exportLexemToFile(std::vector<Token*>& tokens, const char* file)
{	
	std::ofstream f(file, std::ofstream::out);
	f << "Total: " << tokens.size() << std::endl;
	
	for (auto it=tokens.begin(); it != tokens.end(); ++it)
	{
		auto kind = (*it)->kind();
		f << kind << "\t" << ((kind != Token::Kind::LineBreak)?(*it)->get():"#13") << std::endl;
	}
	f.close();
}

#include "core\operations.h"
#define PRINT_OFFSET(n) { for (int i=0; i < (offset+n); i++) { f << "\t"; } }

void printCodeblock(std::ofstream& f, Namespace* ns, Codeblock* cb, int offset = 0)
{
	if (offset > 0)
	{		
		auto args = cb->getArguments();
		for (auto it=args->begin(); it!=args->end(); ++it)
		{
			for (int i=0; i < (offset+1); i++) { f << "\t"; }
			f  << (*it)->name() << ":\t" << (*it)->type()->name << std::endl;
		}
	}
	
	for (int i=0; i < (offset+1); i++) { f << "\t"; }
	f << "{" << std::endl;
	
	ns->name(); // Nothing	

	auto vars = cb->getVariables();
	//std::cout << vars->size() << std::endl;
	if (vars->size() > 0)
	{
		PRINT_OFFSET(2)
		//for (int i=0; i < (offset+2); i++) { f << "\t"; }
		f << "Local variables" << std::endl;
		for (auto it=vars->begin(); it!=vars->end(); ++it)
		{
			PRINT_OFFSET(2)
			f << (*it)->name() << ":\t" << (*it)->type()->name << std::endl;
		}
	}
	
	auto cmds = cb->getOperations();
	for (auto it=cmds->begin(); it!=cmds->end(); ++it)
	{
		if ((*it)->type() == Operation::Type::CodeBlock) {
			printCodeblock(f, ns, ((OpCodeblock*)(*it))->getCodeblock(), offset+1);
		} else if ((*it)->type() == Operation::Type::CallFunction) {
			PRINT_OFFSET(1)
			OpCallFunction* cf = (OpCallFunction*)(*it);
			auto args = cf->getArguments();
			PRINT_OFFSET(1)
			f << cf->getFunction()->name() << "(";
			
			unsigned int c = 1;
			for (auto arg=args->begin(); arg!=args->end(); ++arg)
			{				
				if ((*arg)->name() == nullptr) {
					//std::cout << (*arg)->type()->name << std::endl;
					if (strcmp( (*arg)->type()->name, "~ConstInt") == 0) {
						f << (DWORD)(*((*arg)->data())) << ((c == args->size()) ? "" : ", ");
					} else if (strcmp( (*arg)->type()->name, "~ConstString") == 0) {
						f << "'" << (*arg)->data() << "'" << ((c == args->size()) ? "" : ", ");
					}
				}
				c++;
			}
			
			f << ")" << std::endl;
		}
	}
	
	
	//Codeblock* intCB = 
	
	for (int i=0; i < (offset+1); i++) { f << "\t"; }
	f  << "}" << std::endl;
}

void dumpNamespace(std::ofstream& f, Namespace* ns) {	
	
	f << "Global variables" << std::endl;
	auto vars = ns->getVariables();
	for (auto it=vars->begin(); it!=vars->end(); ++it)
	{
		f << "\t" << (*it)->name() << ":" <<  (*it)->type()->name << std::endl;
	}
	
	f << "Functions" << std::endl;
	auto funcs = ns->getFunctions();
	for (auto it=funcs->begin(); it!=funcs->end(); ++it)
	{
		auto res = (*it)->getResult();
		auto args = (*it)->getArguments();
		auto name = (*it)->name(true);
		
		f << "\t" << ((res == nullptr) ? "func" : (res)->type()->name) << " " << name << "(" ;

		unsigned int c = 1;
		unsigned int sz = args->size();
		for (auto arg=args->begin(); arg!=args->end(); ++arg)
		{
			f << (*arg)->type()->name << " " << (*arg)->name();
			if (c++ != sz) { f << ", "; }
		}
		
		f << ")";
		
		if (strcmp(name, (*it)->name()) != 0)
		{
			f << " as " << (*it)->name();
		}
		
		auto lib = (*it)->getLibrary();
		if (lib != nullptr)
		{
			f << " [" << lib << "]";
		}
		f << std::endl;
		
		if ((*it)->getLibrary() == nullptr)
			printCodeblock(f, ns, (*it)->getCodeblock());
	}
};

void printNamespace(std::ofstream& f, Namespace* ns) {
	auto child = ns->getNamespaces();
	if (child->size() == 0) {
		f << ns->name() << std::endl;
		dumpNamespace(f, ns);
	}
	
	for(auto it=child->begin(); it!=child->end(); ++it)
	{
		f << ns->name() << "\\";
		printNamespace( f, (*it) );			
	}
};

#include "analyzer\parser.h"

void dumpToFile(Parser* pars, const char* file)
{
	std::ofstream f(file, std::ofstream::out);
	auto nss = pars->getNamespaces();
	
	for (auto it=nss->begin(); it!=nss->end(); ++it)
	{
		f << "Namespace "; printNamespace( f, (*it) );		
	}
	f.close();
}


#endif