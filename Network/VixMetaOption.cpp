#include <Network/VixMetaOption.h>
#include <string>

namespace VixMetaBroker
{
	MetaOption::MetaOption()
		: _eMetaType(VIX_META_TYPE_E::NONE), _eFrmMetaOpt(VIX_FRM_META_OPT_E::NONE), _eEvtMetaOpt(VIX_EVT_META_OPT_E::NONE)
		, _eEvtMetaSubOpt(VIX_EVT_META_SUB_OPT_E::NONE), _eEvtAlmMetaOpt(VIX_EVT_ALM_META_OPT_E::NONE)
		, _eFaceMetaOpt(VIX_FACE_META_OPT_E::NONE), _ePlateMetaOpt(VIX_PLATE_META_OPT_E::NONE)
	{
		VIX_META_TYPE_E test;
		//test = VIX_META_TYPE_E::FRAME | VIX_META_TYPE_E::CAMERA_STATE;
		int32 a = 0;
	}

	MetaOption::~MetaOption()
	{

	}

	bool MetaOption::isChannelSelected(int32 ch) const noexcept
	{
		if (isAllChannelSelected()) {
			return true;
		}

		if (_selectedCh[ch]) {
			return true;
		}
		else {
			return false;
		}
	}

	bool MetaOption::isAllChannelSelected() const noexcept
	{
		if (_selectedCh[MAX_VIX_CH_CNT]) {
			return true;
		}
		else {
			return false;
		}
	}

	void MetaOption::addSelectedCh(int32 ch) noexcept
	{
		if (ch < MAX_VIX_CH_CNT) {
			_selectedCh[ch] = true;
		}

		_selectedCh[MAX_VIX_CH_CNT] = false;
	}

	void MetaOption::removeSelectedCh(int32 ch) noexcept
	{
		if (ch < MAX_VIX_CH_CNT) {
			_selectedCh[ch] = false;
		}
	}

	std::string MetaOption::getMetaTypeString(const VIX_META_TYPE_E eMetaType)
	{
		switch (eMetaType) {
			case VIX_META_TYPE_E::FRAME:			return "frmMeta";
			case VIX_META_TYPE_E::EVENT:			return "evtMeta";
			case VIX_META_TYPE_E::EVENT_ALARM:		return "evtAlmMeta";
			case VIX_META_TYPE_E::FACE:				return "faceMeta";
			case VIX_META_TYPE_E::PLATE:			return "plateMeta";
			case VIX_META_TYPE_E::EVENT_ZONE:		return "evtZoneMeta";
			case VIX_META_TYPE_E::EVENT_ZONE_CNT:	return "evtZoneCntMeta";
			case VIX_META_TYPE_E::VEHICHLE_QUE:		return "vehiQueueMeta";
			case VIX_META_TYPE_E::CROSSWALK_STAT:   return "crossWalkStatMeta";
			case VIX_META_TYPE_E::CAMERA_STATE:		return "cameraStateMeta";
			default:								return "";
		}
	}

	std::string MetaOption::getOptStr(const VIX_FRM_META_OPT_E eOpt)
	{
		switch (eOpt)
		{
			case VIX_FRM_META_OPT_E::COLOR: return "color";
			default:						return "";
		}
	}

	std::string MetaOption::getOptStr(const VIX_EVT_META_OPT_E eOpt)
	{
		switch (eOpt)
		{
			case VIX_EVT_META_OPT_E::BEGUN: return "begun";
			case VIX_EVT_META_OPT_E::ENDED: return "ended";
			case VIX_EVT_META_OPT_E::IMAGE: return "evtImg";
			case VIX_EVT_META_OPT_E::COLOR: return "color";
			default:						return "";
		}
	}

	std::string MetaOption::getOptStr(const VIX_EVT_META_SUB_OPT_E eOpt)
	{
		switch (eOpt)
		{
			case VIX_EVT_META_SUB_OPT_E::GID: return "gid";
			default:						  return "";
		}
	}

	std::string MetaOption::getOptStr(const VIX_EVT_ALM_META_OPT_E eOpt)
	{
		switch (eOpt)
		{
			case VIX_EVT_ALM_META_OPT_E::IMAGE:		 return "evtImg";
			case VIX_EVT_ALM_META_OPT_E::FULL_IMAGE: return "fullImg";
			case VIX_EVT_ALM_META_OPT_E::OBJ_INFO:	 return "objInfo";
			default:								 return "";
		}
	}


	std::string MetaOption::getOptStr(const VIX_FACE_META_OPT_E eOpt)
	{
		switch (eOpt)
		{
			case VIX_FACE_META_OPT_E::BEGUN:				return "detectBegun";
			case VIX_FACE_META_OPT_E::ENDED:				return "detectEnded";
			case VIX_FACE_META_OPT_E::ATTR_UPDATED:			return "attrUpdated";
			case VIX_FACE_META_OPT_E::RECOG_REGISTERED:		return "recogRegistered";
			case VIX_FACE_META_OPT_E::RECOG_UNREGISTERED:	return "recogUnregistered";
			case VIX_FACE_META_OPT_E::IMAGE:				return "faceImg";
			default:										return "";
		}
	}

