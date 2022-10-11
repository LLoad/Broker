#ifndef _VIX_META_BROKER_VIX_SERVER_INFO_H_
#define _VIX_META_BROKER_VIX_SERVER_INFO_H_

#include <JsonFormat/LoginFormat.h>

namespace VixMetaBroker
{
	class VixServerInfo : public JsonObject
	{
	public :
		VixServerInfo(uint32 serverId = 0);
		VixServerInfo(const VixServerInfo& info);
		virtual ~VixServerInfo();

	private :
		JSON_VALUE_DECL(uint32, _serverId);
		JSON_VALUE_DECL(std::string, _ipAddr);
		JSON_VALUE_DECL(uint32, _port);
		JSON_VALUE_DECL(int32, _rcvQueCnt)
		
		std::string _loginInfoName;
		LoginFormat _loginInfo;

	public :
		uint32 serverId() const noexcept { return _serverId; }
		const std::string& ipAddr() const noexcept { return _ipAddr; }
		uint32 port() const noexcept { return _port; }
		LoginFormat& loginInfo() noexcept { return _loginInfo; }

	public :
		void setIpAddr(const std::string& ip) noexcept { _ipAddr = ip; }
		void setPort(const uint32 port) noexcept { _port = port; }
		void setLoginInfo(const LoginFormat& info) noexcept { _loginInfo = info; }

	public :
		RET_E createJson() override;

	protected:
		RET_E parseMemberValues() override;

	public :
		VixServerInfo& operator=(const VixServerInfo& info);
		bool operator==(const VixServerInfo& info);
		bool operator!=(const VixServerInfo& info) noexcept { return *this == info ? false : true; }
	};
}

#endif
