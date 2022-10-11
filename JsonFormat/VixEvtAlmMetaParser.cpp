#include <JsonFormat/VixEvtAlmMetaParser.h>

namespace VixMetaBroker
{
	VixEvtAlmMetaParser::VixEvtAlmMetaParser()
		: _uidName("uid"), _evtTypeName("type"), _evtIdName("id"), _evtStartTimeName("tm")
		, _uid(0), _evtType(0), _evtId(0), _evtStartTime("")
	{

	}

	VixEvtAlmMetaParser::VixEvtAlmMetaParser(const std::string& jsonStr)
		: _uidName("uid"), _evtTypeName("type"), _evtIdName("id"), _evtStartTimeName("tm")
		, _uid(0), _evtType(0), _evtId(0), _evtStartTime("")
	{
		parseEvtMeta(jsonStr);
	}

	VixEvtAlmMetaParser::~VixEvtAlmMetaParser()
	{

	}

	RET_E VixEvtAlmMetaParser::parseMemberValues()
	{
		JSON_GET_MEMBER(_uid);
		JSON_GET_MEMBER(_evtType);
		JSON_GET_MEMBER(_evtId);
		JSON_GET_MEMBER(_evtStartTime);

		return VIX_SUCCESS;
	}

	RET_E VixEvtAlmMetaParser::parseEvtMeta(const std::string& jsonStr)
	{
		_doc.Parse(jsonStr);

		std::string evtObj = getObjectMember("evtAlm");

		parseFromString(evtObj);

		return VIX_SUCCESS;
	}
}