	std::string MetaOption::getOptStr(const VIX_PLATE_META_OPT_E eOpt)
	{
		switch (eOpt)
		{
			case VIX_PLATE_META_OPT_E::IMAGE:			return "plateImg";
			case VIX_PLATE_META_OPT_E::FULL_TN_IMAGE:	return "fullTNImg";
			case VIX_PLATE_META_OPT_E::FULL_IMAGE:		return "fullImg";
			case VIX_PLATE_META_OPT_E::PACKAGE:			return "package";
			default:									return "";
		}
	}


#define CHECK_AND_MAKE_META_STRING(string_value, meta_type, make_func)	\
	if (isMetaTypeEnable(meta_type)) {									\
		string_value += "&";											\
		string_value.append(make_func());								\
	}
	
	std::string MetaOption::toString()
	{
		std::string result;

		result.clear();

		CHECK_AND_MAKE_META_STRING(result, VIX_META_TYPE_E::FRAME, makeFrameMetaString);
		CHECK_AND_MAKE_META_STRING(result, VIX_META_TYPE_E::EVENT, makeEventMetaString);
		CHECK_AND_MAKE_META_STRING(result, VIX_META_TYPE_E::EVENT_ALARM, makeEventAlmMetaString);
		CHECK_AND_MAKE_META_STRING(result, VIX_META_TYPE_E::FACE, makeFaceMetaString);
		CHECK_AND_MAKE_META_STRING(result, VIX_META_TYPE_E::PLATE, makePlateMetaString);
		CHECK_AND_MAKE_META_STRING(result, VIX_META_TYPE_E::EVENT_ZONE, makeEventZoneMetaString);
		CHECK_AND_MAKE_META_STRING(result, VIX_META_TYPE_E::EVENT_ZONE_CNT, makeEventZoneCountMetaString);
		CHECK_AND_MAKE_META_STRING(result, VIX_META_TYPE_E::VEHICHLE_QUE, makeVehicleQueMetaString);
		CHECK_AND_MAKE_META_STRING(result, VIX_META_TYPE_E::CAMERA_STATE, makeCameraStateMetaString);
		
		result.append(makeSelectedChannelString());

		return result;
	}

	std::string MetaOption::makeFrameMetaString()
	{
		std::string result;

		result = getMetaTypeString(VIX_META_TYPE_E::FRAME);

		if (_eFrmMetaOpt != VIX_FRM_META_OPT_E::NONE) {
			result += "=";
			if (isOptionEnable(VIX_FRM_META_OPT_E::COLOR)) {
				result += getOptStr(VIX_FRM_META_OPT_E::COLOR);
			}
		}

		return result;
	}

#define CHECK_AND_MAKE_META_OPTION(string_value, opt_type)		\
	if (isOptionEnable(opt_type)) {								\
		if (!string_value.empty()) {							\
			string_value += ",";								\
		}														\
		string_value.append(getOptStr(opt_type));				\
	}

	std::string MetaOption::makeEventMetaString()
	{
		std::string result;

		result = getMetaTypeString(VIX_META_TYPE_E::EVENT);

		std::string optStr;
		if (_eEvtMetaOpt != VIX_EVT_META_OPT_E::NONE) {
			CHECK_AND_MAKE_META_OPTION(optStr, VIX_EVT_META_OPT_E::BEGUN);
			CHECK_AND_MAKE_META_OPTION(optStr, VIX_EVT_META_OPT_E::ENDED);
			CHECK_AND_MAKE_META_OPTION(optStr, VIX_EVT_META_OPT_E::IMAGE);
			CHECK_AND_MAKE_META_OPTION(optStr, VIX_EVT_META_OPT_E::COLOR);
		}

		std::string subOptStr;
		if (_eEvtMetaSubOpt != VIX_EVT_META_SUB_OPT_E::NONE) {
			if (isOptionEnable(VIX_EVT_META_SUB_OPT_E::GID)) {
				subOptStr.assign("&options=gid");
			}
		}
		

		if (!optStr.empty()) {
			result += "=";
			result.append(optStr);

			if (!subOptStr.empty()) {
				result.append(subOptStr);
			}
		}

		return result;
	}

	std::string MetaOption::makeEventAlmMetaString()
	{
		std::string result;

		result = getMetaTypeString(VIX_META_TYPE_E::EVENT_ALARM);

		std::string optStr;
		if (_eEvtAlmMetaOpt != VIX_EVT_ALM_META_OPT_E::NONE) {
			CHECK_AND_MAKE_META_OPTION(optStr, VIX_EVT_ALM_META_OPT_E::IMAGE);
			CHECK_AND_MAKE_META_OPTION(optStr, VIX_EVT_ALM_META_OPT_E::FULL_IMAGE);
			CHECK_AND_MAKE_META_OPTION(optStr, VIX_EVT_ALM_META_OPT_E::OBJ_INFO);
		}

		if (!optStr.empty()) {
			result += "=";
			result.append(optStr);
		}

		return result;
	}

