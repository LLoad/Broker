#include <Setup/VixServerInfo.h>

#define RAPIDJSON_HAS_STDSTRING		1

namespace VixMetaBroker
{
	VixServerInfo::VixServerInfo(uint32 serverId)
		: _serverIdName("ServerID"), _serverId(serverId)
		, _ipAddrName("IP"), _ipAddr("localhost")
		, _portName("Port"), _port(7681)
		, _rcvQueCntName("RcvQueCntFromServer"), _rcvQueCnt(1)
		, _loginInfoName("LoginInfo")
	{
		createJson();
	}

	VixServerInfo::VixServerInfo(const VixServerInfo& info)
		: _serverIdName("ServerID"), _serverId(0)
		, _ipAddrName("IP"), _ipAddr("localhost")
		, _portName("Port"), _port(7681)
		, _rcvQueCntName("RcvQueCntFromServer"), _rcvQueCnt(1)
		, _loginInfoName("LoginInfo")
	{
		*this = info;
	}

	VixServerInfo::~VixServerInfo()
	{

	}

	RET_E VixServerInfo::createJson()
	{
		_doc.SetObject();

		JSON_CREAT_MEMBER(_serverId);
		JSON_CREAT_MEMBER(_ipAddr);
		JSON_CREAT_MEMBER(_port);
		JSON_CREAT_MEMBER(_rcvQueCnt);

		RET_E eRet = VIX_SUCCESS;
		eRet = createObject(_loginInfoName);
		if (eRet != VIX_SUCCESS) {
			return VIX_FAILURE;
		}

		eRet = changeObject(_loginInfoName, _loginInfo.getConvertedValueFromDoc());
		if (eRet != VIX_SUCCESS) {
			return VIX_FAILURE;
		}

		return VIX_SUCCESS;
	}

	RET_E VixServerInfo::parseMemberValues()
	{
		JSON_GET_MEMBER(_serverId);
		JSON_GET_MEMBER(_ipAddr);
		JSON_GET_MEMBER(_port);
		JSON_GET_MEMBER(_rcvQueCnt);

		std::string objStr = getObjectMember(_loginInfoName);

		_loginInfo.parseFromString(objStr);

		return VIX_SUCCESS;
	}

	VixServerInfo& VixServerInfo::operator=(const VixServerInfo& info)
	{
		_ipAddr = info._ipAddr;
		_port = info._port;
		_loginInfo = info._loginInfo;

		createJson();

		return *this;
	}

	bool VixServerInfo::operator==(const VixServerInfo& info)
	{
		if (_ipAddr == info._ipAddr &&
			_port == info._port &&
			_loginInfo == info._loginInfo)
		{
			return true;
		}
		else {
			return false;
		}
	}
}