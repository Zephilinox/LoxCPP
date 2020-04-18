#include <iostream>
#include <fstream>
#include <sstream>

#include "Interpreter.hpp"

//todo: make LoxCPP a library and move this to CTEST/LoxCPPInterpreter/REPL?

int main(const int argc, char* argv[])
{
	if (argc > 2)
	{
		std::cout << "Usage: LoxCPP [script]\n";
		return 64;
	}

	Interpreter interpreter;
	
	if (argc == 2)
	{
		return interpreter.runFile(argv[1]);
	}

	return interpreter.runPrompt();
}