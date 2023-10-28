#include "rhypch.h"
#pragma once
#include "Parser.h"

namespace Compiler {


	class Generator {
		struct Variable {
			size_t stackLocation;
		};

	public:
		Generator(const Node::Program& program);
		void GenerateExpression(const Node::Expr* expr);
		void GenerateTerm(const Node::Term* term);
		void GenerateStatement(const Node::Statement* statement);
		std::string GenerateProgram();

	private:
		void Push(const std::string& reg);
		void Pop(const std::string& reg);
		void Move(const std::string& reg, const std::string& value);
		void ThrowError(const std::string& message);
		void ThrowError(const std::string& message, const std::string& ident);
	private:
		const Node::Program m_Program;
		std::stringstream m_Output;
		size_t m_StackSize = 0;
		std::unordered_map<std::string, Variable> m_VariableList{};
	};
}