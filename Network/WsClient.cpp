#include <Network/WsClient.h>
#include <JsonFormat/PluxityEvtMetaFormat.h>
#include <Common/Logger.h>
#include <Network/VixMetaOption.h>

namespace VixMetaBroker
{
	RcvData::RcvData()
		: _pData(nullptr), _size(0)
	{
	}

	RcvData::RcvData(void* pData, int32 size)
		: _pData(nullptr), _size(size)
	{
		if (_pData) {
			delete[] _pData;
			_pData = nullptr;
		}

		if (size > 0) {
			_pData = new unsigned char[_size] {};
			std::memcpy(_pData, pData, _size);
		}
	}

	RcvData::RcvData(const RcvData& meta)
		: _pData(nullptr), _size(0)
	{
		*this = meta;
	}

	RcvData& RcvData::operator=(const RcvData& meta)
	{
		_size = meta._size;
		
		if (_pData) {
			delete[] _pData;
			_pData = nullptr;
		}

		if (_size > 0) {
			_pData = new unsigned char[_size] {};
			std::memcpy(_pData, meta._pData, _size);
		}
	
		return *this;
	}

	RcvData::~RcvData()
	{
		if (_pData) {
			delete[] _pData;
			_pData = nullptr;
		}
	}

	WsClient::WsClient()
		: Client(NET_SOCKET_TYPE_E::WEBSOCKET, "WsClient")
		, _isConnected(false), _isWaitBinary(false)
	{
		_rcvMetaQue.clear();
	}

	WsClient::~WsClient()
	{
		quit();
		join();
	}

	void WsClient::addMetaData(const RcvData& metaData)
	{
		std::lock_guard<std::mutex> lock(_rcvMetaQueMutex);
		RcvData* pRcvData = new RcvData(metaData);
		_rcvMetaQue.push_back(pRcvData);
	}

	void WsClient::removeFrontMeta()
	{
		std::lock_guard<std::mutex> lock(_rcvMetaQueMutex);
		RcvData* pRcvData = _rcvMetaQue.front();
		delete pRcvData;
		_rcvMetaQue.pop_front();
	}

	RcvData* WsClient::getFrontMeta()
	{
		if (_rcvMetaQue.size()) {
			return _rcvMetaQue.front();
		}
		else {
			//std::shared_ptr<RcvData> t = std::make_shared<RcvData>();
			return nullptr;
		}
	}

	int32 WsClient::callbackClientEstablished(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len)
	{
		_isConnected = true;
		removeSendMsg();

		return 0;
	}

	int32 WsClient::callbackClientRcv(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len)
	{
		RcvData recvMeta = RcvData(pIn, len);
		addMetaData(recvMeta);

		return 0;
	}

	int32 WsClient::callbackClientClosed(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len)
	{
		_isConnected = false;

		VIXERROR("Websocket closed");

		return 0;
	}
}