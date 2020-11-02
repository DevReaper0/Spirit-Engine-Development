#include "spiritpch.h"
#include "Entity.h"

namespace SpiritEngine {

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{
	}

}