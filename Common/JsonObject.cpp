#include <Common/JsonObject.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>

#include <fstream>

namespace VixMetaBroker
{
	JsonObject::JsonObject()
	{
		_doc.SetObject();
		_docValue.SetObject();
	}

	JsonObject::~JsonObject()
	{
		_doc.RemoveAllMembers();
	}

	RET_E JsonObject::readFile(const std::string& fileName)
	{
		std::lock_guard<std::mutex> al(_mutex);

		if (fileName.empty()) {
			return VIX_FAILURE;
		}

		std::ifstream ifs(fileName);
		if (!ifs.is_open()) {
			return VIX_FAILURE;
		}

		rapidjson::IStreamWrapper isw(ifs);
		_doc.ParseStream(isw);

		if (_doc.HasParseError()) {
			_doc.SetObject();
			return VIX_FAILURE;
		}

		return RET_E::SUCCESS;
	}

	RET_E JsonObject::writeFile(const std::string& fileName, bool useEnter)
	{
		std::lock_guard<std::mutex> al(_mutex);

		if (fileName.empty()) {
			return VIX_FAILURE;
		}

		std::ofstream ofs(fileName);

		if (!ofs.is_open()) {
			return VIX_FAILURE;
		}

		rapidjson::OStreamWrapper osw(ofs);

		if (useEnter) {
			rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
			_doc.Accept(writer);
		}
		else {
			rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
			_doc.Accept(writer);
		}

		return RET_E::SUCCESS;
	}

	RET_E JsonObject::parseFromString(const std::string& jsonStr)
	{
		std::lock_guard<std::mutex> al(_mutex);

		if (jsonStr.empty()) {
			return VIX_FAILURE;
		}

		_doc.SetObject();

		_doc.Parse(jsonStr.c_str());
		if (_doc.HasParseError()) {
			_doc.SetObject();
			return VIX_FAILURE;
		}

		parseMemberValues();

		return RET_E::SUCCESS;
	}

#define VIX_CREATE_JSON_MEMBER(objectType, name)					\
	rapidjson::Value jsonName = convertStringToJsonValue(name);		\
	rapidjson::Value jsonValue(objectType);							\
	RET_E eRet = createMemberAndCheck(jsonName, jsonValue);		\
	return eRet;

#define VIX_CREATE_JSON_STRING_MEMBER(name, value)					\
	rapidjson::Value jsonName = convertStringToJsonValue(name);		\
	rapidjson::Value jsonValue = convertStringToJsonValue(value);	\
	RET_E eRet = createMemberAndCheck(jsonName, jsonValue);		\
	return eRet;

#define VIX_CREATE_JSON_TYPE_MEMBER(name, value, objectType, setfuncname)	\
	rapidjson::Value jsonName = convertStringToJsonValue(name);				\
	rapidjson::Value jsonValue(objectType);									\
	jsonValue.setfuncname(value);											\
	RET_E eRet = createMemberAndCheck(jsonName, jsonValue);				\
	return eRet;

