#pragma once
#include "Core/IResource.h"
#include "Core/Types.h"

// #include "Core/Utilities/FileWatcher.h"
// #include "Core/containers/GrowingArray.h"

#include <map>

class ResourceCache
{
public:

	static void Create();
	static void Destroy();
	static ResourceCache* Get();

	void Cache(IResource* pResource, const char* filepath);
	IResource* IsCached(const char* filepath);
	IResource* IsCached(const uint64 hash);

	void Update();

private:
	ResourceCache();
	~ResourceCache();
	static ResourceCache* m_Instance;

	std::map<uint64, IResource*> m_Textures;
	std::map<uint64, IResource*> m_Audios;
	std::map<uint64, IResource*> m_Animations;
	std::map<uint64, IResource*> m_Shaders;

	// Core::GrowingArray<Core::FileWatcher*> m_Watchers;



	IResource* IsCached(const std::map<uint64, IResource*>& ref_map, const uint64 key);
	bool InsertIf(std::map<uint64, IResource*>& ref_map, IResource* const pResource, const IResource::eResourceType type, const uint64 key);

};

