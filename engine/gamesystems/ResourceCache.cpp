#include "ResourceCache.h"
// #include "DL_Debug/DL_Debug.h"
#include "Core/Utilities/Utilities.h"

std::unique_ptr<ResourceCache> ResourceCache::m_Instance = nullptr;
void ResourceCache::Create()
{
    m_Instance = std::make_unique<ResourceCache>();
}

ResourceCache& ResourceCache::Get()
{
    return *m_Instance;
}

void ResourceCache::Cache( IResource* pResource, const char* filepath )
{
    const uint64 key = Core::Hash( filepath );

    if( InsertIf( m_Textures, pResource, IResource::TEXTURE, key ) )
        return;

    if( InsertIf( m_Audios, pResource, IResource::AUDIO, key ) )
        return;

    if( InsertIf( m_Animations, pResource, IResource::ANIMATION, key ) )
        return;

    if( InsertIf( m_Shaders, pResource, IResource::SHADER, key ) )
    {
        //m_Watchers[0]->WatchFileChangeWithDependencies(filepath, std::bind(nullptr,)
        return;
    }
}

IResource* ResourceCache::IsCached( const char* filepath )
{
    return IsCached( Core::Hash( filepath ) );
}

IResource* ResourceCache::IsCached( const uint64 key )
{
    if( IResource* resource = IsCached( m_Textures, key ) )
        return resource;

    if( IResource* resource = IsCached( m_Audios, key ) )
        return resource;

    if( IResource* resource = IsCached( m_Animations, key ) )
        return resource;

    if( IResource* resource = IsCached( m_Shaders, key ) )
        return resource;

    return nullptr;
}

IResource* ResourceCache::IsCached( const std::map<uint64, IResource*>& ref_map, const uint64 key )
{
    const auto& it = ref_map.find( key );
    if( it != ref_map.end() )
    {
        return it->second;
    }

    return nullptr;
}

void ResourceCache::Update()
{
    // for (Core::FileWatcher* watcher : m_Watchers)
    // {
    // 	watcher->FlushChanges();
    // }
}

bool ResourceCache::InsertIf( std::map<uint64, IResource*>& ref_map, IResource* const pResource, const IResource::eResourceType type, const uint64 key )
{
    if( pResource->GetResourceType() == type )
    {
        ref_map.insert( std::make_pair( key, pResource ) );
        return true;
    }
    return false;
}

ResourceCache::ResourceCache()
{
    // m_Watchers.Add(new Core::FileWatcher);
}

ResourceCache::~ResourceCache()
{
    // m_Watchers.DeleteAll();
}
