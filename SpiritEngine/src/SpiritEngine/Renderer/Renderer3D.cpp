#include "spiritpch.h"
#include "SpiritEngine/Renderer/Renderer3D.h"

#include "SpiritEngine/Renderer/VertexArray.h"
#include "SpiritEngine/Renderer/Shader.h"
#include "SpiritEngine/Renderer/UniformBuffer.h"
#include "SpiritEngine/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace SpiritEngine {

	struct CubeVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;

		// Editor-only
		int EntityID;
	};

	struct Renderer3DData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps

		Ref<Texture2D> WhiteTexture;

		Ref<VertexArray> CubeVertexArray;
		Ref<VertexBuffer> CubeVertexBuffer;
		Ref<Shader> CubeShader;

		uint32_t QuadIndexCount = 0;
		CubeVertex* CubeVertexBufferBase = nullptr;
		CubeVertex* CubeVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		glm::vec4 QuadVertexPositions[68];

		Renderer3D::Renderer3DStatistics Stats;

		struct CameraData
		{
			glm::mat4 ViewProjection;
		};
		CameraData CameraBuffer;
		Ref<UniformBuffer> CameraUniformBuffer;
	};

	static Renderer3DData s_Data;

	void Renderer3D::Init()
	{
		SPIRIT_PROFILE_FUNCTION();

		s_Data.CubeVertexArray = VertexArray::Create();

		s_Data.CubeVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CubeVertex));
		s_Data.CubeVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoord"     },
			{ ShaderDataType::Float,  "a_TexIndex"     },
			{ ShaderDataType::Float,  "a_TilingFactor" },
			{ ShaderDataType::Int,    "a_EntityID"     }
			});
		s_Data.CubeVertexArray->AddVertexBuffer(s_Data.CubeVertexBuffer);

		s_Data.CubeVertexBufferBase = new CubeVertex[s_Data.MaxVertices];

		uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
		s_Data.CubeVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		// TODO: Make this load a white cubemap texture with shading
		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		s_Data.CubeShader = Shader::Create("assets/shaders/Renderer3D_Cube.glsl");

		// Set first texture slot to 0
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		// TODO: Replace this after adding custom meshes from models
		s_Data.QuadVertexPositions[0]  = { -0.5f, -0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[1]  = {  0.5f, -0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[2]  = {  0.5f,  0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[3]  = { -0.5f,  0.5f, -0.5f, 1.0f };

		s_Data.QuadVertexPositions[4]  = { -0.5f, -0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[5]  = {  0.5f, -0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[6]  = {  0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[7]  = { -0.5f,  0.5f,  0.5f, 1.0f };

		s_Data.QuadVertexPositions[8]  = { -0.5f, -0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[9]  = { -0.5f, -0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[10] = { -0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[11] = { -0.5f,  0.5f, -0.5f, 1.0f };

		s_Data.QuadVertexPositions[12] = { -0.5f,  0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[13] = { -0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[14] = { -0.5f, -0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[15] = { -0.5f, -0.5f, -0.5f, 1.0f };

		s_Data.QuadVertexPositions[16] = {  0.5f, -0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[17] = {  0.5f, -0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[18] = {  0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[19] = {  0.5f,  0.5f, -0.5f, 1.0f };

		s_Data.QuadVertexPositions[20] = {  0.5f,  0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[21] = {  0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[22] = {  0.5f, -0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[23] = {  0.5f, -0.5f, -0.5f, 1.0f };


		s_Data.QuadVertexPositions[24] = {  0.5f, -0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[25] = {  0.5f, -0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[26] = {  0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[27] = {  0.5f,  0.5f, -0.5f, 1.0f };

		s_Data.QuadVertexPositions[28] = {  0.5f,  0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[29] = {  0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[30] = {  0.5f, -0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[31] = {  0.5f, -0.5f, -0.5f, 1.0f };


		s_Data.QuadVertexPositions[32] = { -0.5f, -0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[33] = {  0.5f, -0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[34] = {  0.5f, -0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[35] = { -0.5f, -0.5f, -0.5f, 1.0f };

		s_Data.QuadVertexPositions[36] = { -0.5f, -0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[37] = {  0.5f, -0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[38] = {  0.5f, -0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[39] = { -0.5f, -0.5f,  0.5f, 1.0f };

		s_Data.QuadVertexPositions[40] = { -0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[41] = {  0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[42] = {  0.5f,  0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[43] = { -0.5f,  0.5f, -0.5f, 1.0f };

		s_Data.QuadVertexPositions[44] = { -0.5f,  0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[45] = {  0.5f,  0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[46] = {  0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[47] = { -0.5f,  0.5f,  0.5f, 1.0f };


		s_Data.QuadVertexPositions[48] = { -0.5f,  0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[49] = { -0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[50] = {  0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[51] = {  0.5f,  0.5f, -0.5f, 1.0f };

		s_Data.QuadVertexPositions[52] = {  0.5f,  0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[53] = {  0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[54] = { -0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[55] = { -0.5f,  0.5f, -0.5f, 1.0f };

		s_Data.QuadVertexPositions[56] = {  0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[57] = {  0.5f,  0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[58] = { -0.5f,  0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[59] = { -0.5f,  0.5f,  0.5f, 1.0f };

		s_Data.QuadVertexPositions[60] = { -0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[61] = { -0.5f,  0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[62] = {  0.5f,  0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[63] = {  0.5f,  0.5f,  0.5f, 1.0f };

		s_Data.QuadVertexPositions[64] = {  0.5f,  0.5f, -0.5f, 1.0f };
		s_Data.QuadVertexPositions[65] = {  0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[66] = { -0.5f,  0.5f,  0.5f, 1.0f };
		s_Data.QuadVertexPositions[67] = { -0.5f,  0.5f, -0.5f, 1.0f };



		s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer3DData::CameraData), 0);
	}

	void Renderer3D::Shutdown()
	{
		SPIRIT_PROFILE_FUNCTION();

		delete[] s_Data.CubeVertexBufferBase;
	}

	void Renderer3D::BeginScene(const OrthographicCamera& camera)
	{
		SPIRIT_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjectionMatrix();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer3DData::CameraData));

		StartBatch();
	}

	void Renderer3D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		SPIRIT_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer3DData::CameraData));

		StartBatch();
	}

	void Renderer3D::BeginScene(const EditorCamera& camera)
	{
		SPIRIT_PROFILE_FUNCTION();

		s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
		s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer3DData::CameraData));

		StartBatch();
	}

	void Renderer3D::EndScene()
	{
		SPIRIT_PROFILE_FUNCTION();

		Flush();
	}

	void Renderer3D::StartBatch()
	{
		s_Data.QuadIndexCount = 0;
		s_Data.CubeVertexBufferPtr = s_Data.CubeVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer3D::Flush()
	{
		if (s_Data.QuadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CubeVertexBufferPtr - (uint8_t*)s_Data.CubeVertexBufferBase);
			s_Data.CubeVertexBuffer->SetData(s_Data.CubeVertexBufferBase, dataSize);

			// Bind textures
			for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
				s_Data.TextureSlots[i]->Bind(i);

			s_Data.CubeShader->Bind();
			RenderCommand::DrawIndexed(s_Data.CubeVertexArray, s_Data.QuadIndexCount);
			s_Data.Stats.DrawCalls++;
		}
	}

	void Renderer3D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer3D::DrawCube(const glm::vec2& position, const glm::vec3& size, const glm::vec4& color)
	{
		DrawCube({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer3D::DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color)
	{
		SPIRIT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, size.z });

		DrawCube(transform, color);
	}

	void Renderer3D::DrawCube(const glm::vec2& position, const glm::vec3& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, const glm::vec2& textureUVOffset)
	{
		DrawCube({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor, textureUVOffset);
	}

	void Renderer3D::DrawCube(const glm::vec3& position, const glm::vec3& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, const glm::vec2& textureUVOffset)
	{
		SPIRIT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, size.z });

		DrawCube(transform, texture, tilingFactor, tintColor, textureUVOffset);
	}

	void Renderer3D::DrawCube(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		SPIRIT_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 68;
		const float textureIndex = 0.0f; // White Texture

		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float tilingFactor = 1.0f;
		const glm::vec2 uvOffsetFactor = { 0.0f, 0.0f };

		if (s_Data.QuadIndexCount >= Renderer3DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.CubeVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.CubeVertexBufferPtr->Color = color;
			s_Data.CubeVertexBufferPtr->TexCoord = textureCoords[i] - uvOffsetFactor;
			s_Data.CubeVertexBufferPtr->TexIndex = textureIndex;
			s_Data.CubeVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.CubeVertexBufferPtr->EntityID = entityID;
			s_Data.CubeVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 68;

		s_Data.Stats.QuadCount++;
	}

	void Renderer3D::DrawCube(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, const glm::vec2& textureUVOffset, int entityID)
	{
		SPIRIT_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 68;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (s_Data.QuadIndexCount >= Renderer3DData::MaxIndices)
			NextBatch();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (*s_Data.TextureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			if (s_Data.TextureSlotIndex >= Renderer3DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.CubeVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.CubeVertexBufferPtr->Color = tintColor;
			s_Data.CubeVertexBufferPtr->TexCoord = textureCoords[i] - textureUVOffset;
			s_Data.CubeVertexBufferPtr->TexIndex = textureIndex;
			s_Data.CubeVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.CubeVertexBufferPtr->EntityID = entityID;
			s_Data.CubeVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 68;

		s_Data.Stats.QuadCount++;
	}

	void Renderer3D::DrawRotatedCube(const glm::vec2& position, const glm::vec3& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedCube({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer3D::DrawRotatedCube(const glm::vec3& position, const glm::vec3& size, float rotation, const glm::vec4& color)
	{
		SPIRIT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, size.z });

		DrawCube(transform, color);
	}

	void Renderer3D::DrawRotatedCube(const glm::vec2& position, const glm::vec3& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, const glm::vec2& textureUVOffset)
	{
		DrawRotatedCube({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor, textureUVOffset);
	}

	void Renderer3D::DrawRotatedCube(const glm::vec3& position, const glm::vec3& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, const glm::vec2& textureUVOffset)
	{
		SPIRIT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, size.z });

		DrawCube(transform, texture, tilingFactor, tintColor, textureUVOffset);
	}

	void Renderer3D::DrawTexturedCube(const glm::mat4& transform, MeshRendererComponent& src, int entityID)
	{
		if (src.Texture)
			DrawCube(transform, src.Texture, src.TilingFactor, src.Color, src.TextureUVOffset, entityID);
		else
			DrawCube(transform, src.Color, entityID);
	}

	void Renderer3D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Renderer3DStatistics));
	}

	Renderer3D::Renderer3DStatistics Renderer3D::GetStats()
	{
		return s_Data.Stats;
	}
}
