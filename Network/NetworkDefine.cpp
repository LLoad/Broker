#include <Network/NetworkDefine.h>

namespace VixMetaBroker
{
	std::string toMethodString(const NET_METHOD_E eMethod)
	{
		switch (eMethod)
		{
		case NET_METHOD_E::GET:
			return std::string("GET");

		case NET_METHOD_E::POST:
			return std::string("POST");

		case NET_METHOD_E::OPTIONS:
			return std::string("OPTIONS");

		case NET_METHOD_E::PUT:
			return std::string("PUT");

		case NET_METHOD_E::PATCH:
			return std::string("PATCH");

		case NET_METHOD_E::DEL:
			return std::string("DELETE");

		case NET_METHOD_E::CONNECT:
			return std::string("CONNECT");

		case NET_METHOD_E::HEAD:
			return std::string("HEAD");

		case NET_METHOD_E::COLON_PATH:
			return std::string("COLON_PATH");

		default:
			return "";
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	net_conn_info_s::net_conn_info_s() noexcept
		: host(""), port(80), threadCnt(1), eSockType(NET_SOCKET_TYPE_E::NONE), eContents(NET_CONTENTS_E::NONE)
		, protocolName(""), subProtocolName("")
	{
		
	}

	net_conn_info_s& net_conn_info_s::operator=(const net_conn_info_s& stInfo) noexcept
	{
		host = stInfo.host;
		port = stInfo.port;
		eSockType = stInfo.eSockType;
		eContents = stInfo.eContents;
		protocolName = stInfo.protocolName;
		subProtocolName = stInfo.subProtocolName;

		return *this;
	}

	bool net_conn_info_s::operator==(const net_conn_info_s& stInfo) noexcept
	{
		if (host == stInfo.host &&
			port == stInfo.port &&
			eSockType == stInfo.eSockType &&
			eContents == stInfo.eContents &&
			protocolName == stInfo.protocolName &&
			subProtocolName == stInfo.subProtocolName)
		{
			return true;
		}
		else {
			return false;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	net_msg_s::net_msg_s() noexcept
		: eCmd(NET_CMD_E::NONE), eMethod(NET_METHOD_E::NONE), eContents(NET_CONTENTS_E::NONE)
		, path(""), apiKey(""), msg(""), protocolName("")
	{

	}

	net_msg_s& net_msg_s::operator=(const net_msg_s& stMsg) noexcept
	{
		eCmd = stMsg.eCmd;
		eMethod = stMsg.eMethod;
		eContents = stMsg.eContents;
		path = stMsg.path;
		apiKey = stMsg.apiKey;
		msg = stMsg.msg;
		protocolName = stMsg.protocolName;

		return *this;
	}

	bool net_msg_s::operator==(const net_msg_s& stMsg) noexcept
	{
		if (eCmd == stMsg.eCmd &&
			eMethod == stMsg.eMethod &&
			eContents == stMsg.eContents &&
			path == stMsg.path &&
			apiKey == stMsg.apiKey &&
			msg == stMsg.msg &&
			protocolName == stMsg.protocolName)
		{
			return true;
		}
		else {
			return false;
		}
	}
}