#pragma once

#include "SpiritEngine/Core/Timestep.h"
#include "SpiritEngine/Core/UUID.h"
#include "SpiritEngine/Renderer/EditorCamera.h"

#include "entt.hpp"

class b2World;

namespace SpiritEngine {
	
	class Entity;

	class Scene
	{
	public:
		Scene(const std::string& name = "Untitled");
		~Scene();

		Entity CreateEntity(const std::string& name = std::string(), const std::string& tag = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string(), const std::string& tag = std::string());
		void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

		Entity DuplicateEntity(Entity entity);

		static Ref<Scene> Copy(Ref<Scene> scene);
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	public:
		std::string Name;
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		b2World* m_PhysicsWorld = nullptr;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};

}
