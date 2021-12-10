#pragma once

#include "SpiritEngine/Renderer/OrthographicCamera.h"

#include "SpiritEngine/Renderer/Texture.h"

#include "SpiritEngine/Renderer/Camera.h"
#include "SpiritEngine/Renderer/EditorCamera.h"

#include "SpiritEngine/Scene/Components.h"

namespace SpiritEngine {

	class Renderer3D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const OrthographicCamera& camera); // TODO: Remove
		static void EndScene();
		static void Flush();

		// Primitives
		static void DrawCube(const glm::vec2& position, const glm::vec3& size, const glm::vec4& color);
		static void DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color);
		static void DrawCube(const glm::vec2& position, const glm::vec3& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), const glm::vec2& textureUVOffset = glm::vec2(0.0f));
		static void DrawCube(const glm::vec3& position, const glm::vec3& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), const glm::vec2& textureUVOffset = glm::vec2(0.0f));

		static void DrawCube(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawCube(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), const glm::vec2& textureUVOffset = glm::vec2(0.0f), int entityID = -1);

		static void DrawRotatedCube(const glm::vec2& position, const glm::vec3& size, float rotation, const glm::vec4& color);
		static void DrawRotatedCube(const glm::vec3& position, const glm::vec3& size, float rotation, const glm::vec4& color);
		static void DrawRotatedCube(const glm::vec2& position, const glm::vec3& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), const glm::vec2& textureUVOffset = glm::vec2(0.0f));
		static void DrawRotatedCube(const glm::vec3& position, const glm::vec3& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), const glm::vec2& textureUVOffset = glm::vec2(0.0f));

		static void DrawTexturedCube(const glm::mat4& transform, MeshRendererComponent& src, int entityID);

		// Stats
		struct Renderer3DStatistics
		{
			uint32_t DrawCalls = 0;
			uint32_t QuadCount = 0;

			uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
		};
		static void ResetStats();
		static Renderer3DStatistics GetStats();

	private:
		static void StartBatch();
		static void NextBatch();
	};

}
