#include <JsonFormat/ApiKeyFormat.h>

namespace VixMetaBroker
{
	ApiKeyFormat::ApiKeyFormat()
		: _apiKeyName("api-key"), _apiKey("")
	{
		createJson();
	}

	ApiKeyFormat::~ApiKeyFormat()
	{

	}

	RET_E ApiKeyFormat::createJson()
	{
		_doc.SetObject();

		RET_E eRet = JSON_CREAT_MEMBER(_apiKey);
		if (eRet != VIX_SUCCESS) {
			return VIX_FAILURE;
		}

		return VIX_SUCCESS;
	}

	RET_E ApiKeyFormat::parseMemberValues()
	{
		RET_E eRet = VIX_SUCCESS;

		eRet = getMember(_apiKeyName, &_apiKey);
		if (eRet != VIX_SUCCESS) {
			return VIX_FAILURE;
		}

		return VIX_SUCCESS;
	}

	void ApiKeyFormat::setApiKey(const std::string& apiKey)
	{
		_apiKey = apiKey;

		JSON_CHANGE_MEMBER(_apiKey);
	}

	ApiKeyFormat& ApiKeyFormat::operator=(const ApiKeyFormat& fmt)
	{
		_apiKey = fmt._apiKey;

		createJson();

		return *this;
	}

	bool ApiKeyFormat::operator==(const ApiKeyFormat& fmt)
	{
		if (_apiKey == fmt._apiKey) {
			return true;
		}
		else {
			return false;
		}
	}
}