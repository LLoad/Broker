#include <Common/Logger.h>
#include <iostream>
#include <fstream>
#include <mutex>
#include <cstring>
#include <algorithm>
#include <iomanip>


#define _snwprintf  snprintf

namespace VixMetaBroker
{
	void _vix_vformat(std::string* result, const std::string& format, va_list args)
	{
		static const std::string types("diouxXDOUeEfFgGaACcSspn");

		result->clear();

		std::string tf;
		const uint32 TS_N = 4096;
		char ts[TS_N];

		std::string::const_iterator si = format.begin();
		std::string::const_iterator t, i = si;

		while (true) {
			i = std::find(i, format.end(), '%');
			if (i == format.end()) {
				result->append(si, format.end());
				break;
			}

			if (*(i + 1) == '%') {
				result->append(si, i + 1);
				i += 2;
				si = i;
				continue;
			}

			t = std::find_first_of(i, format.end(), types.begin(), types.end());
			if (t == format.end()) {
				std::cerr << __FUNCTION__ << "(): Unknown type "
					<< "\"%" << *(i + 1) << "\"" << std::endl;
				abort();
				return;
			}

			result->append(si, i);

			tf.assign(i, t + 1);
			int32 dist = static_cast<int32>(std::distance(i, t));
			if (*t == 'd' || *t == 'i' || *t == 'o' ||
				*t == 'u' || *t == 'x' || *t == 'X')
			{
				if (4 <= dist && *(t - 3) == 'I' && *(t - 2) == '6' && *(t - 1) == '4') {
					_snwprintf(ts, TS_N, tf.c_str(), va_arg(args, int64));
				}

				else if (3 <= dist && *(t - 2) == 'l' && *(t - 1) == 'l') {
					_snwprintf(ts, TS_N, tf.c_str(), va_arg(args, int64));
				}
				else if (2 <= dist && *(t - 1) == 'l') {
					_snwprintf(ts, TS_N, tf.c_str(), va_arg(args, long));
				}
				else {
					_snwprintf(ts, TS_N, tf.c_str(), va_arg(args, int));
				}
			}
			else if (*t == 'e' || *t == 'E' || *t == 'f' || *t == 'F' ||
				*t == 'g' || *t == 'G' || *t == 'a' || *t == 'A')
			{
				_snwprintf(ts, TS_N, tf.c_str(), va_arg(args, double));
			}
			else if (*t == 'c') {
				tf[tf.length() - 1] = 'c';
				_snwprintf(ts, TS_N, tf.c_str(), va_arg(args, int));
			}
			else if (*t == 'C') {
				tf[tf.length() - 1] = 'C';
				_snwprintf(ts, TS_N, tf.c_str(), va_arg(args, wint_t));
			}
			else if (*t == 's') {
				tf[tf.length() - 1] = 's';
				char* s = va_arg(args, char*);
				_snwprintf(ts, TS_N, tf.c_str(), s);
			}
			else if (*t == 'S') {
				tf[tf.length() - 1] = 'S';
				char* ws = va_arg(args, char*);
				//size_t tssn = std::wcslen(ws) + 1;
				_snwprintf(ts, TS_N, tf.c_str(), ws);
			}
			else if (*t == 'p') {
				_snwprintf(ts, TS_N, tf.c_str(), va_arg(args, void*));
			}

			ts[TS_N - 1] = '\0';
			result->append(ts);
			i = t + 1;
			si = i;
		}
	}


	void vix_vlog(const LogOption& option, const std::string& format, va_list args)
	{
		static std::mutex s_logLock;
		std::lock_guard<std::mutex> lg(s_logLock);

		std::string result;
		_vix_vformat(&result, format, args);

		std::wcout.imbue(std::locale());
		std::cout << std::setw(5);

		bool clearStyle = (LogOption::STYLE_E::ANSI_NONE != option.style());

		if (LogOption::STYLE_E::ANSI_RB == option.style()) {
			std::cout << "\x1B[41m\x1B[1;34m";
		}
		else if (LogOption::STYLE_E::ANSI_BW == option.style()) {
			std::cout << "\x1B[47m\x1B[1;30m";
		}
		else if (LogOption::STYLE_E::ANSI_GY == option.style()) {
			std::cout << "\x1B[42m\x1B[1;33m";
		}
		else if (LogOption::STYLE_E::ANSI_MG == option.style()) {
			std::cout << "\x1B[45m\x1B[1;35m";
		}

		std::cout << "<";
		std::cout << option.name().c_str();
		std::cout << ">";

		if (clearStyle) {
			std::cout << "\x1B[0m";
		}

		std::cout << " " << result << std::endl;
	}
}