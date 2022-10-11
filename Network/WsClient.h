#ifndef _VIX_META_BROKER_WS_CLIENT_H_
#define _VIX_META_BROKER_WS_CLIENT_H_

#include <Network/Client.h>
#include <Network/VixMetaOption.h>

#include <deque>
#include <memory>

#define JSON_HEAD_FRM_META			"\"frm\""
#define JSON_HEAD_EVT_META			"\"evt\""
#define JSON_HEAD_EVT_ALM_META		"\"evtAlm\""
#define JSON_HEAD_FACE_META			"\"face\""
#define JSON_HEAD_PLATE_META		"\"plate\""
#define JSON_HEAD_EVT_ZONE_META		"\"evtZone\""
#define JSON_HEAD_EVT_ZONE_CNT_META	"\"evtZoneCnt\""
#define JSON_HEAD_VEHI_QUE_META		"\"vehiQueue\""
#define JSON_HEAD_CROSSWALK_META	"\"CrossWalkStatistics\""
#define JSON_HEAD_CAM_STATE_META	"\"camStatus\""

namespace VixMetaBroker
{
	class RcvData
	{
	public :
		RcvData();
		RcvData(void* pData, int32 size);
		RcvData(const RcvData& meta);
		virtual ~RcvData();

	private :
		unsigned char* _pData;
		int32 _size;

	public :
		unsigned char* data() noexcept { return _pData; }
		int32 size() const noexcept { return _size; }

	public :
		RcvData& operator=(const RcvData& rcvData);
	};

	class WsClient : public Client
	{
	public :
		WsClient();
		virtual ~WsClient();

	private :
		bool _isConnected;
		
		std::mutex _rcvMetaQueMutex;
		std::deque<RcvData*> _rcvMetaQue;
		MetaOption _metaOption;
		bool _isWaitBinary;
		
	public :
		bool isConnected() const noexcept { return _isConnected; }

	private :
		void addMetaData(const RcvData& metaData);

	public :
		void removeFrontMeta();
		RcvData* getFrontMeta();
		size_t getMetaQueSize() { return _rcvMetaQue.size(); }
		
	public :
		void setMetaOption(const MetaOption& opt) noexcept { _metaOption = opt; }

	protected :
		int32 callbackClientEstablished(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len) override;
		int32 callbackClientRcv(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len) override;
		int32 callbackClientClosed(lws* pLws, lws_callback_reasons eReason, void* pIn, size_t len) override;
	};
}

#endif
