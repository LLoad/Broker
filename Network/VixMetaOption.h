#ifndef _VIX_META_BROKER_VIX_META_OPTION_H_
#define _VIX_META_BROKER_VIX_META_OPTION_H_

#include <Common/Type.h>


namespace VixMetaBroker
{
#if 0
#define ENUM_OPERATOR_OR_DECL(type)									\
	constexpr type operator|(const type left, const type right);	\
	constexpr type operator~(const type value);						\
	constexpr type operator&(const type left, const type right);
#endif



#define MAX_VIX_CH_CNT	255

	enum class VIX_META_TYPE_E : uint32
	{
		NONE			= 0,
		FRAME			= 0x00000001,
		EVENT			= 0x00000002,
		EVENT_ALARM		= 0x00000004,
		FACE			= 0x00000008,
		PLATE			= 0x00000010,
		EVENT_ZONE		= 0x00000020,
		EVENT_ZONE_CNT	= 0x00000040,
		VEHICHLE_QUE	= 0x00000080,
		CROSSWALK_STAT  = 0x00000100,
		CAMERA_STATE	= 0x00000200,
		ALL				= 0xFFFFFFFF,
	};

	ENUM_CLASS_OPERATOR_DEFINE(VIX_META_TYPE_E);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class VIX_FRM_META_OPT_E : uint32
	{
		NONE = 0,
		COLOR,
		ALL = 0xFFFFFFFF,
	};

	ENUM_CLASS_OPERATOR_DEFINE(VIX_FRM_META_OPT_E);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class VIX_EVT_META_OPT_E : uint32
	{
		NONE  = 0,
		BEGUN = 0x00000001,
		ENDED = 0x00000002,
		IMAGE = 0x00000004,
		COLOR = 0x00000008,

		ALL   = 0xFFFFFFFF,
	};

	ENUM_CLASS_OPERATOR_DEFINE(VIX_EVT_META_OPT_E);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class VIX_EVT_META_SUB_OPT_E : uint32
	{
		NONE = 0,
		GID,

		ALL = 0xFFFFFFFF,
	};

	ENUM_CLASS_OPERATOR_DEFINE(VIX_EVT_META_SUB_OPT_E);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class VIX_EVT_ALM_META_OPT_E : uint32
	{
		NONE		= 0,
		IMAGE		= 0x00000001,
		FULL_IMAGE	= 0x00000002,
		OBJ_INFO	= 0x00000004,
		ALL			= 0xFFFFFFFF,
	};

	ENUM_CLASS_OPERATOR_DEFINE(VIX_EVT_ALM_META_OPT_E);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class VIX_FACE_META_OPT_E : uint32
	{
		NONE				= 0,
		BEGUN				= 0x00000001,
		ENDED				= 0x00000002,
		ATTR_UPDATED		= 0x00000004,
		RECOG_REGISTERED	= 0x00000008,
		RECOG_UNREGISTERED  = 0x00000010,
		IMAGE				= 0x00000020,
		ALL					= 0xFFFFFFFF,
	};

	ENUM_CLASS_OPERATOR_DEFINE(VIX_FACE_META_OPT_E);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class VIX_PLATE_META_OPT_E : uint32
	{
		NONE = 0,
		IMAGE = 0x00000001,
		FULL_TN_IMAGE = 0x00000002,
		FULL_IMAGE = 0x00000004,
		PACKAGE = 0x00000008,
		ALL = 0xFFFFFFFF,
	};

