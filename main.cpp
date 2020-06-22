#include <iostream>
#include <fstream>
#include <string>

unsigned int MAX_STACK_SIZE = 4;

#include "utils/logger.h"
#include "analyzer/token.h"
#include "analyzer/lexer.h"
#include "analyzer/parser.h"
#include "builder/linker.h"
#include "utils/arguments.h"
#include "utils/utils.h"

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

	if (args.is("-e")) { // Log file
		const char* log = args.get("-e"); // Get Log file
		Logger::start(log, true);
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
	
	if (args.is("-l")) { // Export lexem to file		
		const char* f = args.get("-l");
		std::cout << "[out] Lexem file: \t" << f << std::endl;
		exportLexemToFile(tokens, f);
	}
	
	Parser pars (tokens);
	pars.parse();	
	
	if (args.is("-d")) { // Export dump to file		
		const char* f = args.get("-d");
		std::cout << "[out] File with dump: \t" << f << std::endl;
		dumpToFile(&pars, f);
	}
	
	MAX_STACK_SIZE = 0x1000;
	std::cout << "MAX_STACK_SIZE: " << MAX_STACK_SIZE << std::endl;
	
	Linker link (&pars, "Program", "main");	
	

	if (args.is("-o")) { // Output binary file
		const char* f = args.get("-o");
		link.build(f);
	} else {		
		char* of = (char*)malloc(strlen(f) + 4 + 1);
		strcpy(of, f);
		memcpy(of + strlen(f), ".exe\0", 5);
		link.build(of);
		free(of);
	}	
	
	return 0;
}