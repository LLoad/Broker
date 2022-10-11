#ifndef _VIX_META_BROKER_JSON_OBJECT_H_
#define _VIX_META_BROKER_JSON_OBJECT_H_

#define RAPIDJSON_HAS_STDSTRING		1

#include <Common/Type.h>

#include <rapidjson/document.h>

#include <mutex>

#define VALUE_NAME_DECL(valueName)		std::string valueName##Name;
#define JSON_VALUE_DECL(type, name)		std::string name##Name; type name;

#define JSON_CREAT_MEMBER(value)	createMember(value##Name, value)
#define JSON_CHANGE_MEMBER(value)	changeMember(value##Name, value)
#define JSON_GET_MEMBER(value)		getMember(value##Name, &value);
#define JSON_CHECK_AND_CREATE_MEMBER(value)			\
	eRet = JSON_GET_MEMBER(value);					\
	if (eRet != VIX_SUCCESS) {						\
		JSON_CREAT_MEMBER(value);					\
	}

namespace VixMetaBroker
{
	class JsonObject
	{
	public:
		JsonObject();
		virtual ~JsonObject();

	protected:
		rapidjson::Document _doc;
		rapidjson::Value _docValue;

		std::mutex _mutex;

	public:
		virtual RET_E createJson() = 0;
		virtual RET_E createJsonValue() { return VIX_SUCCESS; }

	protected:
		virtual RET_E parseMemberValues() { return RET_E::SUCCESS; }
		virtual RET_E parseMemberValue(const std::string& valName) { return RET_E::SUCCESS; }

	public:
		RET_E readFile(const std::string& fileName);
		RET_E writeFile(const std::string& fileName, bool useEnter = true);
		RET_E parseFromString(const std::string& jsonStr);

	public:
		RET_E createObject(const std::string& jName);
		RET_E createArray(const std::string& jName);
		RET_E createMember(const std::string& jName, const std::string& value);
		RET_E createMember(const std::string& jName, int32 value);
		RET_E createMember(const std::string& jName, uint32 value);
		RET_E createMember(const std::string& jName, int64 value);
		RET_E createMember(const std::string& jName, uint64 value);
		RET_E createMember(const std::string& jName, float32 value);
		RET_E createMember(const std::string& jName, float64 value);
		RET_E createMember(const std::string& jName, bool value);

	public:
		RET_E changeObject(const std::string& jName, rapidjson::Value& value);
		RET_E changeArray(const std::string& jName, rapidjson::Value& value);
		RET_E changeMember(const std::string& jName, const std::string& value);
		RET_E changeMember(const std::string& jName, int32 value);
		RET_E changeMember(const std::string& jName, uint32 value);
		RET_E changeMember(const std::string& jName, int64 value);
		RET_E changeMember(const std::string& jName, uint64 value);
		RET_E changeMember(const std::string& jName, float32 value);
		RET_E changeMember(const std::string& jName, float64 value);
		RET_E changeMember(const std::string& jName, bool value);

	public:
		RET_E pushObjectArrayMember(const std::string& name, rapidjson::Value& value);
		RET_E pushArrayMember(const std::string& jName, const std::string& value);
		RET_E pushArrayMember(const std::string& jName, int32 value);
		RET_E pushArrayMember(const std::string& jName, uint32 value);
		RET_E pushArrayMember(const std::string& jName, int64 value);
		RET_E pushArrayMember(const std::string& jName, uint64 value);
		RET_E pushArrayMember(const std::string& jName, float32 value);
		RET_E pushArrayMember(const std::string& jName, float64 value);
		RET_E pushArrayMember(const std::string& jName, bool value);

	public:
		RET_E changeObjectArrayMember(const std::string& name, int32 idx, rapidjson::Value& value);
		RET_E changeArrayMember(const std::string& jName, int32 idx, const std::string& value);
		RET_E changeArrayMember(const std::string& jName, int32 idx, int32 value);
		RET_E changeArrayMember(const std::string& jName, int32 idx, uint32 value);
		RET_E changeArrayMember(const std::string& jName, int32 idx, int64 value);
		RET_E changeArrayMember(const std::string& jName, int32 idx, uint64 value);
		RET_E changeArrayMember(const std::string& jName, int32 idx, float32 value);
		RET_E changeArrayMember(const std::string& jName, int32 idx, float64 value);
		RET_E changeArrayMember(const std::string& jName, int32 idx, bool value);

	public:
		std::string getObjectMember(const std::string& name);
		RET_E getMember(const std::string& name, std::string* pValue);
		RET_E getMember(const std::string& name, int32* pValue);
		RET_E getMember(const std::string& name, uint32* pValue);
		RET_E getMember(const std::string& name, int64* pValue);
		RET_E getMember(const std::string& name, uint64* pValue);
		RET_E getMember(const std::string& name, float32* pValue);
		RET_E getMember(const std::string& name, float64* pValue);
		RET_E getMember(const std::string& name, bool* pValue);

	public:
		int32 getArrayMemberSize(const std::string& name);

		std::string getObjectArrayMember(const std::string& name, int32 idx);
		RET_E getArrayMember(const std::string& name, int32 idx, std::string* pValue);
		RET_E getArrayMember(const std::string& name, int32 idx, int32* pValue);
		RET_E getArrayMember(const std::string& name, int32 idx, uint32* pValue);
		RET_E getArrayMember(const std::string& name, int32 idx, int64* pValue);
		RET_E getArrayMember(const std::string& name, int32 idx, uint64* pValue);
		RET_E getArrayMember(const std::string& name, int32 idx, float32* pValue);
		RET_E getArrayMember(const std::string& name, int32 idx, float64* pValue);
		RET_E getArrayMember(const std::string& name, int32 idx, bool* pValue);

	protected:
		rapidjson::Value convertStringToJsonValue(const std::string& value);
		void convertDocToJsonValue();
		rapidjson::Value& docValue() { return _docValue; }

	public :
		rapidjson::Value& getConvertedValueFromDoc();

	protected :
		std::string convertObjectToString(rapidjson::Value& value);

		RET_E createMemberAndCheck(rapidjson::Value& name, rapidjson::Value& value);
		RET_E changeMemberAndCheck(rapidjson::Value& name, rapidjson::Value& value);

	public:
		std::string toString(bool useEnter = false) const;
		std::string toStdString(bool useEnter = false) const;
	};
}

#endif