	RET_E JsonObject::createObject(const std::string& jName) { VIX_CREATE_JSON_MEMBER(rapidjson::kObjectType, jName) }
	RET_E JsonObject::createArray(const std::string& jName) { VIX_CREATE_JSON_MEMBER(rapidjson::kArrayType, jName) }
	RET_E JsonObject::createMember(const std::string& jName, const std::string& value) { VIX_CREATE_JSON_STRING_MEMBER(jName, value) }
	RET_E JsonObject::createMember(const std::string& jName, int32 value) { VIX_CREATE_JSON_TYPE_MEMBER(jName, value, rapidjson::kNumberType, SetInt) }
	RET_E JsonObject::createMember(const std::string& jName, uint32 value) { VIX_CREATE_JSON_TYPE_MEMBER(jName, value, rapidjson::kNumberType, SetUint) }
	RET_E JsonObject::createMember(const std::string& jName, int64 value) { VIX_CREATE_JSON_TYPE_MEMBER(jName, value, rapidjson::kNumberType, SetInt64) }
	RET_E JsonObject::createMember(const std::string& jName, uint64 value) { VIX_CREATE_JSON_TYPE_MEMBER(jName, value, rapidjson::kNumberType, SetUint64) }
	RET_E JsonObject::createMember(const std::string& jName, float32 value) { VIX_CREATE_JSON_TYPE_MEMBER(jName, value, rapidjson::kNumberType, SetFloat) }
	RET_E JsonObject::createMember(const std::string& jName, float64 value) { VIX_CREATE_JSON_TYPE_MEMBER(jName, value, rapidjson::kNumberType, SetDouble) }
	RET_E JsonObject::createMember(const std::string& jName, bool value) { VIX_CREATE_JSON_TYPE_MEMBER(jName, value, rapidjson::kTrueType, SetBool) }

#define VIX_CHANGE_JSON_MEMBER(objectType, name, value)				\
	rapidjson::Value jsonName = convertStringToJsonValue(name);		\
	RET_E eRet = changeMemberAndCheck(jsonName, value);			\
	return eRet;

#define VIX_CHANGE_JSON_STRING_MEMBER(name, value)					\
	rapidjson::Value jsonName = convertStringToJsonValue(name);		\
	rapidjson::Value jsonValue = convertStringToJsonValue(value);	\
	RET_E eRet = changeMemberAndCheck(jsonName, jsonValue);		\
	return eRet;

#define VIX_CHANGE_JSON_TYPE_MEMBER(name, value, objectType, setfuncname)	\
	rapidjson::Value jsonName = convertStringToJsonValue(name);				\
	rapidjson::Value jsonValue(objectType);									\
	jsonValue.setfuncname(value);											\
	RET_E eRet = changeMemberAndCheck(jsonName, jsonValue);				\
	return eRet;

	RET_E JsonObject::changeObject(const std::string& jName, rapidjson::Value& value) { VIX_CHANGE_JSON_MEMBER(rapidjson::kObjectType, jName, value) }
	RET_E JsonObject::changeArray(const std::string& jName, rapidjson::Value& value) { VIX_CHANGE_JSON_MEMBER(rapidjson::kArrayType, jName, value) }
	RET_E JsonObject::changeMember(const std::string& jName, const std::string& value) { VIX_CHANGE_JSON_STRING_MEMBER(jName, value) }
	RET_E JsonObject::changeMember(const std::string& jName, int32 value) { VIX_CHANGE_JSON_TYPE_MEMBER(jName, value, rapidjson::kNumberType, SetInt) }
	RET_E JsonObject::changeMember(const std::string& jName, uint32 value) { VIX_CHANGE_JSON_TYPE_MEMBER(jName, value, rapidjson::kNumberType, SetUint) }
	RET_E JsonObject::changeMember(const std::string& jName, int64 value) { VIX_CHANGE_JSON_TYPE_MEMBER(jName, value, rapidjson::kNumberType, SetInt64) }
	RET_E JsonObject::changeMember(const std::string& jName, uint64 value) { VIX_CHANGE_JSON_TYPE_MEMBER(jName, value, rapidjson::kNumberType, SetUint64) }
	RET_E JsonObject::changeMember(const std::string& jName, float32 value) { VIX_CHANGE_JSON_TYPE_MEMBER(jName, value, rapidjson::kNumberType, SetFloat) }
	RET_E JsonObject::changeMember(const std::string& jName, float64 value) { VIX_CHANGE_JSON_TYPE_MEMBER(jName, value, rapidjson::kNumberType, SetDouble) }
	RET_E JsonObject::changeMember(const std::string& jName, bool value) { VIX_CHANGE_JSON_TYPE_MEMBER(jName, value, rapidjson::kTrueType, SetBool) }

#define VIX_PUSH_JSON_MEMBER(name, value)									\
	rapidjson::Value jsonName = convertStringToJsonValue(name);				\
	if (!value.IsObject()) {												\
		return VIX_FAILURE;													\
	}																		\
	rapidjson::Value object(value, _doc.GetAllocator());					\
	if (_doc.HasMember(name)) {												\
		if (_doc[jName].IsArray()) {										\
			_doc[jName].PushBack(object, _doc.GetAllocator());				\
			return RET_E::SUCCESS;											\
		}																	\
		else {																\
			return VIX_FAILURE;												\
		}																	\
	}																		\
	else {																	\
		return VIX_FAILURE;													\
	}

#define VIX_PUSH_JSON_STRING_MEMBER(name, value)								\
	rapidjson::Value jsonName = convertStringToJsonValue(name);					\
	rapidjson::Value jsonValue = convertStringToJsonValue(value);				\
	if (_doc.HasMember(name)) {													\
		if (_doc[jName].IsArray()) {											\
			_doc[jName].PushBack(jsonValue, _doc.GetAllocator());				\
			return RET_E::SUCCESS;												\
		}																		\
		else {																	\
			return VIX_FAILURE;													\
		}																		\
	}																			\
	else {																		\
		return VIX_FAILURE;														\
	}

#define VIX_PUSH_JSON_TYPE_MEMBER(name, value, objectType, setfuncname)			\
	rapidjson::Value jsonName = convertStringToJsonValue(name);					\
	rapidjson::Value jsonValue(objectType);										\
	jsonValue.setfuncname(value);												\
	if (_doc.HasMember(name)) {													\
		if (_doc[jName].IsArray()) {											\
			_doc[jName].PushBack(jsonValue, _doc.GetAllocator());				\
			return RET_E::SUCCESS;												\
		}																		\
		else {																	\
			return VIX_FAILURE;													\
		}																		\
	}																			\
	else {																		\
		return VIX_FAILURE;														\
	}


