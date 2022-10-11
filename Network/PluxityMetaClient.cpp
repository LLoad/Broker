#include <Common/Logger.h>
#include <Network/PluxityMetaClient.h>

namespace VixMetaBroker
{
	PluxityHttpClient::PluxityHttpClient()
		: Client(NET_SOCKET_TYPE_E::HTTP, "PluxityHttpClient"), _apiKey(""), _sendedCnt(0)
	{

	}

	PluxityHttpClient::~PluxityHttpClient()
	{
		quit();
		join();
	}

	int32 PluxityHttpClient::callbackClientAppedHandshakeHeader(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len)
	{
		uint8** p = (uint8**)pIn;
		uint8* pEnd = (*p) + len;

		NET_MSG_S stMsg = _sendMsgQue.front();

		std::string contentsStr;
		if (stMsg.eContents == NET_CONTENTS_E::JSON) {
			contentsStr = "application/json";
		}

		if (connectInfo().eSockType == NET_SOCKET_TYPE_E::HTTP) {
			if (lws_add_http_header_by_token(pLws, WSI_TOKEN_HTTP_CONTENT_TYPE, (uint8*)contentsStr.c_str(), contentsStr.size(), p, pEnd)) {
				return -1;
			}

			std::string msgSizeStr = std::to_string(stMsg.msg.length());
			(stMsg.msg.length());
			if (lws_add_http_header_by_token(pLws, WSI_TOKEN_HTTP_CONTENT_LENGTH, (uint8*)msgSizeStr.c_str(), msgSizeStr.size(), p, pEnd)) {
				return -1;
			}
		}

		if (!stMsg.apiKey.empty()) {
			std::string keyStr = stMsg.apiKey;
			std::string keyName = "Api-Key:";
			if (lws_add_http_header_by_name(pLws, (uint8*)keyName.c_str(), (uint8*)keyStr.c_str(), keyStr.size(), p, pEnd)) {
				return -1;
			}
		}

		lws_client_http_body_pending(pLws, 1);
		lws_callback_on_writable(pLws);

		return 0;
	}

	int32 PluxityHttpClient::callbackClientRcvHttp(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len)
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

		_isWaitResponse = false;
		RET_E eRet = removeSendMsg();

		return 0;
	}

	int32 PluxityHttpClient::callbackClientHttpClosed(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len)
	{
		return 0;
	}
	
	/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	PluxityMetaClient::PluxityMetaClient()
		: _eStatus(STATUS_E::READY), _pServerInfo(nullptr)
	{
		_sendMsgQue.clear();
	}

	PluxityMetaClient::~PluxityMetaClient()
	{
		_pluxityRestClient.quit();
		_pluxityRestClient.join();
	}

	RET_E PluxityMetaClient::initialize(PluxityServerInfo* pServerInfo)
	{
		if (pServerInfo == nullptr) {
			return VIX_FAILURE;
		}

		_pServerInfo = pServerInfo;

		NET_CONN_INFO_S stConnInfo;
		stConnInfo.host = pServerInfo->ipAddr();
		stConnInfo.port = pServerInfo->port();
		stConnInfo.threadCnt = 1;
		stConnInfo.eSockType = NET_SOCKET_TYPE_E::HTTP;

		RET_E eRet = _pluxityRestClient.initialize(stConnInfo);
		if (eRet != VIX_SUCCESS) {
			return VIX_FAILURE;
		}

		runThread(); 

		return VIX_SUCCESS;
	}

	void PluxityMetaClient::looperFunc()
	{
		//VIXTRACE("SendMsgQue Info : size(%d), frontMsg(%s)", _sendMsgQue.size(), _sendMsgQue.front().c_str());

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	void PluxityMetaClient::pushMsg(std::string msg)
	{
		if (msg.empty()) {
			return;
		}

		NET_MSG_S stMsg;
		stMsg.eCmd = NET_CMD_E::SEND_PLUXITY_META;
		stMsg.eMethod = NET_METHOD_E::POST;
		stMsg.eContents = NET_CONTENTS_E::JSON;
		stMsg.path = _pServerInfo->evtUrl();
		stMsg.apiKey = _pServerInfo->apiKey();
		stMsg.msg = msg;

		_pluxityRestClient.addSendMsg(stMsg);

		//std::lock_guard<std::mutex> lg(_sendQueLock);
		//_sendMsgQue.push_back(msg);
	}

	void PluxityMetaClient::popFrontMsg()
	{
		if (_sendMsgQue.empty()) {
			return;
		}

		//std::lock_guard<std::mutex> lg(_sendQueLock);
		//_sendMsgQue.pop_front();
	}
}