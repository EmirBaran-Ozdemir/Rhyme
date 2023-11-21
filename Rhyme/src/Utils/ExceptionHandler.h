#include <stdexcept>
#include <cassert>
#include <cstdarg>

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
namespace Handler {

	class Exception {
	public:

		static void ThrowError(const char* className, const char* format, ...)
		{
			int buffSize = 200;
			char buffer[buffSize];

			va_list args;
			va_start(args, format);
			std::vsnprintf(buffer, buffSize, format, args);
			va_end(args);

			throw std::invalid_argument(buffer);
		}

		static inline std::string ClassName(const std::string& prettyFunction)
		{
			std::size_t endPos = prettyFunction.find("(");

			if (endPos != std::string::npos) {
				std::size_t startPos = prettyFunction.rfind("::", endPos);
				endPos = startPos;
				if (startPos != std::string::npos) {
					startPos = prettyFunction.rfind("::", startPos - 1);
					if (startPos != std::string::npos) {
						std::string className = prettyFunction.substr(startPos + 2, endPos - startPos - 2);
						return className;
					}
				}
			}
			return prettyFunction;
		}


	};
}
#define THROW_ERROR(...) Handler::Exception::ThrowError(Handler::Exception::ClassName(RHY_FUNC_SIG).c_str(), __VA_ARGS__)










