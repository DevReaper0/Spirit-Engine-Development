#include "spiritpch.h"
#include "Scene.h"

#include "Components.h"
#include "SpiritEngine/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

#include "Entity.h"

namespace SpiritEngine {

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name, const std::string& tag)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tagComp = entity.AddComponent<TagComponent>();
		tagComp.Name = name.empty() ? "New Entity" : name;
		tagComp.Tag = tag.empty() ? "Entity" : tag;
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnUpdate(Timestep ts)
	{
		// Update scripts
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				// TODO: Move to Scene::OnScenePlay
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_Entity = Entity{ entity, this };
					nsc.Instance->OnCreate();
				}

				nsc.Instance->OnUpdate(ts);
			});
		}

		// Render 2D
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
				
				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		if (mainCamera)
		{
			//Ref<Texture2D> m_CheckerboardTexture = Texture2D::Create("../SpiritEditor/assets/textures/Checkerboard.png");

			Renderer2D::BeginScene(*mainCamera, cameraTransform);
			//Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerboardTexture, 10.0f);

			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				/*if (sprite.Shape == "Square")
				{
					Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
				}
				else if (sprite.Shape == "Triangle")
				{
					Renderer2D::DrawTriangle(transform.GetTransform(), sprite.Color);
				}
				else if (sprite.Shape == "Line")
				{
					Renderer2D::DrawLine(transform.GetTransform(), sprite.Color);
				}*/
				Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);

				//Renderer2D::DrawLine(transform.Translation, glm::vec3{ transform.Translation.x + 10, transform.Translation.y + 10, transform.Translation.z + 10 }, transform.Scale.x, sprite.Color);
			}

			/*auto group2 = m_Registry.group<TransformComponent>(entt::get<LineRendererComponent>);
			for (auto entity2 : group2)
			{
				auto [transform2, line] = group2.get<TransformComponent, LineRendererComponent>(entity2);

				Renderer2D::DrawLine(transform2.Translation, transform2.Translation, line.Width, line.Color);
			}*/

			Renderer2D::EndScene();
		}

	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize our non-FixedAspectRatio cameras
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}

	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
#ifndef __GNUC__
		static_assert(false);
#else
		assert(false);
#endif
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<LineRendererComponent>(Entity entity, LineRendererComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{
	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{
	}


}
