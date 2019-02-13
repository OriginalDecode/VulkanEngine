#pragma once

class IResource
{
public:
	enum eResourceType
	{
		NONE = -1,
		TEXTURE,
		ANIMATION,
		AUDIO,
		SHADER,
	};

	IResource(eResourceType resource_type) : m_ResourceType(resource_type) { }
	eResourceType GetResourceType() { return m_ResourceType; }
protected:
	eResourceType m_ResourceType{ NONE };
};