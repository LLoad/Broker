#ifndef _VIX_META_BROKER_TYPE_H_
#define _VIX_META_BROKER_TYPE_H_

#include <iostream>
#include <stdio.h>
#include <stdint.h>

#include <Common/ErrorType.h>

namespace VixMetaBroker
{
	typedef int8_t		int8;
	typedef uint8_t		uint8;
	typedef int16_t		int16;
	typedef uint16_t	uint16;
	typedef int32_t		int32;
	typedef uint32_t	uint32;
	typedef int64_t		int64;
	typedef uint64_t    uint64;
	typedef float_t		float32;
	typedef double_t	float64;


#define VIX_SUCCESS		RET_E::SUCCESS
#define VIX_FAILURE		RET_E::FAILURE

#define VIX_LOG_NAME	"VixMetaBroker"

	enum class LOG_LEVEL_E
	{
		OFF,
		FATAL,
		ERROR,
		WARN,
		INFO,
		DEBUG,
		TRACE,
		ALL,
	};

#define ENUM_TO_INT32(eValue)	static_cast<int32>(eValue)
#define ENUM_TO_UINT32(eValue)	static_cast<uint32>(eValue)

#define ENUM_CLASS_OPERATOR_DEFINE(type)											\
	constexpr type operator|(const type left, const type right)						\
	{																				\
		return static_cast<type>(ENUM_TO_UINT32(left) | ENUM_TO_UINT32(right));		\
	}																				\
	constexpr type operator~(const type value)										\
	{																				\
		return static_cast<type>(~ENUM_TO_UINT32(value));							\
	}																				\
	constexpr type operator&(const type left, const type right)						\
	{																				\
		return static_cast<type>(ENUM_TO_UINT32(left) & ENUM_TO_UINT32(right));		\
	}


}

#endif
