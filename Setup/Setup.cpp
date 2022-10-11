#include <Setup/Setup.h>

namespace VixMetaBroker
{
	Setup::Setup()
		: _fileName("VixBrokerSetup.json")
		, _versionName("Version"), _version("0.0.1")
		, _revisionName("Revision"), _revision("0001")
		, _setupVersionName("SetupVersion"), _setupVersion("0.0.1")
		, _setupRevisionName("SetupRevision"), _setupRevision("0001")
		, _vixServerCountName("VixCount"), _vixServerCount(0)
		, _vixServerInfoName("VixServerInfo")
		, _pluxityServerInfoName("PluxityServerInfo")
	{
		_vixServerInfos.clear();
	}

	Setup::~Setup()
	{
		write();
	}

	void Setup::addVixServerInfo(const VixServerInfo& info)
	{
		_vixServerInfos.push_back(info);

		VixServerInfo& temp = const_cast<VixServerInfo&>(info);
		RET_E eRet = pushObjectArrayMember(_vixServerInfoName, temp.getConvertedValueFromDoc());
		if (eRet != VIX_SUCCESS) {
			int32 a = 0;
		}

		_vixServerCount = _vixServerInfos.size();
		JSON_CHANGE_MEMBER(_vixServerCount);
	}

	VixServerInfo* Setup::getServerInfoByIdx(int32 idx)
	{
		if (_vixServerInfos.size() < idx) {
			return nullptr;
		}

		return &_vixServerInfos[idx];
	}

	RET_E Setup::load()
	{
		RET_E eRet = readFile(_fileName);
		if (eRet != VIX_SUCCESS) {
			eRet = createJson();
			if (eRet != VIX_SUCCESS) {
				return VIX_FAILURE;
			}

			eRet = writeFile(_fileName);
			if (eRet != VIX_SUCCESS) {
				return VIX_FAILURE;
			}
		}

		eRet = parseMemberValues();
		if (eRet != VIX_SUCCESS) {
			return VIX_FAILURE;
		}

		return VIX_SUCCESS;
	}

	RET_E Setup::write()
	{
		RET_E eRet = VIX_SUCCESS;

		eRet = writeFile(_fileName);
		if (eRet != VIX_SUCCESS) {
			return VIX_FAILURE;
		}

		return eRet;
	}

	RET_E Setup::createJson()
	{
		_doc.SetObject();

		JSON_CREAT_MEMBER(_version);
		JSON_CREAT_MEMBER(_revision);
		JSON_CREAT_MEMBER(_vixServerCount);

		RET_E eRet = createArray(_vixServerInfoName);
		if (eRet != VIX_SUCCESS) {
			return VIX_FAILURE;
		}

		eRet = createObject(_pluxityServerInfoName);
		if (eRet != VIX_SUCCESS) {
			return VIX_FAILURE;
		}
		
		eRet = changeObject(_pluxityServerInfoName, _pluxityServerInfo.getConvertedValueFromDoc());
		if (eRet != VIX_SUCCESS) {
			return VIX_FAILURE;
		}
		
		return VIX_SUCCESS;
	}

	RET_E Setup::parseMemberValues()
	{
		RET_E eRet = VIX_SUCCESS;
		JSON_GET_MEMBER(_version);
		JSON_GET_MEMBER(_revision);
		JSON_CHECK_AND_CREATE_MEMBER(_setupVersion)
		JSON_CHECK_AND_CREATE_MEMBER(_setupRevision)
		JSON_GET_MEMBER(_vixServerCount);

		if (_vixServerCount != 0) {
			for (int32 i = 0; i < _vixServerCount; i++) {
				std::string objStr = getObjectArrayMember(_vixServerInfoName, i);

				VixServerInfo info;
				info.parseFromString(objStr);

				_vixServerInfos.push_back(info);
			}
		}

		std::string objStr = getObjectMember(_pluxityServerInfoName);
		_pluxityServerInfo.parseFromString(objStr);

		return VIX_SUCCESS;
	}

	Setup& Setup::operator=(const Setup& setup)
	{
		_version = setup._version;
		_revision = setup._revision;
		_vixServerCount = setup._vixServerCount;

		_pluxityServerInfo = setup._pluxityServerInfo;

		createJson();

		return *this;
	}

	bool Setup::operator==(const Setup& setup) noexcept
	{
		if (_version == setup._version &&
			_revision == setup._revision &&
			_pluxityServerInfo == setup._pluxityServerInfo)
		{
			if (_vixServerCount != setup._vixServerCount) {
				return false;
			}
			else {
				return true;
			}
		}
		else {
			return false;
		}
	}
}