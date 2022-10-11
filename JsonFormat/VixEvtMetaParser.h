#ifndef _VIX_META_BROKER_VIX_EVT_META_PARSER_H_
#define _VIX_META_BROKER_VIX_EVT_META_PARSER_H_

#include <Common/JsonObject.h>

namespace VixMetaBroker
{
	class VixEvtMetaParser : public JsonObject
	{
	public :
		VixEvtMetaParser();
		VixEvtMetaParser(const std::string& jsonStr);
		virtual ~VixEvtMetaParser();

	private :
		JSON_VALUE_DECL(int32, _uid);
		JSON_VALUE_DECL(std::string, _evtStat);
		JSON_VALUE_DECL(int32, _evtType);
		JSON_VALUE_DECL(int32, _evtId);
		JSON_VALUE_DECL(std::string, _evtStartTime);
		JSON_VALUE_DECL(std::string, _evtEndTime);

	public :
		int32 uid() const noexcept { return _uid; }
		const std::string& stat() const noexcept { return _evtStat; }
		int32 evtType() const noexcept { return _evtType; }
		int32 evtId() const noexcept { return _evtId; }
		const std::string& evtStartTime() const noexcept { return _evtStartTime; }
		const std::string& evtEndTime() const noexcept { return _evtEndTime; }

	public :
		RET_E createJson() override { return VIX_SUCCESS; }
		RET_E parseEvtMeta(const std::string& jsonStr);

	protected :
		RET_E parseMemberValues() override;

	};
}

#endif
