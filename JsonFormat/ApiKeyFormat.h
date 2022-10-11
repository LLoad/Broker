#ifndef _VIX_META_BROKER_API_KEY_FORMAT_H_
#define _VIX_META_BROKER_API_KEY_FORMAT_H_

#include <Common/JsonObject.h>

namespace VixMetaBroker
{
	class ApiKeyFormat : public JsonObject
	{
	public :
		ApiKeyFormat();
		virtual ~ApiKeyFormat();

	private :
		JSON_VALUE_DECL(std::string, _apiKey);

	public :
		const std::string& apiKey() const noexcept { return _apiKey; }
		void setApiKey(const std::string& apiKey);

		RET_E createJson() override;

	protected :
		RET_E parseMemberValues() override;

	public :
		ApiKeyFormat& operator=(const ApiKeyFormat& fmt);
		bool operator==(const ApiKeyFormat& fmt);
		bool operator!=(const ApiKeyFormat& fmt) noexcept { *this == fmt ? false : true; }
	};
}

#endif