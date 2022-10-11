#include <JsonFormat/VixEvtMetaParser.h>

namespace VixMetaBroker
{
	VixEvtMetaParser::VixEvtMetaParser()
		: _uidName("uid"), _evtStatName("stat"), _evtTypeName("type"), _evtIdName("id"), _evtStartTimeName("tm"), _evtEndTimeName("etm")
		, _uid(0), _evtStat(""), _evtType(0), _evtId(0), _evtStartTime(""), _evtEndTime("")
	{

	}

	VixEvtMetaParser::VixEvtMetaParser(const std::string& jsonStr)
		: _uidName("uid"), _evtStatName("stat"), _evtTypeName("type"), _evtIdName("id"), _evtStartTimeName("tm"), _evtEndTimeName("etm")
		, _uid(0), _evtStat(""), _evtType(0), _evtId(0), _evtStartTime(""), _evtEndTime("")
	{
		parseEvtMeta(jsonStr);
	}

	VixEvtMetaParser::~VixEvtMetaParser()
	{

	}

	RET_E VixEvtMetaParser::parseMemberValues()
	{
		JSON_GET_MEMBER(_uid);
		JSON_GET_MEMBER(_evtStat);
		JSON_GET_MEMBER(_evtType);
		JSON_GET_MEMBER(_evtId);
		JSON_GET_MEMBER(_evtStartTime);
		JSON_GET_MEMBER(_evtEndTime);

		return VIX_SUCCESS;
	}

	RET_E VixEvtMetaParser::parseEvtMeta(const std::string& jsonStr)
	{
		_doc.Parse(jsonStr);

		std::string evtObj = getObjectMember("evt");

		parseFromString(evtObj);

		return VIX_SUCCESS;
	}
}