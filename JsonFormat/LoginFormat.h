#ifndef _VIX_META_BROKER_LOGIN_FORMAT_H_
#define _VIX_META_BROKER_LOGIN_FORMAT_H_

#include <Common/JsonObject.h>

namespace VixMetaBroker
{
	class BasicLoginFormat : public JsonObject
	{
	public :
		BasicLoginFormat();
		virtual ~BasicLoginFormat();

	private :
		JSON_VALUE_DECL(std::string, _id);
		JSON_VALUE_DECL(std::string, _pw);

	public :
		const std::string& id() const noexcept { return _id; }
		const std::string& pw() const noexcept { return _pw; }

		void setId(const std::string& id);
		void setPw(const std::string& pw);

		RET_E createJson() override;
		RET_E parseMemberValues() override;

	public :
		BasicLoginFormat& operator=(const BasicLoginFormat& fmt) noexcept;
		bool operator==(const BasicLoginFormat& fmt) noexcept;
		bool operator!=(const BasicLoginFormat& fmt) noexcept { return !(*this == fmt); }
	};

	class LoginFormat : public JsonObject
	{
	public :
		LoginFormat();
		virtual ~LoginFormat();

	private :
		JSON_VALUE_DECL(std::string, _id);
		JSON_VALUE_DECL(std::string, _pw);
		JSON_VALUE_DECL(uint32, _keepAliveTimeout)

	public :
		const std::string& id() const noexcept { return _id; }
		const std::string& pw() const noexcept { return _pw; }
		uint32 keepAliveTimeout() const noexcept { return _keepAliveTimeout; }

		void setId(const std::string& id);
		void setPw(const std::string& pw);
		void setKeepAliveTimeout(const uint32 timeout);

		RET_E createJson() override;
		RET_E parseMemberValues() override;

	public :
		LoginFormat& operator=(const LoginFormat& fmt) noexcept;
		bool operator==(const LoginFormat& fmt) noexcept;
		bool operator!=(const LoginFormat& fmt) noexcept { return *this == fmt ? false : true; }
	};
}

#endif
