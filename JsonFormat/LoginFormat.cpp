#include <JsonFormat/LoginFormat.h>

namespace VixMetaBroker
{
	BasicLoginFormat::BasicLoginFormat()
		: _idName("id"), _pwName("pw")
		, _id(""), _pw("")
	{
		createJson();
	}

	BasicLoginFormat::~BasicLoginFormat()
	{

	}

	RET_E BasicLoginFormat::createJson()
	{
		_doc.SetObject();

		JSON_CREAT_MEMBER(_id);
		JSON_CREAT_MEMBER(_pw);

		return VIX_SUCCESS;
	}

	RET_E BasicLoginFormat::parseMemberValues()
	{
		JSON_GET_MEMBER(_id);
		JSON_GET_MEMBER(_pw);

		return VIX_SUCCESS;
	}

	void BasicLoginFormat::setId(const std::string& id)
	{
		_id = id;

		JSON_CHANGE_MEMBER(_id);
	}

	void BasicLoginFormat::setPw(const std::string& pw)
	{
		_pw = pw;

		JSON_CHANGE_MEMBER(_pw);
	}

	BasicLoginFormat& BasicLoginFormat::operator=(const BasicLoginFormat& fmt) noexcept
	{
		_id = fmt.id();
		_pw = fmt.pw();

		createJson();

		return *this;
	}

	bool BasicLoginFormat::operator==(const BasicLoginFormat& fmt) noexcept
	{
		if (_id == fmt.id() &&
			_pw == fmt.pw())
		{
			return true;
		}
		else {
			return false;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	LoginFormat::LoginFormat()
		: _idName("id"), _pwName("pw"), _keepAliveTimeoutName("keepAliveTimeOut")
		, _id(""), _pw(""), _keepAliveTimeout(15 * 60 * 1000)
	{
		createJson();
	}

	LoginFormat::~LoginFormat()
	{

	}

	RET_E LoginFormat::createJson()
	{
		_doc.SetObject();

		JSON_CREAT_MEMBER(_id);
		JSON_CREAT_MEMBER(_pw);
		JSON_CREAT_MEMBER(_keepAliveTimeout);

		return VIX_SUCCESS;
	}

	RET_E LoginFormat::parseMemberValues()
	{
		JSON_GET_MEMBER(_id);
		JSON_GET_MEMBER(_pw);
		JSON_GET_MEMBER(_keepAliveTimeout);

		return VIX_SUCCESS;
	}

	void LoginFormat::setId(const std::string& id)
	{
		_id = id;

		JSON_CHANGE_MEMBER(_id);
	}

	void LoginFormat::setPw(const std::string& pw)
	{
		_pw = pw;

		JSON_CHANGE_MEMBER(_pw);
	}

	void LoginFormat::setKeepAliveTimeout(const uint32 timeout)
	{
		_keepAliveTimeout = timeout;

		JSON_CHANGE_MEMBER(_keepAliveTimeout);
	}

	LoginFormat& LoginFormat::operator=(const LoginFormat& fmt) noexcept
	{
		_id = fmt._id;
		_pw = fmt._pw;
		_keepAliveTimeout = fmt._keepAliveTimeout;

		createJson();

		return *this;
	}

	bool LoginFormat::operator==(const LoginFormat& fmt) noexcept
	{
		if (_id == fmt._id &&
			_pw == fmt._pw &&
			_keepAliveTimeout == fmt._keepAliveTimeout)
		{
			return true;
		}
		else {
			return false;
		}
	}
}