#include <Network/HttpClient.h>
#include <JsonFormat/ApiKeyFormat.h>
#include <Common/Util.h>
#include <Common/Logger.h>

namespace VixMetaBroker
{
	HttpClient::HttpClient()
		: Client(NET_SOCKET_TYPE_E::HTTP, "HttpClient")
		, _apiKey("")
	{

	}

	HttpClient::~HttpClient()
	{
		quit();
		join();
	}

	int32 HttpClient::callbackClientRcvHttp(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len)
	{
		char buffer[LWS_PRE + 4096] = { 0, };

		char* px = buffer + LWS_PRE;
		int32 lenx = sizeof(buffer) - LWS_PRE;

		if (lws_http_client_read(pLws, &px, &lenx) < 0) {
			return -1;
		}

		std::string msg(px);

		if (msg.empty()) {
			return 0;
		}

		int32 ret = recvMessageProc(msg);
		if (ret != 0) {
			// Fix me
		}
		else {
			removeSendMsg();
		}

		return 0;
	}

	int32 HttpClient::callbackConnectionError(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len)
	{
		if (_sendMsgQue.front().eCmd == NET_CMD_E::LOGIN) {
			_isWaitResponse = false;
			_stSessInfo.loginTryCnt++;
			
			VIXERROR("Connect to %s failed. Retry %d / %d", connectInfo().host.c_str(), _stSessInfo.loginTryCnt, MAX_TRY_LOGIN_CNT);
			
			if (_stSessInfo.loginTryCnt == MAX_TRY_LOGIN_CNT) {
				removeSendMsg();
			}

			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		return 0;
	}

	int32 HttpClient::recvMessageProc(const std::string& msgStr)
	{
		switch (_sendMsgQue.front().eCmd)
		{
		case NET_CMD_E::LOGIN:
			/* Api-Key Parsing */
			// If parsing is failed, return -1.
			recvLoginMessageProc(msgStr);
			break;

		case NET_CMD_E::KEEP_ALIVE:
			{
				int32 a = 0;
			}
			break;
		}

		return 0;
	}

	RET_E HttpClient::recvLoginMessageProc(const std::string& msgStr)
	{
		ApiKeyFormat keyFmt;
		keyFmt.parseFromString(msgStr);
		if (!keyFmt.apiKey().empty()) {
			_apiKey = keyFmt.apiKey();
			_stSessInfo.isConnected = true;
			_stSessInfo.loginTryCnt = 0;
			_stSessInfo.loginTick = TickUtil::currentTick();
			_stSessInfo.lastAliveTick = _stSessInfo.loginTick;
			VIXWARN("Login to %s Success..\n", connectInfo().host.c_str());
		}

		return VIX_SUCCESS;
	}
}