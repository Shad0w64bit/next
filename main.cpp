#include <iostream>
#include <fstream>

unsigned int MAX_STACK_SIZE = 4;

#include "analyzer/token.h"
#include "analyzer/lexer.h"
#include "analyzer/parser.h"
#include "builder/linker.h"
#include "arguments.h"
#include "utils.h"

void printHelp()
{
	std::cout 
		<< "Next compilier v0.1\n"
		<< "\t-h | --help\t\tHelp\n"
		<< "\t-i <in:filename>\tSource file\n"
		<< "\t-l <out:filename>\tFile with lexem\n"
		<< "\t-d <out:filename>\tDump internal tables\n"
		<< "\t-o <out:filename>\tProgram output file\n";
}

int main()
{
	setlocale(LC_ALL, "rus");
	
	Arguments args;
	
	if (args.is("-h") || args.is("--help")) {
		printHelp();
		return 0;
	}	
	
	const char* f = args.get("-i"); // Get Input File
	
	if (strcmp(f, "") == 0) {
		std::cout << "No input file specified." << std::endl;
		return -1;
	}
	
	if (!FileExists( f )) {
		std::cout << "File doesn't exists." << std::endl;
		return -1;
	}
	
	std::ifstream file( f, std::ios_base::in | std::ios_base::binary );
	
	if (!file.is_open()) {
		std::cout << "Could not open file." << std::endl;
        return -1; 
	}
	
	std::cout << "[in]  Source file: \t" << f << std::endl;

	std::vector<Token*> tokens;
	
	Lexer lex (file, tokens);
	lex.analyze();	
	
	Parser pars (tokens);
	pars.parse();	
	
	Linker link (&pars, "Program", "main");
	
	link.build("test1.exe");
	
	
	if (args.is("-l")) { // Export lexem to file		
		const char* f = args.get("-l");
		std::cout << "[out] Lexem file: \t" << f << std::endl;
		exportLexemToFile(tokens, f);
	}
	
	if (args.is("-d")) { // Export dump to file		
		const char* f = args.get("-d");
		std::cout << "[out] File with dump: \t" << f << std::endl;
		dumpToFile(&pars, f);
	}
	
	return 0;
}