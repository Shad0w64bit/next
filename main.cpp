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
		<< "\t-o <out:filename>\tProgram output file\n"
		<< "\t--type <in:gui/console>\tProgram type\n";
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
	
	if (args.is("--type")) { 
		const char* env = args.get("--type");
		if (strcmp(env, "gui") == 0 || strcmp(env, "GUI") == 0)
		{
			link.setEnvironment(Builder::AppEnvironment::GUI);
		} else if (strcmp(env, "Console") == 0 || strcmp(env, "console") == 0 || strcmp(env, "con") == 0 || strcmp(env, "term") == 0)
		{
			link.setEnvironment(Builder::AppEnvironment::Console);
		} else {
			std::cerr << "Unknown argument --type is \"" << env << "\" must be GUI or Console."	<< std::endl;
			return -1;
		}
		
	}
	

	if (args.is("-o")) { // Output binary file
		const char* f = args.get("-o");
		// TODO: if the target directory does not exist, create it	
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