	RET_E JsonObject::pushObjectArrayMember(const std::string& jName, rapidjson::Value& value) { VIX_PUSH_JSON_MEMBER(jName, value) }
	RET_E JsonObject::pushArrayMember(const std::string& jName, const std::string& value) { VIX_PUSH_JSON_STRING_MEMBER(jName, value) }
	RET_E JsonObject::pushArrayMember(const std::string& jName, int32 value) { VIX_PUSH_JSON_TYPE_MEMBER(jName, value, rapidjson::kNumberType, SetInt) }
	RET_E JsonObject::pushArrayMember(const std::string& jName, uint32 value) { VIX_PUSH_JSON_TYPE_MEMBER(jName, value, rapidjson::kNumberType, SetUint) }
	RET_E JsonObject::pushArrayMember(const std::string& jName, int64 value) { VIX_PUSH_JSON_TYPE_MEMBER(jName, value, rapidjson::kNumberType, SetInt64) }
	RET_E JsonObject::pushArrayMember(const std::string& jName, uint64 value) { VIX_PUSH_JSON_TYPE_MEMBER(jName, value, rapidjson::kNumberType, SetUint64) }
	RET_E JsonObject::pushArrayMember(const std::string& jName, float32 value) { VIX_PUSH_JSON_TYPE_MEMBER(jName, value, rapidjson::kNumberType, SetFloat) }
	RET_E JsonObject::pushArrayMember(const std::string& jName, float64 value) { VIX_PUSH_JSON_TYPE_MEMBER(jName, value, rapidjson::kNumberType, SetDouble) }
	RET_E JsonObject::pushArrayMember(const std::string& jName, bool value) { VIX_PUSH_JSON_TYPE_MEMBER(jName, value, rapidjson::kTrueType, SetBool) }

#define VIX_CHANGE_ARRAY_MEMBER(name, idx, value)					\
	rapidjson::Value jsonName = convertStringToJsonValue(name);		\
	if (!value.IsObject()) {										\
		return VIX_FAILURE;					\
	}																\
	rapidjson::Value object(value, _doc.GetAllocator());			\
	if (_doc.HasMember(jName)) {						\
		if (_doc[jName].IsArray()) {					\
			_doc[jName][idx].Swap(object);				\
			return RET_E::SUCCESS;								\
		}															\
		else {														\
			return VIX_FAILURE;				\
		}															\
	}																\
	else {															\
		return VIX_FAILURE;				\
	}

#define VIX_CHANGE_ARRAY_STRING_MEMBER(name, idx, value)			\
	rapidjson::Value jsonName = convertStringToJsonValue(name);		\
	rapidjson::Value jsonValue = convertStringToJsonValue(value);	\
	if (_doc.HasMember(name)) {							\
		if (_doc[jName].IsArray()) {					\
			_doc[jName][idx].Swap(jsonValue);			\
			return RET_E::SUCCESS;								\
		}															\
		else {														\
			return VIX_FAILURE;				\
		}															\
	}																\
	else {															\
		return VIX_FAILURE;				\
	}

#define VIX_CHANGE_ARRAY_TYPE_MEMBER(name, idx, value, objectType, setfuncname)	\
	rapidjson::Value jsonName = convertStringToJsonValue(name);					\
	rapidjson::Value jsonValue(objectType);										\
	jsonValue.setfuncname(value);												\
	if (_doc.HasMember(name)) {										\
		if (_doc[jName].IsArray()) {								\
			_doc[jName][idx].Swap(jsonValue);						\
			return RET_E::SUCCESS;											\
		}																		\
		else {																	\
			return VIX_FAILURE;							\
		}																		\
	}																			\
	else {																		\
		return VIX_FAILURE;							\
	}


