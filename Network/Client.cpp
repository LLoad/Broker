#include <Network/Client.h>
#include <Common/Logger.h>
#include <string>

namespace
{
	static int32_t _clientCallbackFunc(lws* pLws, lws_callback_reasons eReason, void* pUser, void* pIn, size_t len)
	{
		if (pUser) {
			VixMetaBroker::Client* pClient = reinterpret_cast<VixMetaBroker::Client*>(pUser);
			return pClient->clientCallbackFunc(pLws, eReason, pIn, len);
		}
		else {
			return lws_callback_http_dummy(pLws, eReason, pUser, pIn, len);
		}
	}
}

namespace VixMetaBroker
{
	Client::Client(const NET_SOCKET_TYPE_E eSockType, const std::string& name)
		: _eType(NET_TYPE_E::CLIENT)
		, _eSocketType(eSockType), _name(name), _pLwsCtx(nullptr), _pLws(nullptr)
		, _isWaitResponse(false)
	{
		
	}

	Client::~Client()
	{
		destroyLwsCtx();

		quit();
		join();
	}

	RET_E Client::initialize(const NET_CONN_INFO_S& stConnInfo)
	{
		_stConnInfo = stConnInfo;

		RET_E eRet = VIX_SUCCESS;
		eRet = createLwsCtx();
		if (eRet != VIX_SUCCESS) {
			return eRet;
		}

		_isRun = true;

		runThread();

		return VIX_SUCCESS;
	}


