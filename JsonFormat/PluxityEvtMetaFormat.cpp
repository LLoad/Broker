#include <JsonFormat/PluxityEvtMetaFormat.h>

#include <algorithm>

namespace VixMetaBroker
{
	PluxityEvtMetaFmt::PluxityEvtMetaFmt()
		: _dvcCdName("dvcCd"), _dvcStatusName("dvcStatus"), _evtIdName("evtId"), _evtTimeName("evtTime"), _evtTypeName("evtType")
		, _dvcCd(""), _dvcStatus(0), _evtId(0), _evtTime(""), _evtType(0)
	{
		createJson();
	}

	PluxityEvtMetaFmt::~PluxityEvtMetaFmt()
	{

	}

	RET_E PluxityEvtMetaFmt::setDvcCd(const int32 dvcCd)
	{
		_channel = dvcCd;

		_dvcCd.clear();
		_dvcCd = "ITV_" + std::to_string(dvcCd);

		RET_E eRet = RET_E::SUCCESS;
		eRet = changeMember(_dvcCdName, _dvcCd);
		if (eRet != RET_E::SUCCESS) {
			return eRet;
		}

		return eRet;
	}

	RET_E PluxityEvtMetaFmt::setDvcStatus(const int32 dvcStatus)
	{
		_dvcStatus = dvcStatus;

		RET_E eRet = RET_E::SUCCESS;
		eRet = changeMember(_dvcStatusName, _dvcStatus);
		if (eRet != RET_E::SUCCESS) {
			return eRet;
		}

		return eRet;
	}

	RET_E PluxityEvtMetaFmt::setEvtId(const int32 evtId)
	{
		_evtId = evtId;

		RET_E eRet = RET_E::SUCCESS;
		eRet = changeMember(_evtIdName, _evtId);
		if (eRet != RET_E::SUCCESS) {
			return eRet;
		}

		return eRet;
	}

	RET_E PluxityEvtMetaFmt::setEvtTime(const std::string& evtTime)
	{
		_evtTime = evtTime;

		RET_E eRet = RET_E::SUCCESS;
		eRet = changeMember(_evtTimeName, _evtTime);
		if (eRet != RET_E::SUCCESS) {
			return eRet;
		}

		return eRet;
	}

	RET_E PluxityEvtMetaFmt::setEvtType(const int32 evtType)
	{
		_evtType = evtType;

		RET_E eRet = RET_E::SUCCESS;
		eRet = changeMember(_evtTypeName, _evtType);
		if (eRet != RET_E::SUCCESS) {
			return eRet;
		}

		return eRet;
	}

	RET_E PluxityEvtMetaFmt::makeJsonFromVixEvtMeta(const VixEvtMetaParser& parser)
	{
		setDvcCd(parser.uid());
		setEvtId(parser.evtId());

		int32 statNum = std::atoi(parser.stat().c_str());

		std::string timeStr;
		if (statNum == 0x00000004) {
			setDvcStatus(PLUXITY_EVT_OFF);
			timeStr = makeTimeFormatFromVix(parser.evtEndTime());
		}
		else {
			setDvcStatus(PLUXITY_EVT_ON);
			timeStr = makeTimeFormatFromVix(parser.evtStartTime());
		}

		setEvtTime(timeStr);
		setEvtType(parser.evtType());

		return VIX_SUCCESS;
	}

	RET_E PluxityEvtMetaFmt::makeJsonFromVixEvtAlmMeta(const VixEvtAlmMetaParser& parser)
	{
		setDvcCd(parser.uid());
		setEvtId(parser.evtId());

		std::string timeStr;
		setDvcStatus(PLUXITY_EVT_ON);
		timeStr = makeTimeFormatFromVix(parser.evtStartTime());

		setEvtTime(timeStr);
		setEvtType(parser.evtType());

		return VIX_SUCCESS;
	}

	std::string PluxityEvtMetaFmt::makeTimeFormatFromVix(const std::string& tmStr)
	{
		std::string newTimeStr = tmStr;
		if (tmStr.empty()) {
			return "";
		}

		newTimeStr.insert(4, "-");
		newTimeStr.insert(7, "-");
		std::replace(newTimeStr.begin(), newTimeStr.end(), 'T', ' ');
		newTimeStr.insert(13, ":");
		newTimeStr.insert(16, ":");
		newTimeStr.resize(19);

		return newTimeStr;
	}

	RET_E PluxityEvtMetaFmt::createJson()
	{
		JSON_CREAT_MEMBER(_dvcCd);
		JSON_CREAT_MEMBER(_dvcStatus);
		JSON_CREAT_MEMBER(_evtId);
		JSON_CREAT_MEMBER(_evtTime);
		JSON_CREAT_MEMBER(_evtType);

		return VIX_SUCCESS;
	}


}