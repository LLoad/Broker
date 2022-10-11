#include <System/VixMetaParser.h>
#include <Common/Logger.h>
#include <JsonFormat/VixEvtAlmMetaParser.h>

namespace VixMetaBroker
{
	VixMetaParser::VixMetaParser()
		: Thread()
		, _pWsClient(nullptr), _totalCount(0), _parsedCount(0), _evtMetaCount(0)
	{

	}

	VixMetaParser::~VixMetaParser()
	{
		quit();
		join();
	}

	RET_E VixMetaParser::initialize(WsClient* pClient)
	{
		if (!pClient) {
			return VIX_FAILURE;
		}

		_pWsClient = pClient;
		runThread();

		VIXERROR("VixMetaParser is initialize success..");
		return VIX_SUCCESS;
	}

	void VixMetaParser::looperFunc()
	{
		RcvData* pMetaData = _pWsClient->getFrontMeta();
		
		if (pMetaData != nullptr) {
			if (pMetaData->data() == nullptr || pMetaData->size() == 0) {
				pMetaData = nullptr;
				_pWsClient->removeFrontMeta();
			}
		}

		while (pMetaData) {
			_totalCount++;

			std::string msgHeader((char*)pMetaData->data(), 22);

			if (msgHeader.find("\"evt\"", 0) != std::string::npos) {
				_evtMetaCount++;
				std::string msg((char*)pMetaData->data(), pMetaData->size());

				//VixEvtAlmMetaParser evtMeta(msg);
				VixEvtMetaParser evtMeta(msg);

				PluxityEvtMetaFmt pluxityEvtFmt;
				pluxityEvtFmt.makeJsonFromVixEvtMeta(evtMeta);

				std::string pluxityEvtMetaStr = pluxityEvtFmt.toString();

				_parsedCount++;

				addParsedQue(pluxityEvtMetaStr);
			}
			
			_pWsClient->removeFrontMeta();
			pMetaData = _pWsClient->getFrontMeta();
			std::this_thread::sleep_for(std::chrono::microseconds(100));
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}

	void VixMetaParser::addParsedQue(const std::string& parsedMsg)
	{
		std::lock_guard<std::mutex> lg(_parsedQueLock);
		_parsedQue.push_back(parsedMsg);
	}

	void VixMetaParser::removeFrontParsedQue()
	{
		std::lock_guard<std::mutex> lg(_parsedQueLock);
		_parsedQue.pop_front();
	}
}