	void Client::looperFunc()
	{
		//printf("<%s:%s(%d)> ** Thread running..\n", __FILE__, __FUNCTION__, __LINE__);

		subLooperFunc();

		sendMsg();

		if (_pLwsCtx) {
			lws_service(_pLwsCtx, 0);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	RET_E Client::createLwsCtx()
	{
		RET_E eRet = RET_E::SUCCESS;

		lws_set_log_level(LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE, NULL);

		lws_context_creation_info stCtxInfo = { 0, };
		stCtxInfo.port = CONTEXT_PORT_NO_LISTEN;
		stCtxInfo.count_threads = _stConnInfo.threadCnt;
		stCtxInfo.user = this;
		stCtxInfo.pt_serv_buf_size = VIX_LWS_SERVICE_BUFF_SIZE;

		if (_stConnInfo.eSockType == NET_SOCKET_TYPE_E::HTTP) {
			makeLwsProtocol(&stCtxInfo.protocols);
			stCtxInfo.options = LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;
		}
		else {
			makeLwsProtocol(&stCtxInfo.protocols);
		}

		_pLwsCtx = lws_create_context(&stCtxInfo);
		if (!_pLwsCtx) {
			return VIX_FAILURE;
		}

		return eRet;
	}

	RET_E Client::destroyLwsCtx()
	{
		RET_E eRet = VIX_SUCCESS;

		if (_pLws) {
			_pLws = nullptr;
		}

		if (_pLwsCtx) {
			lws_context_destroy(_pLwsCtx);
			_pLwsCtx = nullptr;
		}

		return eRet;
	}

	void Client::makeLwsProtocol(const lws_protocols** ppstProtocols)
	{
		std::string protocolName = _stConnInfo.protocolName;

		static lws_protocols clientProtocols[] = {
			{ protocolName.c_str(), _clientCallbackFunc, VIX_CLIENT_SESSION_DATA_SIZE, VIX_CLIENT_RX_BUFFER_SIZE, 0, NULL, VIX_CLIENT_TX_BUFFER_SIZE },
			{ NULL, NULL, 0, 0}
		};

		*ppstProtocols = clientProtocols;
	}

	int32 Client::clientCallbackFunc(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len)
	{
		switch (eReason) {
		case LWS_CALLBACK_CLIENT_ESTABLISHED:
			callbackClientEstablished(pLws, eReason, pIn, len);
			break;

		case LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP:
			callbackClientEstablishedHttp(pLws, eReason, pIn, len);
			break;

		case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
			callbackClientAppedHandshakeHeader(pLws, eReason, pIn, len);
			break;

		case LWS_CALLBACK_CLIENT_HTTP_WRITEABLE:
			callbackClientHttpWriteable(pLws, eReason, pIn, len);
			break;

		case LWS_CALLBACK_CLIENT_RECEIVE:
			callbackClientRcv(pLws, eReason, pIn, len);
			break;

		case LWS_CALLBACK_RECEIVE_CLIENT_HTTP:
			callbackClientRcvHttp(pLws, eReason, pIn, len);
			break;

		case LWS_CALLBACK_RECEIVE_CLIENT_HTTP_READ:
			callbackClientRcvHttpRead(pLws, eReason, pIn, len);
			break;

		case LWS_CALLBACK_CLOSED_CLIENT_HTTP:	// Rest
			callbackClientHttpClosed(pLws, eReason, pIn, len);
			break;

		case LWS_CALLBACK_CLIENT_CLOSED:	// Websocket
			callbackClientClosed(pLws, eReason, pIn, len);
			break;

		case LWS_CALLBACK_CLOSED:
			callbackClosed(pLws, eReason, pIn, len);
			break;

		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR: // Rest
			callbackConnectionError(pLws, eReason, pIn, len);
			break;
		}

		return 0;
	}

	RET_E Client::addSendMsg(const NET_MSG_S& stMsg)
	{
		if (_sendMsgQue.size() >= VIX_CLIENT_MAX_SND_QUE_SIZE) {
			return VIX_FAILURE;
		}

		std::lock_guard<std::mutex> lockGuard(_sendQueMutex);
		_sendMsgQue.push_back(stMsg);

		return VIX_SUCCESS;
	}
	
	RET_E Client::removeSendMsg()
	{
		if (!_sendMsgQue.empty() && !_isWaitResponse) {
			std::lock_guard<std::mutex> lockGuard(_sendQueMutex);
			_sendMsgQue.pop_front();
			return VIX_SUCCESS;
		}

		if (_sendMsgQue.size() >= 2 && _isWaitResponse) {
			std::lock_guard<std::mutex> lockGuard(_sendQueMutex);
			NET_MSG_S stFrontMsg = _sendMsgQue.front();
			_sendMsgQue.pop_front();
			bool stop = false;
			while (!stop) {
				NET_MSG_S stMsg = _sendMsgQue.front();
				if (stMsg == stFrontMsg) {
					_sendMsgQue.pop_front();
					if (_sendMsgQue.size() == 0) {
						stop = true;
					}
				}
				else {
					stop = true;
				}
			}

			return VIX_SUCCESS;
		}
		else if (_isWaitResponse) {
			return VIX_SUCCESS;
		}

		return VIX_FAILURE;
	}

	RET_E Client::getFrontOfSendMsgQue(NET_MSG_S* pstMsg)
	{
		if (_sendMsgQue.empty()) {
			pstMsg = nullptr;
		}
		else {
			pstMsg = &(_sendMsgQue.front());
		}

		return VIX_SUCCESS;
	}

	RET_E Client::sendMsg()
	{
		RET_E eRet = VIX_SUCCESS;

		if (_isWaitResponse) {
			return VIX_FAILURE;
		}

		if (_sendMsgQue.size()) {
			NET_MSG_S stMsg = _sendMsgQue.front();

			if (_eSocketType == NET_SOCKET_TYPE_E::HTTP) {
				eRet = sendHttpMsg(stMsg);
			}
			else if (_eSocketType == NET_SOCKET_TYPE_E::WEBSOCKET) {
				eRet = sendWsMsg(stMsg);
			}
			else {
				return eRet;
			}

			if (eRet == VIX_SUCCESS) {
				_isWaitResponse = true;
			}
		}

		return eRet;
	}

	RET_E Client::sendHttpMsg(const NET_MSG_S& stMsg)
	{
		if (!_pLwsCtx) {
			return VIX_FAILURE;
		}

		if (_stConnInfo.eSockType != NET_SOCKET_TYPE_E::HTTP) {
			return VIX_FAILURE;
		}

		lws_client_connect_info stClientConnInfo = { 0, };
		stClientConnInfo.context = _pLwsCtx;
		stClientConnInfo.host = lws_canonical_hostname(_pLwsCtx);

		stClientConnInfo.protocol = "http";
		stClientConnInfo.alpn = "http/1.1";
		stClientConnInfo.port = _stConnInfo.port;
		stClientConnInfo.userdata = this;
		stClientConnInfo.opaque_user_data = this;

		stClientConnInfo.address = _stConnInfo.host.c_str();
		stClientConnInfo.origin = stClientConnInfo.address;
		stClientConnInfo.path = stMsg.path.c_str();
		
		std::string methodStr = toMethodString(stMsg.eMethod);
		stClientConnInfo.method = methodStr.c_str();
		stClientConnInfo.pwsi = &_pLws;
		if (_name == "PluxityHttpClient") {
			VIXTRACE("Sended Msg : %s", stMsg.msg.c_str());
		}
		
		_pLws = lws_client_connect_via_info(&stClientConnInfo);
		if (!_pLws) {
			return VIX_FAILURE;
		}

		return VIX_SUCCESS;
	}

	RET_E Client::sendWsMsg(const NET_MSG_S& stMsg)
	{
		if (!_pLwsCtx) {
			return VIX_FAILURE;
		}

		if (_stConnInfo.eSockType != NET_SOCKET_TYPE_E::WEBSOCKET) {
			return VIX_FAILURE;
		}

		lws_client_connect_info stClientConnInfo = { 0, };
		stClientConnInfo.context = _pLwsCtx;
		stClientConnInfo.host = lws_canonical_hostname(_pLwsCtx);

		stClientConnInfo.protocol = _stConnInfo.subProtocolName.c_str();
		stClientConnInfo.port = _stConnInfo.port;
		stClientConnInfo.userdata = this;
		stClientConnInfo.opaque_user_data = this;

		stClientConnInfo.address = _stConnInfo.host.c_str();
		stClientConnInfo.origin = "origin";

		stClientConnInfo.path = stMsg.path.c_str();

		stClientConnInfo.pwsi = &_pLws;

		_pLws = lws_client_connect_via_info(&stClientConnInfo);
		if (!_pLws) {
			return VIX_FAILURE;
		}

		return VIX_SUCCESS;
	}

	int32 Client::callbackClientAppedHandshakeHeader(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len)
	{
		uint8** p = (uint8**)pIn;
		uint8* pEnd = (*p) + len;

		NET_MSG_S stMsg = _sendMsgQue.front();

		std::string contentsStr;
		if (stMsg.eContents == NET_CONTENTS_E::JSON) {
			contentsStr = "application/json";
		}

		if (_stConnInfo.eSockType == NET_SOCKET_TYPE_E::HTTP) {
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
			std::string keyName = "api-key:";
			if (lws_add_http_header_by_name(pLws, (uint8*)keyName.c_str(), (uint8*)keyStr.c_str(), keyStr.size(), p, pEnd)) {
				return -1;
			}
		}

		lws_client_http_body_pending(pLws, 1);
		lws_callback_on_writable(pLws);

		return 0;
	}

	int32 Client::callbackClientHttpWriteable(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len)
	{
		if (!_isWaitResponse) {
			return 0;
		}

		NET_MSG_S stMsg = _sendMsgQue.front();

		std::string msg = stMsg.msg;

		char* msgBuff = new char[LWS_PRE + msg.length() + 8];
		uint8* p = (uint8*)(msgBuff + LWS_PRE);
		memcpy(p, msg.c_str(), msg.size());
		lws_client_http_body_pending(pLws, 0);
		int32 writeLength = lws_write(pLws, p, msg.size(), LWS_WRITE_TEXT);
		delete[] msgBuff;

		return 0;
	}
}