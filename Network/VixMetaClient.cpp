#include <Network/VixMetaClient.h>
#include <Common/Logger.h>
#include <Common/Util.h>
#include <memory>

namespace VixMetaBroker
{
	uint64 traceTick = 0;

	VixMetaClient::VixMetaClient()
		: _eStatus(STATUS_E::READY)
		, _sendedToPluxityCnt(0), _lastSendedMsg("")
	{

	}

	VixMetaClient::~VixMetaClient()
	{
		_vixRestClient.quit();
		_vixMetaParser.quit();
		_vixWsClient.quit();
	}

	RET_E VixMetaClient::initialize(VixServerInfo* pServerInfo)
	{
		if (pServerInfo == nullptr) {
			return VIX_FAILURE;
		}

		_pServerInfo = pServerInfo;

		initRestClient();

		runThread();

		VIXWARN("VixMetaClient Initialize Success...\n");
		return VIX_SUCCESS;
	}

	RET_E VixMetaClient::initRestClient()
	{
		NET_CONN_INFO_S stConnInfo;
		stConnInfo.host = "localhost";
		stConnInfo.port = 7681;
		stConnInfo.threadCnt = 10;
		stConnInfo.eSockType = NET_SOCKET_TYPE_E::HTTP;

		RET_E eRet = _vixRestClient.initialize(stConnInfo);
		if (eRet != VIX_SUCCESS) {
			return VIX_FAILURE;
		}

		NET_MSG_S stLoginMsg = makeLoginMsg();
		_vixRestClient.addSendMsg(stLoginMsg);

		setStatus(STATUS_E::REST_TRY_LOGIN);

		return VIX_SUCCESS;
	}

	RET_E VixMetaClient::initWsClient()
	{
		NET_CONN_INFO_S stConnInfo;
		stConnInfo.host = "localhost";
		stConnInfo.port = 7681;
		stConnInfo.threadCnt = 10;
		stConnInfo.eSockType = NET_SOCKET_TYPE_E::WEBSOCKET;
		stConnInfo.subProtocolName = "va-metadata";

		RET_E eRet = _vixWsClient.initialize(stConnInfo);
		if (eRet != VIX_SUCCESS) {
			return VIX_FAILURE;
		}

		setStatus(STATUS_E::INIT_WS_CLIENT);

		MetaOption vixMetaOpt;
		//vixMetaOpt.setMetaType(VIX_META_TYPE_E::EVENT_ALARM);
		vixMetaOpt.setMetaType(VIX_META_TYPE_E::EVENT);
		vixMetaOpt.setEvtMetaOpt(VIX_EVT_META_OPT_E::BEGUN | VIX_EVT_META_OPT_E::ENDED);
		requestMetaData(vixMetaOpt);

		return VIX_SUCCESS;
	}

	NET_MSG_S VixMetaClient::makeLoginMsg()
	{
		LoginFormat loginFmt;

		loginFmt.setId("intellivix");
		loginFmt.setPw("pass0001!");

		NET_MSG_S stMsg;
		stMsg.eCmd = NET_CMD_E::LOGIN;
		stMsg.eContents = NET_CONTENTS_E::JSON;
		stMsg.eMethod = NET_METHOD_E::POST;
		stMsg.msg = loginFmt.toString();
		stMsg.path = "/users/login";

		return stMsg;
	}

	NET_MSG_S VixMetaClient::makeRequestMetaMsg()
	{
		NET_MSG_S stWsMsg;
		stWsMsg.path = "/vaMetadata?api-key=";
		stWsMsg.path.append(_vixRestClient.apiKey());
		stWsMsg.path.append(_vixMetaOption.toString());
		stWsMsg.protocolName = "va-metadata";

		return stWsMsg;
	}

	NET_MSG_S VixMetaClient::makeKeepAliveMsg()
	{
		NET_MSG_S stMsg;
		stMsg.eCmd = NET_CMD_E::KEEP_ALIVE;
		stMsg.eContents = NET_CONTENTS_E::JSON;
		stMsg.eMethod = NET_METHOD_E::POST;
		stMsg.msg = "{}";
		stMsg.path = "/keepalive";

		return stMsg;
	}

	void VixMetaClient::looperFunc()
	{
		checkLoginProc();

		checkClientProc();

		//checkVixParserQue();
				
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	void VixMetaClient::checkLoginProc()
	{
		if (isStatusOn(STATUS_E::REST_TRY_LOGIN)) {
			if (_vixRestClient.sessInfo().loginTryCnt == MAX_TRY_LOGIN_CNT) {
				closeClient();
			}

			if (!_vixRestClient.apiKey().empty()) {
				initWsClient();
				VIXTRACE("Vix Meta Parser initialize...");
				_vixMetaParser.initialize(&_vixWsClient);
			}
		}
		else if (isStatusOn(STATUS_E::REQUEST_TERMINATE)) {
			quit();
		}
	}

	void VixMetaClient::checkClientProc()
	{
		if (!_vixWsClient.isConnected()) {
			// Websocket Disconnected
		}

		checkKeepAliveProc();
	}

	void VixMetaClient::checkKeepAliveProc()
	{
		if (_vixRestClient.isConnected()) {
			uint64 currTick = TickUtil::currentTick();
			uint64 diffTick = currTick - _vixRestClient.sessInfo().lastAliveTick;

			if (diffTick > (1000 * 60 * 14)) {
				NET_MSG_S stKeepAliveMsg = makeKeepAliveMsg();
				_vixRestClient.addSendMsg(stKeepAliveMsg);
				_vixRestClient.updateAliveTick(currTick);
				VIXERROR("Send KeepAlive (currTick : %llu)", TickUtil::currentTick());
			}
		}
	}

	void VixMetaClient::checkVixParserQue()
	{
		std::string frontMsg = _vixMetaParser.getFrontParsedMsg();

		if (!frontMsg.empty()) {
			// Send To Pluxity Server
			_sendedToPluxityCnt++;
			_lastSendedMsg = frontMsg;

			_vixMetaParser.removeFrontParsedQue();
		}

		uint64 currTick = TickUtil::currentTick();
		if (traceTick == 0 || currTick - traceTick > (1000 * 60)) {
			VIXTRACE("Sended To Pluxity Information - SendMsgCnt : %llu, LastSendMsg : %s", _sendedToPluxityCnt, _lastSendedMsg.c_str());
			traceTick = currTick;
		}
	}

	std::string VixMetaClient::getFrontMetaDataForPluxity()
	{
		return _vixMetaParser.getFrontParsedMsg();
	}

	void VixMetaClient::removeFrontMetaData()
	{
		_vixMetaParser.removeFrontParsedQue();
	}

	void VixMetaClient::closeClient()
	{
		_vixRestClient.quit();
		
		setStatus(STATUS_E::REQUEST_TERMINATE);
	}

	void VixMetaClient::requestMetaData(const MetaOption& metaOpt)
	{
		if (_eStatus < STATUS_E::INIT_WS_CLIENT) {
			return;
		}

		if (_vixMetaOption == metaOpt) {
			return;
		}

		_vixMetaOption = metaOpt;

		NET_MSG_S stMsg = makeRequestMetaMsg();
		VIXERROR("Request MetaData : %s", stMsg.path.c_str());
		_vixWsClient.setMetaOption(_vixMetaOption);
		_vixWsClient.addSendMsg(stMsg);

		setStatus(STATUS_E::REQUEST_METADATA);
	}
}