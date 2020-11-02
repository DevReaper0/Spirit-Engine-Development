#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "SceneCamera.h"
#include "ScriptableEntity.h"

namespace SpiritEngine {

	struct TagComponent
	{
		std::string Name;
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& name, const std::string& tag="")
			: Name(name), Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 OriginalTranslation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Translation = OriginalTranslation;
		glm::vec3 OriginalRotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = OriginalRotation;
		glm::vec3 OriginalScale = { 1.0f, 1.0f, 1.0f };
		glm::vec3 Scale = OriginalScale;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Rotation.x, { 1, 0, 0 })
				* glm::rotate(glm::mat4(1.0f), Rotation.y, { 0, 1, 0 })
				* glm::rotate(glm::mat4(1.0f), Rotation.z, { 0, 0, 1 });

			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 OriginalColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec4 Color = OriginalColor;

		std::string OriginalShape = "Square";
		std::string Shape = OriginalShape;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color, const std::string shape = "Square")
			: Color(color), Shape(shape) {}
	};

	struct LineRendererComponent
	{
		glm::vec4 OriginalColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		glm::vec4 Color = OriginalColor;
		float OriginalWidth = 1.0f;
		float Width = OriginalWidth;

		LineRendererComponent() = default;
		LineRendererComponent(const LineRendererComponent&) = default;
		LineRendererComponent(const glm::vec4& color, float width = 1.0f)
			: Color(color), Width(width)
		{
		}
	};

	struct CameraComponent
	{
		SceneCamera OriginalCamera;
		SceneCamera Camera = OriginalCamera;
		bool OriginalPrimary = true; // TODO: think about moving to Scene
		bool Primary = OriginalPrimary;
		bool OriginalFixedAspectRatio = false;
		bool FixedAspectRatio = OriginalFixedAspectRatio;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

}