	RET_E JsonObject::changeObjectArrayMember(const std::string& jName, int32 idx, rapidjson::Value& value) { VIX_CHANGE_ARRAY_MEMBER(jName, idx, value) }
	RET_E JsonObject::changeArrayMember(const std::string& jName, int32 idx, const std::string& value) { VIX_CHANGE_ARRAY_STRING_MEMBER(jName, idx, value) }
	RET_E JsonObject::changeArrayMember(const std::string& jName, int32 idx, int32 value) { VIX_CHANGE_ARRAY_TYPE_MEMBER(jName, idx, value, rapidjson::kNumberType, SetInt) }
	RET_E JsonObject::changeArrayMember(const std::string& jName, int32 idx, uint32 value) { VIX_CHANGE_ARRAY_TYPE_MEMBER(jName, idx, value, rapidjson::kNumberType, SetUint) }
	RET_E JsonObject::changeArrayMember(const std::string& jName, int32 idx, int64 value) { VIX_CHANGE_ARRAY_TYPE_MEMBER(jName, idx, value, rapidjson::kNumberType, SetInt64) }
	RET_E JsonObject::changeArrayMember(const std::string& jName, int32 idx, uint64 value) { VIX_CHANGE_ARRAY_TYPE_MEMBER(jName, idx, value, rapidjson::kNumberType, SetUint64) }
	RET_E JsonObject::changeArrayMember(const std::string& jName, int32 idx, float32 value) { VIX_CHANGE_ARRAY_TYPE_MEMBER(jName, idx, value, rapidjson::kNumberType, SetFloat) }
	RET_E JsonObject::changeArrayMember(const std::string& jName, int32 idx, float64 value) { VIX_CHANGE_ARRAY_TYPE_MEMBER(jName, idx, value, rapidjson::kNumberType, SetDouble) }
	RET_E JsonObject::changeArrayMember(const std::string& jName, int32 idx, bool value) { VIX_CHANGE_ARRAY_TYPE_MEMBER(jName, idx, value, rapidjson::kTrueType, SetBool) }

	std::string JsonObject::getObjectMember(const std::string& name)
	{
		rapidjson::Value value;
		value.SetObject();

		if (_doc.HasMember(name)) {
			if (_doc[name].IsObject()) {
				std::string objectValue(convertObjectToString(_doc[name]));
				return objectValue;
			}
		}

		return "";
	}

#define VIX_GET_STRING_MEMBER(name, value)\
		if (_doc.HasMember(name)) {		\
			if (_doc[name].IsString()) {	\
				*value = std::string(_doc[name].GetString());	\
				return RET_E::SUCCESS;				\
			}			\
			else {	\
				return VIX_FAILURE;	\
			}	\
		}	\
		return VIX_FAILURE;

#define VIX_GET_TYPE_MEMBER(name, value, typeFunc, getFunc)	\
		if (_doc.HasMember(name)) {		\
			if (_doc[name].typeFunc()) {	\
				*value = _doc[name].getFunc();	\
				return RET_E::SUCCESS;				\
			}			\
			else {	\
				return VIX_FAILURE;	\
			}	\
		}	\
		return VIX_FAILURE;

