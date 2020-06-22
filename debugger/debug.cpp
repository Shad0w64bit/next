#include <iostream>
#include "../utils/arguments.h"
#include "../utils/conmenu.h"


void printHelp()
{
	std::cout 
		<< "Next debugger v0.1\n"
		<< "\t-h | --help\t\tHelp\n"
		<< "\t-f <in:filename>\tExecutable debug file\n"
		<< "\t-m <in:filename>\tMeta-file with debug information";
}

int main()
{
	Arguments args;
	
	if ((args.is("-h") || args.is("--help")) ||
	    (!args.is("-f") || !args.is("-m")))
	{
		printHelp();
		return 0;
	}
	
	const char* exe = args.get("-f");
	const char* meta = args.get("-m");
	std::system("cls");
	std::cout << "Debug file: " << exe
		<< "\nMeta-file: " << meta << std::endl;
		
	
	/*
	ConMenu menu;
	
	menu.add("Main");
	menu.add("View");
	menu.add("Options");
	menu.add("Exit");
	
	*/
	/*for (int i=1; i<=5; i++)
	{
		char str[8];
		sprintf(str,"Item %d", i);
		menu.add(str);
		
	}*/
	/*
	while (true)
	{
		int res = menu.show();
		printf("Selected item is %d", res+1);
		if (res == 3)
			break;
	}
	*/
	
	return 0;
}