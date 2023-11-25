#include "rhypch.h"

#include "Compiler/Tokenizer.h"
#include "Compiler/Parser.h"
#include "Compiler/Generator.h"

int main(int argc, char* argv[])
{
	std::string contents;
	std::fstream input_file("/home/Emir/VisualStudio/RhymeProject/Rhyme/src/test.rhy", std::ios::in);
	bool paramDebug = false;
	bool paramClean = false;
	//if (argc < 2)
	//{
	//	std::cerr << "ERROR::INCORRECT_USAGE" << std::endl;
	//	std::cerr << "Correct usage: Rhyme <filename>.rhy (optional flags) -D(enable debug)" << std::endl;
	//	return EXIT_FAILURE;
	//}
	//std::fstream input_file(argv[1], std::ios::in);
	if (argc > 1)
	{
		for (int i = 1; i < argc; i++) 
		{
			if (std::strcmp(argv[i], "-D") == 0)
			{
				paramDebug = true;
			}
			if (std::strcmp(argv[i], "-C") == 0)
			{
				paramClean = true;
			}
		}
	}
	std::string s = fmt::format("Enable Debug = {}\nEnable Clean = {}\n",paramDebug,paramClean);
	std::cout << s;
	if (!input_file.is_open())
	{
		std::cerr << "ERROR::FILE_NOT_FOUND" << std::endl;
		return EXIT_FAILURE;
	}
	else
	{
		std::stringstream buffer;
		buffer << input_file.rdbuf();
		contents = buffer.str();
		input_file.close();
	}
#ifdef RHY_ENABLE_DEBUG

#elif defined RHY_ENABLE_RELEASE

#endif // RHY_ENABLE_DEBUG

	try
	{
		Compiler::Tokenizer tokenizer(contents);
		auto tokens = tokenizer.Tokenize(contents);

		Compiler::Parser parser(tokens);
		auto program = parser.ParseProgram();

		if (!program.has_value())
		{
			std::cerr << "ERROR::NO_EXIT_FOUND" << std::endl;
			return EXIT_FAILURE;
		}

		Compiler::Generator generator(program.value(), paramDebug);
		auto assembly = generator.GenerateProgram();

		std::fstream output_file("out.asm", std::ios::out);
		if (!output_file.is_open())
		{
			std::cerr << "ERROR::FILE_CREATION_FAILED" << std::endl;
			return EXIT_FAILURE;
		}
		else
		{
			output_file << assembly;
			output_file.close();
		}

		std::system("nasm -f elf64 out.asm");
		std::system("ld out.o -o out");
		int exitCode = std::system("./out");
		if (paramClean)
		{
			std::system("rm out");
			std::system("rm out.o");
			std::system("rm out.asm");
		}
	#ifdef RHY_PLATFORM_WINDOWS
		std::cout << "Exit Status: " << exitCode << std::endl;
	#elif defined RHY_PLATFORM_LINUX
		std::cout << "Exit Status: " << WEXITSTATUS(exitCode) << std::endl;
	#endif
	}
	catch (std::invalid_argument& err)
	{
		std::cerr << err.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}