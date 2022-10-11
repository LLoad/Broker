#ifndef _VIX_META_BROKER_PLUXITY_EVT_META_FORMAT_H_
#define _VIX_META_BROKER_PLUXITY_EVT_META_FORMAT_H_

#include <Common/JsonObject.h>
#include <JsonFormat/VixEvtMetaParser.h>
#include <JsonFormat/VixEvtAlmMetaParser.h>

namespace VixMetaBroker
{
	const int32 PLUXITY_EVT_OFF = 0;
	const int32 PLUXITY_EVT_ON = 3;

	class PluxityEvtMetaFmt : public JsonObject
	{
	public :
		PluxityEvtMetaFmt();
		virtual ~PluxityEvtMetaFmt();

	private :
		int32 _channel;

		JSON_VALUE_DECL(std::string, _dvcCd);
		JSON_VALUE_DECL(int32, _dvcStatus);
		JSON_VALUE_DECL(int32, _evtId);
		JSON_VALUE_DECL(std::string, _evtTime);
		JSON_VALUE_DECL(int32, _evtType);

	public :
		RET_E setDvcCd(const int32 dvcCd);
		RET_E setDvcStatus(const int32 dvcStatus);
		RET_E setEvtId(const int32 evtId);
		RET_E setEvtTime(const std::string& evtTime);
		RET_E setEvtType(const int32 evtType);

		RET_E makeJsonFromVixEvtMeta(const VixEvtMetaParser& parser);
		RET_E makeJsonFromVixEvtAlmMeta(const VixEvtAlmMetaParser& parser);

		std::string makeTimeFormatFromVix(const std::string& tmStr);

	public :
		RET_E createJson() override;


	};
}

#endif
