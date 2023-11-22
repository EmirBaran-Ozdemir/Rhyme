#pragma once
#include <string>
#include <stdexcept>
#include <fmt/core.h>

namespace Utils {

	class ExceptionHandler {
	public:
		static void ThrowError(const char* className, const std::string& error);
		static void ThrowError(const char* className, const char* format, fmt::format_args args);
		static std::string ClassName(const std::string& prettyFunction);


	};
}

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
	#define RHY_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
	#define RHY_FUNC_SIG __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
	#define RHY_FUNC_SIG __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
	#define RHY_FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
	#define RHY_FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
	#define RHY_FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
	#define RHY_FUNC_SIG __func__
#else
	#define RHY_FUNC_SIG "WARNING::UNKNOWN::RHY_FUNC_SIG"
#endif

#define THROW_ERROR_ARGS(format, ...) Utils::ExceptionHandler::ThrowError(Utils::ExceptionHandler::ClassName(RHY_FUNC_SIG).c_str(),format, fmt::make_format_args(__VA_ARGS__))
#define THROW_ERROR(error) Utils::ExceptionHandler::ThrowError(Utils::ExceptionHandler::ClassName(RHY_FUNC_SIG).c_str(), error)
#define test() printf("test")