	std::string MetaOption::makeFaceMetaString()
	{
		std::string result;

		result = getMetaTypeString(VIX_META_TYPE_E::FACE);

		std::string optStr;
		if (_eFaceMetaOpt != VIX_FACE_META_OPT_E::NONE) {
			CHECK_AND_MAKE_META_OPTION(optStr, VIX_FACE_META_OPT_E::BEGUN);
			CHECK_AND_MAKE_META_OPTION(optStr, VIX_FACE_META_OPT_E::ENDED);
			CHECK_AND_MAKE_META_OPTION(optStr, VIX_FACE_META_OPT_E::ATTR_UPDATED);
			CHECK_AND_MAKE_META_OPTION(optStr, VIX_FACE_META_OPT_E::RECOG_REGISTERED);
			CHECK_AND_MAKE_META_OPTION(optStr, VIX_FACE_META_OPT_E::RECOG_UNREGISTERED);
			CHECK_AND_MAKE_META_OPTION(optStr, VIX_FACE_META_OPT_E::IMAGE);
		}

		if (!optStr.empty()) {
			result += "=";
			result.append(optStr);
		}

		return result;
	}

	std::string MetaOption::makePlateMetaString()
	{
		std::string result;

		result = getMetaTypeString(VIX_META_TYPE_E::PLATE);

		std::string optStr;
		if (_ePlateMetaOpt != VIX_PLATE_META_OPT_E::NONE) {
			CHECK_AND_MAKE_META_OPTION(optStr, VIX_PLATE_META_OPT_E::IMAGE);
			CHECK_AND_MAKE_META_OPTION(optStr, VIX_PLATE_META_OPT_E::FULL_TN_IMAGE);
			CHECK_AND_MAKE_META_OPTION(optStr, VIX_PLATE_META_OPT_E::FULL_IMAGE);
		}

		if (!optStr.empty()) {
			result += "=";
			result.append(optStr);
		}

		return result;
	}

	std::string MetaOption::makeEventZoneMetaString()
	{
		std::string result;

		result = getMetaTypeString(VIX_META_TYPE_E::EVENT_ZONE);

		return result;
	}

	std::string MetaOption::makeEventZoneCountMetaString()
	{
		std::string result;

		result = getMetaTypeString(VIX_META_TYPE_E::EVENT_ZONE_CNT);

		return result;
	}

	std::string MetaOption::makeVehicleQueMetaString()
	{
		std::string result;

		result = getMetaTypeString(VIX_META_TYPE_E::VEHICHLE_QUE);

		return result;
	}

	std::string MetaOption::makeCameraStateMetaString()
	{
		std::string result;

		result = getMetaTypeString(VIX_META_TYPE_E::CAMERA_STATE);

		return result;
	}

	std::string MetaOption::makeSelectedChannelString()
	{
		std::string result = "";
		if (_selectedCh[MAX_VIX_CH_CNT]) {
			return result;
		}

		std::string chStr;
		for (int32 i = 0; i < MAX_VIX_CH_CNT; i++) {
			if (_selectedCh[i]) {
				if (!chStr.empty()) {
					chStr += ",";
				}
				chStr.append(std::to_string(i));
			}
		}

		if (!chStr.empty()) {
			result += "&ch=";
			result.append(chStr);
		}

		return result;
	}

	MetaOption& MetaOption::operator=(const MetaOption& opt)
	{
		_eMetaType = opt._eMetaType;
		_eFrmMetaOpt = opt._eFrmMetaOpt;
		_eEvtMetaOpt = opt._eEvtMetaOpt;
		_eEvtMetaSubOpt = opt._eEvtMetaSubOpt;
		_eEvtAlmMetaOpt = opt._eEvtAlmMetaOpt;
		_eFaceMetaOpt = opt._eFaceMetaOpt;
		_ePlateMetaOpt = opt._ePlateMetaOpt;

		for (int32 i = 0; i < MAX_VIX_CH_CNT; i++) {
			_selectedCh[i] = opt._selectedCh[i];
		}

		return *this;
	}

	bool MetaOption::operator==(const MetaOption& opt)
	{
		if (_eMetaType == opt._eMetaType &&
			_eFrmMetaOpt == opt._eFrmMetaOpt &&
			_eEvtMetaOpt == opt._eEvtMetaOpt &&
			_eEvtMetaSubOpt == opt._eEvtMetaSubOpt &&
			_eEvtAlmMetaOpt == opt._eEvtAlmMetaOpt &&
			_eFaceMetaOpt == opt._eFaceMetaOpt &&
			_ePlateMetaOpt == opt._ePlateMetaOpt)
		{
			for (int32 i = 0; i < MAX_VIX_CH_CNT; i++) {
				if (_selectedCh[i] != opt._selectedCh[i]) {
					return false;
				}
			}

			return true;
		}
		else {
			return false;
		}
	}
}