	RET_E JsonObject::getMember(const std::string& name, std::string* pValue) { VIX_GET_STRING_MEMBER(name, pValue) }
	RET_E JsonObject::getMember(const std::string& name, int32* pValue) { VIX_GET_TYPE_MEMBER(name, pValue, IsInt, GetInt) }
	RET_E JsonObject::getMember(const std::string& name, uint32* pValue) { VIX_GET_TYPE_MEMBER(name, pValue, IsUint, GetUint) }
	RET_E JsonObject::getMember(const std::string& name, int64* pValue) { VIX_GET_TYPE_MEMBER(name, pValue, IsInt64, GetInt64) }
	RET_E JsonObject::getMember(const std::string& name, uint64* pValue) { VIX_GET_TYPE_MEMBER(name, pValue, IsUint, GetUint64) }
	RET_E JsonObject::getMember(const std::string& name, float32* pValue) { VIX_GET_TYPE_MEMBER(name, pValue, IsFloat, GetFloat) }
	RET_E JsonObject::getMember(const std::string& name, float64* pValue) { VIX_GET_TYPE_MEMBER(name, pValue, IsDouble, GetDouble) }
	RET_E JsonObject::getMember(const std::string& name, bool* pValue) { VIX_GET_TYPE_MEMBER(name, pValue, IsBool, GetBool) }

	int32 JsonObject::getArrayMemberSize(const std::string& name)
	{
		if (_doc.HasMember(name)) {
			if (_doc[name].IsArray()) {
				return static_cast<int32>(_doc[name].GetArray().Size());
			}
		}

		return -1;
	}

#define VIX_GET_STRING_ARRAY_MEMBER(name, value)								\
	if (idx > getArrayMemberSize(name)) {										\
		value = nullptr;														\
		return VIX_FAILURE;						\
	}																			\
	if (_doc.HasMember(name)) {										\
		if (_doc[name].IsArray()) {									\
			if (_doc[name][idx].IsString()) {						\
				*pValue = std::string(_doc[name][idx].GetString());	\
				return RET_E::SUCCESS;										\
			}																	\
			else {																\
				value = nullptr;												\
				return VIX_FAILURE;						\
			}																	\
		}																		\
		else {																	\
			value = nullptr;													\
			return VIX_FAILURE;							\
		}																		\
	}																			\
	else {																		\
		value = nullptr;														\
		return VIX_FAILURE;							\
	}

#define VIX_GET_TYPE_ARRAY_MEMBER(name, value, typeFunc, getFunc)				\
	if (idx > getArrayMemberSize(name)) {										\
		value = nullptr;														\
		return VIX_FAILURE;						\
	}																			\
	if (_doc.HasMember(name)) {										\
		if (_doc[name].IsArray()) {									\
			if (_doc[name][idx].typeFunc()) {						\
				*pValue = _doc[name][idx].getFunc();				\
				return RET_E::SUCCESS;										\
			}																	\
			else {																\
				value = nullptr;												\
				return VIX_FAILURE;						\
			}																	\
		}																		\
		else {																	\
			value = nullptr;													\
			return VIX_FAILURE;							\
		}																		\
	}																			\
	else {																		\
		value = nullptr;														\
		return VIX_FAILURE;							\
	}

	std::string JsonObject::getObjectArrayMember(const std::string& name, int32 idx)
	{
		if (idx > getArrayMemberSize(name)) {
			return "";
		}

		if (_doc.HasMember(name)) {
			if (_doc[name].IsArray()) {
				if (_doc[name][idx].IsObject()) {
					std::string objectStr(convertObjectToString(_doc[name][idx]));

					return objectStr;
				}
			}
		}

		return "";
	}

