#include <Setup/PluxityServerInfo.h>

#ifndef RAPIDJSON_HAS_STDSTRING
	#define RAPIDJSON_HAS_STDSTRING		1
#endif

namespace VixMetaBroker
{
	PluxityServerInfo::PluxityServerInfo()
		: _ipAddrName("IP"), _ipAddr("27.102.205.66")
		, _portName("port"), _port(8130)
		, _evtUrlName("evt_url"), _evtUrl("itf/api/event/cctv")
		, _apiKeyName("api-key"), _apiKey("")
		, _loginInfoName("LoginInfo")
	{
		createJson();
	}

	PluxityServerInfo::PluxityServerInfo(const PluxityServerInfo& info)
		: _ipAddrName("IP"), _ipAddr("27.102.205.66")
		, _portName("port"), _port(8130)
		, _evtUrlName("evt_url"), _evtUrl("itf/api/event/cctv")
		, _apiKeyName("api-key"), _apiKey("")
		, _loginInfoName("LoginInfo")
	{
		*this = info;
	}

	PluxityServerInfo::~PluxityServerInfo()
	{

	}

	RET_E PluxityServerInfo::createJson()
	{
		_doc.SetObject();

		JSON_CREAT_MEMBER(_ipAddr);
		JSON_CREAT_MEMBER(_port);
		JSON_CREAT_MEMBER(_evtUrl);
		JSON_CREAT_MEMBER(_apiKey);

		RET_E eRet = createObject(_loginInfoName);
		if (eRet != VIX_SUCCESS) {
			return VIX_FAILURE;
		}

		eRet = changeObject(_loginInfoName, _loginInfo.getConvertedValueFromDoc());
		if (eRet != VIX_SUCCESS) {
			return VIX_FAILURE;
		}
		
		return VIX_SUCCESS;
	}

	RET_E PluxityServerInfo::parseMemberValues()
	{
		JSON_GET_MEMBER(_ipAddr);
		JSON_GET_MEMBER(_port);
		JSON_GET_MEMBER(_evtUrl);
		JSON_GET_MEMBER(_apiKey);

		std::string objStr = getObjectMember(_loginInfoName);

		_loginInfo.parseFromString(objStr);

		return VIX_SUCCESS;
	}

	PluxityServerInfo& PluxityServerInfo::operator=(const PluxityServerInfo& info)
	{
		_ipAddr = info._ipAddr;
		_port = info._port;
		_evtUrl = info._evtUrl;
		_apiKey = info._apiKey;
		_loginInfo = info._loginInfo;

		createJson();

		return *this;
	}

	bool PluxityServerInfo::operator==(const PluxityServerInfo& info)
	{
		if (_ipAddr == info._ipAddr &&
			_port == info._port &&
			_evtUrl == info._evtUrl &&
			_apiKey == info._apiKey &&
			_loginInfo == info._loginInfo)
		{
			return true;
		}
		else {
			return false;
		}
	}
}