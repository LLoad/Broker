#ifndef _VIX_META_BROKER_HTTP_CLIENT_H_
#define _VIX_META_BROKER_HTTP_CLIENT_H_

#include <Network/Client.h>

#define MAX_TRY_LOGIN_CNT		5

namespace VixMetaBroker
{
	typedef struct session_check_info_s
	{
		bool isConnected;
		int32 loginTryCnt;
		uint64 loginTick;
		uint64 lastAliveTick;

		session_check_info_s()
			: isConnected(false), loginTryCnt(0), loginTick(0), lastAliveTick(0)
		{

		}

		void reset()
		{
			isConnected = false;
			loginTryCnt = 0;
			loginTick = 0;
			lastAliveTick = 0;
		}

	} SESSION_CHECK_INFO_S;

	class HttpClient : public Client
	{
	public :
		HttpClient();
		virtual ~HttpClient();

	private :
		std::string _apiKey;

		SESSION_CHECK_INFO_S _stSessInfo;

	public :
		const std::string& apiKey() const noexcept { return _apiKey; }
		const SESSION_CHECK_INFO_S& sessInfo() const noexcept { return _stSessInfo; }
		bool isConnected() const noexcept { return _stSessInfo.isConnected; }
		void updateAliveTick(uint64 tickCount) noexcept { _stSessInfo.lastAliveTick = tickCount; }

	protected :
		int32 callbackClientRcvHttp(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len) override;
		int32 callbackConnectionError(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len) override;
		int32 recvMessageProc(const std::string& msgStr);

	protected :
		RET_E recvLoginMessageProc(const std::string& msgStr);

	};
}

#endif
