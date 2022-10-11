#ifndef _VIX_META_BROKER_VIX_META_PARSER_H_
#define _VIX_META_BROKER_VIX_MeTA_PARSER_H_

#include <Common/Thread.h>
#include <Network/WsClient.h>
#include <JsonFormat/PluxityEvtMetaFormat.h>

#include <mutex>
#include <deque>

namespace VixMetaBroker
{
	class VixMetaParser : public Thread
	{
	public :
		VixMetaParser();
		virtual ~VixMetaParser();

	private :
		WsClient* _pWsClient;
		uint64 _totalCount;
		uint64 _evtMetaCount;
		uint64 _parsedCount;

		std::mutex _parsedQueLock;
		std::deque<std::string> _parsedQue;

	public :
		RET_E initialize(WsClient* pWsClient);
		std::string getFrontParsedMsg() { if (_parsedQue.size()) return _parsedQue.front(); else return ""; }
		void removeFrontParsedQue();

		uint64 totalCount() const noexcept { return _totalCount; }
		uint64 evtMetaCount() const noexcept { return _evtMetaCount; }
		uint64 parsedCount() const noexcept { return _parsedCount; }

	protected :
		void looperFunc() override;
		void addParsedQue(const std::string& parsedMsg);
		
	};
}

#endif