	RET_E JsonObject::getArrayMember(const std::string& name, int32 idx, std::string* pValue) { VIX_GET_STRING_ARRAY_MEMBER(name, pValue) }
	RET_E JsonObject::getArrayMember(const std::string& name, int32 idx, int32* pValue) { VIX_GET_TYPE_ARRAY_MEMBER(name, pValue, IsInt, GetInt) }
	RET_E JsonObject::getArrayMember(const std::string& name, int32 idx, uint32* pValue) { VIX_GET_TYPE_ARRAY_MEMBER(name, pValue, IsUint, GetUint) }
	RET_E JsonObject::getArrayMember(const std::string& name, int32 idx, int64* pValue) { VIX_GET_TYPE_ARRAY_MEMBER(name, pValue, IsInt64, GetInt64) }
	RET_E JsonObject::getArrayMember(const std::string& name, int32 idx, uint64* pValue) { VIX_GET_TYPE_ARRAY_MEMBER(name, pValue, IsUint64, GetUint64) }
	RET_E JsonObject::getArrayMember(const std::string& name, int32 idx, float32* pValue) { VIX_GET_TYPE_ARRAY_MEMBER(name, pValue, IsFloat, GetFloat) }
	RET_E JsonObject::getArrayMember(const std::string& name, int32 idx, float64* pValue) { VIX_GET_TYPE_ARRAY_MEMBER(name, pValue, IsDouble, GetDouble) }
	RET_E JsonObject::getArrayMember(const std::string& name, int32 idx, bool* pValue) { VIX_GET_TYPE_ARRAY_MEMBER(name, pValue, IsBool, GetBool) }

	rapidjson::Value JsonObject::convertStringToJsonValue(const std::string& value)
	{
		rapidjson::Value strValue(rapidjson::kStringType);
		strValue.SetString(value.c_str(), _doc.GetAllocator());

		return strValue;
	}

	void JsonObject::convertDocToJsonValue()
	{
		_docValue.SetObject();
		rapidjson::Value::ConstMemberIterator itr;
		for (itr = _doc.MemberBegin(); itr != _doc.MemberEnd(); itr++) {
			std::string nameStr(itr->name.GetString());

			rapidjson::Value jName = convertStringToJsonValue(nameStr);
			rapidjson::Value jValue(itr->value, _doc.GetAllocator());
			_docValue.AddMember(jName, jValue, _doc.GetAllocator());
		}
	}

	rapidjson::Value& JsonObject::getConvertedValueFromDoc()
	{
		convertDocToJsonValue();

		return _docValue;
	}

	std::string JsonObject::convertObjectToString(rapidjson::Value& value)
	{
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		value.Accept(writer);

		return std::string(buffer.GetString());
	}

	RET_E JsonObject::createMemberAndCheck(rapidjson::Value& name, rapidjson::Value& value)
	{
		std::string itemName(name.GetString());
		rapidjson::Type eType = value.GetType();

		if (!_doc.HasMember(itemName)) {
			_doc.AddMember(name, value, _doc.GetAllocator());
			if (_doc[itemName].GetType() == eType) {
				return RET_E::SUCCESS;
			}
		}

		return RET_E::FAILURE;
	}

	RET_E JsonObject::changeMemberAndCheck(rapidjson::Value& name, rapidjson::Value& value)
	{
		if (_doc.HasMember(name)) {
			if (_doc[name].GetType() == value.GetType()) {
				_doc[name].Swap(value);

				return RET_E::SUCCESS;
			}
		}

		return RET_E::FAILURE;
	}

	std::string JsonObject::toString(bool useEnter) const
	{
		if (!_doc.IsObject()) {
			return "";
		}

		rapidjson::StringBuffer buffer;

		if (useEnter) {
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
			_doc.Accept(writer);
		}
		else {
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			_doc.Accept(writer);
		}

		return std::string(buffer.GetString());
	}

	std::string JsonObject::toStdString(bool useEnter) const
	{
		std::string jsonStr(toString(useEnter));

		return jsonStr;
	}
}