#ifndef _VIX_META_BROKER_LOGGER_H_
#define _VIX_META_BROKER_LOGGER_H_

#include <Common/Type.h>
#include <cstdarg>
#include <unordered_set>
#pragma warning(disable: 4996)

namespace VixMetaBroker
{
	class LogOption {
	public :
		enum class STYLE_E
		{
			ANSI_NONE = 0,
			ANSI_RB,
			ANSI_BW,
			ANSI_GY,
			ANSI_MG,
		};

	public :
		LogOption() : _name(""), _funcName(""), _line(0), _eStyle(STYLE_E::ANSI_NONE)
		{

		}

		LogOption(const std::string& name, const std::string& funcName = "", int32 line = 0, STYLE_E eStyle = STYLE_E::ANSI_NONE)
			: _name(name), _funcName(funcName), _line(line), _eStyle(eStyle)
		{

		}

	protected :
		std::string _name;
		std::string _funcName;
		int32 _line;
		STYLE_E _eStyle;

	public :
		const std::string name() const noexcept { return _name; }
		const std::string funcName() const noexcept { return _funcName; }
		int32 line() const noexcept { return _line; }
		STYLE_E style() const noexcept { return _eStyle; }
	};

	void vix_vlog(const LogOption& option, const std::string& format, va_list args);

	namespace
	{
		/*/
		void vix_log(const std::string& name, const std::string* pFormat, ...)
		{
			if (name.empty()) {
				return;
			}

			LogOption option(name, "???");
			char tf[1024] = { 0, };
			va_list args;
			va_start(args, pFormat);
			snprintf(tf, 1024, pFormat->c_str(), args);
			va_end(args);

			std::string newFormat(tf);

			vix_vlog(option, newFormat);
		}
		*/

		void vix_log(const LogOption& option, const std::string* pFormat, ...)
		{
			if (!option.name().empty()) {
				va_list args;
				va_start(args, pFormat);
				vix_vlog(option, *pFormat, args);
				va_end(args);
			}
		}
		
		#define __TRACE_RT_PARMS   VIX_LOG_NAME,std::string(__FUNCTION__),__LINE__
		#define TL_DEBUG  LogOption(__TRACE_RT_PARMS, LogOption::STYLE_E::ANSI_NONE)
		#define TL_INFO   LogOption(__TRACE_RT_PARMS, LogOption::STYLE_E::ANSI_BW)
		#define TL_WARN   LogOption(__TRACE_RT_PARMS, LogOption::STYLE_E::ANSI_GY)
		#define TL_ERROR  LogOption(__TRACE_RT_PARMS, LogOption::STYLE_E::ANSI_MG)
		#define TL_FATAL  LogOption(__TRACE_RT_PARMS, LogOption::STYLE_E::ANSI_RB)

		#define VIX_C_LOG(option, format, ...)					\
			do {												\
				if (VIX_LOG_NAME[0] != '\0') {					\
					std::string fmt(format);					\
					vix_log(option, &fmt, ## __VA_ARGS__);		\
				}												\
			} while(0)

		#define VIXTRACE(format, ...)	VIX_C_LOG(TL_DEBUG, format, __VA_ARGS__)
		#define VIXWARN(format, ...)	VIX_C_LOG(TL_WARN, format, __VA_ARGS__)
		#define VIXERROR(format, ...)	VIX_C_LOG(TL_ERROR, format, __VA_ARGS__)
		#define VIXFATAL(format, ...)	VIX_C_LOG(TL_FATAL, format, __VA_ARGS__)
	}
}

#endif