	ENUM_CLASS_OPERATOR_DEFINE(VIX_PLATE_META_OPT_E);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class MetaOption
	{
	public :
		MetaOption();
		virtual ~MetaOption();

	private :
		VIX_META_TYPE_E _eMetaType;
		
		VIX_FRM_META_OPT_E _eFrmMetaOpt;
		VIX_EVT_META_OPT_E _eEvtMetaOpt;
		VIX_EVT_META_SUB_OPT_E _eEvtMetaSubOpt;
		VIX_EVT_ALM_META_OPT_E _eEvtAlmMetaOpt;
		VIX_FACE_META_OPT_E _eFaceMetaOpt;
		VIX_PLATE_META_OPT_E _ePlateMetaOpt;

		bool _selectedCh[MAX_VIX_CH_CNT + 1] = { 0, };

	public :
		VIX_META_TYPE_E metaType() const noexcept { return _eMetaType; }
		VIX_FRM_META_OPT_E frmMetaOpt() const noexcept { return _eFrmMetaOpt; }
		VIX_EVT_META_OPT_E evtMetaOpt() const noexcept { return _eEvtMetaOpt; }
		VIX_EVT_META_SUB_OPT_E evtMetaSubOpt() const noexcept { return _eEvtMetaSubOpt; }
		VIX_FACE_META_OPT_E faceMetaOpt() const noexcept { return _eFaceMetaOpt; }
		VIX_PLATE_META_OPT_E plateMetaOpt() const noexcept { return _ePlateMetaOpt; }

		bool isChannelSelected(int32 ch) const noexcept;
		bool isAllChannelSelected() const noexcept;

	public :
		void setMetaType(const VIX_META_TYPE_E eType) noexcept { _eMetaType = eType; }
		void setFrmMetaOpt(const VIX_FRM_META_OPT_E eOpt) noexcept { _eFrmMetaOpt = eOpt; }
		void setEvtMetaOpt(const VIX_EVT_META_OPT_E eOpt) noexcept { _eEvtMetaOpt = eOpt; }
		void setEvtMetaSubOpt(const VIX_EVT_META_SUB_OPT_E eOpt) noexcept { _eEvtMetaSubOpt = eOpt; }
		void setEvtAlmMetaOpt(const VIX_EVT_ALM_META_OPT_E eOpt) noexcept { _eEvtAlmMetaOpt = eOpt; }
		void setPlateMetaOpt(const VIX_PLATE_META_OPT_E eOpt) noexcept { _ePlateMetaOpt = eOpt; }

		void addSelectedCh(int32 ch) noexcept;
		void removeSelectedCh(int32 ch) noexcept;
		void activateAllCh() noexcept { _selectedCh[MAX_VIX_CH_CNT] = true; }
		void deactivateAllCh() noexcept { _selectedCh[MAX_VIX_CH_CNT] = false; }

	public :
		std::string getMetaTypeString(const VIX_META_TYPE_E eMetaType);

		std::string getOptStr(const VIX_FRM_META_OPT_E eOpt);
		std::string getOptStr(const VIX_EVT_META_OPT_E eOpt);
		std::string getOptStr(const VIX_EVT_META_SUB_OPT_E eOpt);
		std::string getOptStr(const VIX_EVT_ALM_META_OPT_E eOpt);
		std::string getOptStr(const VIX_FACE_META_OPT_E eOpt);
		std::string getOptStr(const VIX_PLATE_META_OPT_E eOpt);

	public :
		bool isMetaTypeEnable(const VIX_META_TYPE_E eMetaType) const noexcept { return static_cast<bool>(_eMetaType & eMetaType); }
		bool isOptionEnable(const VIX_FRM_META_OPT_E eOpt) const noexcept { return static_cast<bool>(_eFrmMetaOpt & eOpt); }
		bool isOptionEnable(const VIX_EVT_META_OPT_E eOpt) const noexcept { return static_cast<bool>(_eEvtMetaOpt & eOpt); }
		bool isOptionEnable(const VIX_EVT_META_SUB_OPT_E eOpt) const noexcept { return static_cast<bool>(_eEvtMetaSubOpt & eOpt); }
		bool isOptionEnable(const VIX_EVT_ALM_META_OPT_E eOpt) const noexcept { return static_cast<bool>(_eEvtAlmMetaOpt & eOpt); }
		bool isOptionEnable(const VIX_FACE_META_OPT_E eOpt) const noexcept { return static_cast<bool>(_eFaceMetaOpt & eOpt); }
		bool isOptionEnable(const VIX_PLATE_META_OPT_E eOpt) const noexcept { return static_cast<bool>(_ePlateMetaOpt & eOpt); }

	private :
		std::string makeFrameMetaString();
		std::string makeEventMetaString();
		std::string makeEventAlmMetaString();
		std::string makeFaceMetaString();
		std::string makePlateMetaString();
		std::string makeEventZoneMetaString();
		std::string makeEventZoneCountMetaString();
		std::string makeVehicleQueMetaString();
		std::string makeCameraStateMetaString();
		std::string makeSelectedChannelString();

	public :
		std::string toString();

	public :
		MetaOption& operator=(const MetaOption& opt);
		bool operator==(const MetaOption& opt);
		bool operator!=(const MetaOption& opt) { return *this == opt ? false : true; }

	};
}

#endif