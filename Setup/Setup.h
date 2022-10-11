#ifndef _VIX_META_BROKER_SETUP_H_
#define _VIX_META_BROKER_SETUP_H_

#include <Setup/VixServerInfo.h>
#include <Setup/PluxityServerInfo.h>

#include <vector>

namespace VixMetaBroker
{
	class Setup : public JsonObject
	{
	public :
		Setup();
		virtual ~Setup();

	private :
		std::string _fileName;

	private :
		JSON_VALUE_DECL(std::string, _version);
		JSON_VALUE_DECL(std::string, _revision);
		JSON_VALUE_DECL(std::string, _setupVersion);
		JSON_VALUE_DECL(std::string, _setupRevision);
		JSON_VALUE_DECL(int32, _vixServerCount);

		std::string _vixServerInfoName;
		std::vector<VixServerInfo> _vixServerInfos;

		std::string _pluxityServerInfoName;
		PluxityServerInfo _pluxityServerInfo;

	public :
		const std::string& version() const noexcept { return _version; }
		const std::string& revision() const noexcept { return _revision; }
		int32 vixServerCount() const noexcept { return _vixServerCount; }

	public :
		void addVixServerInfo(const VixServerInfo& info);
		VixServerInfo* getServerInfoByIdx(int32 idx);

		PluxityServerInfo& pluxityServerInfo() { return _pluxityServerInfo; }

	public :
		RET_E load();
		RET_E write();

	public :
		RET_E createJson() override;
		
	protected :
		RET_E parseMemberValues() override;

	public :
		Setup& operator=(const Setup& setup);
		bool operator==(const Setup& setup) noexcept;
		bool operator!=(const Setup& setup) noexcept { return *this == setup ? false : true; }
	};
}

#endif
