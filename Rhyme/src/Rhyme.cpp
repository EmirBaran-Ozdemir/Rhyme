#include "rhypch.h"

#include "Compiler/Tokenizer.h"
#include "Compiler/Parser.h"
#include "Compiler/Generator.h"

int main(int argc, char* argv[])
{
	std::string contents;
	std::fstream input_file("/home/Emir/VisualStudio/Rhyme/Rhyme/src/test.rhy", std::ios::in);

	//if (argc != 2)
	//{
	//	std::cerr << "ERROR::INCORRECT_USAGE" << std::endl;
	//	std::cerr << "Correct usage: Rhyme <filename>.rhy" << std::endl;
	//	return EXIT_FAILURE;
	//}

	//std::fstream input_file(argv[1], std::ios::in);
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

		Compiler::Generator generator(program.value());
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

		std::cout << "Exit Status: " << WEXITSTATUS(exitCode) << std::endl;

	}
	catch (std::invalid_argument& err)
	{
		std::cerr << err.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}