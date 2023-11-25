#include "ExceptionHandler.h"

namespace Utils {

	void ExceptionHandler::ThrowError(const char* className, const std::string& error)
	{
		throw std::invalid_argument(error);
	}

	void ExceptionHandler::ThrowError(const char* className, const char* format, fmt::format_args args)
	{
		std::string buffer = fmt::vformat(format, args);
		throw std::invalid_argument(buffer);
	}

	std::string ExceptionHandler::ClassName(const std::string& prettyFunction)
	{
		std::size_t endPos = prettyFunction.find("(");

		if (endPos != std::string::npos)
		{
			std::size_t startPos = prettyFunction.rfind("::", endPos);
			endPos = startPos;
			if (startPos != std::string::npos)
			{
				startPos = prettyFunction.rfind("::", startPos - 1);
				if (startPos != std::string::npos)
				{
					std::string className = prettyFunction.substr(startPos + 2, endPos - startPos - 2);
					return className;
				}
			}
		}
		return prettyFunction;
	}

}