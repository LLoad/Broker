#ifndef _VIX_META_BROKER_NETWORK_DEFINE_H_
#define _VIX_META_BROKER_NETWORK_DEFINE_H_

#include <Common/Type.h>

namespace VixMetaBroker
{
	#define VIX_CLIENT_SESSION_DATA_SIZE	262144		// 256 x 1024
	#define VIX_CLIENT_RX_BUFFER_SIZE		1048576		// 1024 x 1024
	#define VIX_CLIENT_TX_BUFFER_SIZE		VIX_CLIENT_RX_BUFFER_SIZE
	#define VIX_CLINET_MAX_RETRY_CNT		5
	#define VIX_CLIENT_RETRY_SLEEP_MS		250			// ms
	#define VIX_CLIENT_MAX_SND_QUE_SIZE		99999
	#define VIX_CLIENT_MAX_RCV_QUE_SIZE		99999

	enum class NET_TYPE_E
	{
		NONE = 0,
		SERVER,
		CLIENT,
	};

	enum class NET_METHOD_E
	{
		NONE = 0,
		GET,
		POST,
		OPTIONS,
		PUT,
		PATCH,
		DEL,
		CONNECT,
		HEAD,
		COLON_PATH,
	};

	std::string toMethodString(const NET_METHOD_E eMethod);

	enum class NET_CMD_E
	{
		NONE = 0x00000000,
		LOGIN = 0x00000001,
		KEEP_ALIVE = 0x00000002,

		REQ_FRAME_META = 0x00010000,

		SEND_PLUXITY_META = 0x10000001,
	};

	enum class NET_CONTENTS_E
	{
		NONE = 0,
		JSON,
		XML,
	};

	enum class NET_SOCKET_TYPE_E
	{
		NONE = 0,
		WEBSOCKET,
		HTTP,
	};

	typedef struct net_conn_info_s
	{
		std::string host;
		uint32 port;
		int32 threadCnt;
		NET_SOCKET_TYPE_E eSockType;
		NET_CONTENTS_E eContents;
		std::string protocolName;
		std::string subProtocolName;

		net_conn_info_s() noexcept;

		net_conn_info_s& operator=(const net_conn_info_s& stInfo) noexcept;
		bool operator==(const net_conn_info_s& stInfo) noexcept;
		bool operator!=(const net_conn_info_s& stInfo) noexcept { return *this == stInfo ? false : true; }
	} NET_CONN_INFO_S;

	typedef struct net_msg_s
	{
		NET_CMD_E eCmd;
		NET_METHOD_E eMethod;
		NET_CONTENTS_E eContents;
		std::string path;
		std::string apiKey;
		std::string msg;
		std::string protocolName;

		net_msg_s() noexcept;

		net_msg_s& operator=(const net_msg_s& stMsg) noexcept;
		bool operator==(const net_msg_s& stMsg) noexcept;
		bool operator!=(const net_msg_s& stMsg) noexcept { return *this == stMsg ? false : true; }
	} NET_MSG_S;
}

#endif
