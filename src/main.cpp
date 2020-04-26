#include <iostream>
#include <fstream>
#include <sstream>

#include "Runner.hpp"

//todo: make LoxCPP a library and move this to CTEST/LoxCPPRunner/REPL?

int main(const int argc, char* argv[])
{
	if (argc > 2)
	{
		std::cout << "Usage: LoxCPP [script]\n";
		return 64;
	}

	Runner Runner;
	
	if (argc == 2)
	{
		return Runner.runFile(argv[1]);
	}

	return Runner.runPrompt();
}