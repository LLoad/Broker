#ifndef _VIX_META_BROKER_CLIENT_H_
#define _VIX_META_BROKER_CLIENT_H_

#include <Common/Type.h>
#include <Common/Thread.h>
#include <Network/NetworkDefine.h>

#include <libwebsockets.h>

#include <deque>
#include <mutex>

#define VIX_LWS_SERVICE_BUFF_SIZE		8192

namespace VixMetaBroker
{
	class Client : public Thread
	{
	public :
		Client(const NET_SOCKET_TYPE_E eSockType = NET_SOCKET_TYPE_E::NONE, const std::string& name = "");
		virtual ~Client();

	private :
		std::string _name;
		NET_TYPE_E _eType;
		NET_SOCKET_TYPE_E _eSocketType;
		NET_CONN_INFO_S _stConnInfo;

		lws_context* _pLwsCtx;
		lws* _pLws;
		
	protected :
		std::deque<NET_MSG_S> _sendMsgQue;
		std::mutex _sendQueMutex;
		bool _isWaitResponse;

	public :
		const NET_CONN_INFO_S& connectInfo() const noexcept { return _stConnInfo; }

	public :
		virtual RET_E initialize(const NET_CONN_INFO_S& stConnInfo);
		virtual int32 clientCallbackFunc(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len);

		RET_E addSendMsg(const NET_MSG_S& stMsg);
		RET_E removeSendMsg();
		RET_E getFrontOfSendMsgQue(NET_MSG_S* pstMsg);
		int32 getSendMsgQueSize() { return _sendMsgQue.size(); }

	protected :
		void looperFunc() override;

		RET_E createLwsCtx();
		RET_E destroyLwsCtx();

		virtual void makeLwsProtocol(const lws_protocols** ppstProtocols);
		virtual RET_E sendMsg();
		virtual RET_E sendHttpMsg(const NET_MSG_S& stMsg);
		virtual RET_E sendWsMsg(const NET_MSG_S& stMsg);

	protected:
		virtual int32 callbackClientEstablished(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len) { return 0; }
		virtual int32 callbackClientEstablishedHttp(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len) { return 0; }
		virtual int32 callbackClientAppedHandshakeHeader(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len);
		virtual int32 callbackClientHttpWriteable(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len);
		virtual int32 callbackClientRcv(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len) { return 0; }
		virtual int32 callbackClientRcvHttp(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len) { return 0; }
		virtual int32 callbackClientRcvHttpRead(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len) { return 0; }
		virtual int32 callbackClientHttpClosed(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len) { return 0; }
		virtual int32 callbackClientClosed(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len) { return 0; }
		virtual int32 callbackClosed(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len) { return 0; }
		virtual int32 callbackConnectionError(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len) { return 0; }
	};
}

#endif
