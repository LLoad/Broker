#ifndef _PLUXITY_SERVER_INFO_H_
#define _PLUXITY_SERVER_INFO_H_

#include <JsonFormat/LoginFormat.h>

namespace VixMetaBroker
{
	class PluxityServerInfo : public JsonObject
	{
	public :
		PluxityServerInfo();
		PluxityServerInfo(const PluxityServerInfo& info);
		virtual ~PluxityServerInfo();

	private :
		JSON_VALUE_DECL(std::string, _ipAddr);
		JSON_VALUE_DECL(uint32, _port);
		JSON_VALUE_DECL(std::string, _evtUrl);
		JSON_VALUE_DECL(std::string, _apiKey);

		std::string _loginInfoName;
		BasicLoginFormat _loginInfo;

	public :
		const std::string& ipAddr() const noexcept { return _ipAddr; }
		uint32 port() const noexcept { return _port; }
		const std::string& evtUrl() const noexcept { return _evtUrl; }
		const std::string& apiKey() const noexcept { return _apiKey; }

		BasicLoginFormat& loginInfo() noexcept { return _loginInfo; }


	public :
		RET_E createJson() override;

	protected :
		RET_E parseMemberValues() override;

	public :
		PluxityServerInfo& operator=(const PluxityServerInfo& info);
		bool operator==(const PluxityServerInfo& info);
		bool operator!=(const PluxityServerInfo& info) { return !(*this == info); }
	};
}

#endif
