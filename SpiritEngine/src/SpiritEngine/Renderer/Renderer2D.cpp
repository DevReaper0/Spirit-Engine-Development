#include "spiritpch.h"
#include "SpiritEngine/Renderer/Renderer2D.h"

#include "SpiritEngine/Renderer/VertexArray.h"
#include "SpiritEngine/Renderer/Shader.h"
#include "SpiritEngine/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

#define UseLineShader

namespace SpiritEngine {

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;
	};

	struct LineVertex
	{
		glm::vec4 ClipCrd;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float Width;
		float Length;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps
		
		static const uint32_t MaxLines = 20000;
		static const uint32_t MaxLineVertices = MaxLines * 4;
		static const uint32_t MaxLineIndices = MaxLines * 6;

		Ref<VertexArray> QuadVertexArray;
		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<Shader> TextureShader;
		Ref<Texture2D> WhiteTexture;

		Ref<VertexArray> LineVertexArray;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<Shader> LineShader;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		uint32_t LineIndexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		glm::vec4 QuadVertexPositions[4];

		Renderer2D::Statistics Stats;

		glm::mat4 ViewProjectionMatrix;
		uint32_t Width = 1600, Height = 900;
	};

	static Renderer2DData s_Data;

	void Renderer2D::Init()
	{
		SPIRIT_PROFILE_FUNCTION();

		// Quads
		{
			s_Data.QuadVertexArray = VertexArray::Create();
			
			s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
			s_Data.QuadVertexBuffer->SetLayout({
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float4, "a_Color" },
				{ ShaderDataType::Float2, "a_TexCoord" },
				{ ShaderDataType::Float, "a_TexIndex" },
				{ ShaderDataType::Float, "a_TilingFactor" }
				});
			s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

			s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

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
			s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
			delete[] quadIndices;
		}

#ifdef UseLineShader
		// Lines
		{
			s_Data.LineVertexArray = VertexArray::Create();

			s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxLineVertices * sizeof(LineVertex));
			s_Data.LineVertexBuffer->SetLayout({
				{ ShaderDataType::Float4, "a_ClipCrd" },
				{ ShaderDataType::Float4, "a_Color" },
				{ ShaderDataType::Float2, "a_TexCoord" },
				{ ShaderDataType::Float, "a_Width" },
				{ ShaderDataType::Float, "a_Length" }
				});
			s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);

			s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxLineVertices];

			uint32_t* TeLineIndices = new uint32_t[s_Data.MaxLineIndices];

			uint32_t offset = 0;
			for (uint32_t i = 0; i < s_Data.MaxLineIndices; i += 6)
			{
				TeLineIndices[i + 0] = offset + 0;
				TeLineIndices[i + 1] = offset + 1;
				TeLineIndices[i + 2] = offset + 2;

				TeLineIndices[i + 3] = offset + 2;
				TeLineIndices[i + 4] = offset + 3;
				TeLineIndices[i + 5] = offset + 0;

				offset += 4;
			}

			Ref<IndexBuffer> TeLinesIB = IndexBuffer::Create(TeLineIndices, s_Data.MaxLineIndices);
			s_Data.LineVertexArray->SetIndexBuffer(TeLinesIB);
			delete[] TeLineIndices;
		}
#endif

		// Textures and shaders
		s_Data.WhiteTexture = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		int32_t samplers[s_Data.MaxTextureSlots];
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
			samplers[i] = i;

		s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

#ifdef UseLineShader
		s_Data.LineShader = Shader::Create("assets/shaders/Line.glsl");
		s_Data.LineShader->Bind();
		s_Data.LineShader->SetInt("u_Caps", 3); //0 none, 1 square, 2 round, 3 triangle
#endif

		// Set first texture slot to 0
		s_Data.TextureSlots[0] = s_Data.WhiteTexture;

		s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer2D::Shutdown()
	{
		SPIRIT_PROFILE_FUNCTION();

		delete[] s_Data.QuadVertexBufferBase;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		SPIRIT_PROFILE_FUNCTION();

		s_Data.ViewProjectionMatrix = camera.GetViewProjectionMatrix();

		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

		StartBatch();
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		SPIRIT_PROFILE_FUNCTION();

		glm::mat4 viewProj = camera.GetProjection() * glm::inverse(transform);

		s_Data.TextureShader->Bind();
		s_Data.TextureShader->SetMat4("u_ViewProjection", viewProj);

		StartBatch();
	}

	void Renderer2D::EndScene()
	{
		SPIRIT_PROFILE_FUNCTION();

		Flush();
	}

	void Renderer2D::StartBatch()
	{
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.LineIndexCount = 0;
		s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::Flush()
	{
		if (s_Data.QuadIndexCount == 0)
			return; // Nothing to draw

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
		s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

#ifdef UseLineShader
		dataSize = (uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase;
		s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);
#endif
		
		// Bind textures
		for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
			s_Data.TextureSlots[i]->Bind(i);
		
		s_Data.QuadVertexBuffer->Bind();
		s_Data.TextureShader->Bind();
		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);

#ifdef UseLineShader
		s_Data.LineVertexBuffer->Bind();
		s_Data.LineShader->Bind();
		s_Data.LineVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.LineVertexArray, s_Data.LineIndexCount);
#endif

		s_Data.Stats.DrawCalls++;
	}
	
	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	void Renderer2D::SetViewport(uint32_t width, uint32_t height)
	{
		s_Data.Width = width;
		s_Data.Height = height;
	}

	void Renderer2D::DrawLine(const glm::vec3& positionI, const glm::vec3& positionJ, float width, const glm::vec4& color)
	{
		if (s_Data.LineIndexCount >= Renderer2DData::MaxLineIndices)
			NextBatch();

		//world to clip
		glm::vec4 clipI = s_Data.ViewProjectionMatrix * glm::vec4(positionI.x, positionI.y, positionI.z, 1.0f);
		glm::vec4 clipJ = s_Data.ViewProjectionMatrix * glm::vec4(positionJ.x, positionJ.y, positionJ.z, 1.0f);

		//clip to pixel
		glm::vec2 pixelI, pixelJ;
		pixelI.x = 0.5f * (float)s_Data.Width * (clipI.x / clipI.w + 1.0f);
		pixelI.y = 0.5f * (float)s_Data.Height * (1.0f - clipI.y / clipI.w);
		pixelJ.x = 0.5f * (float)s_Data.Width * (clipJ.x / clipJ.w + 1.0f);
		pixelJ.y = 0.5f * (float)s_Data.Height * (1.0f - clipJ.y / clipJ.w);

		glm::vec2 dir = { pixelJ.x - pixelI.x, pixelJ.y - pixelI.y };
		float LineLength = glm::length(dir);

		if (LineLength < 1e-10) return;

		dir /= LineLength;
		glm::vec2 normal = { -dir.y, +dir.x };

		float d = 0.5f * width;

		float dOverWidth = d / (float)s_Data.Width;
		float dOverHeight = d / (float)s_Data.Height;

		glm::vec4 offset(0.0f);

		offset.x = (-dir.x + normal.x) * dOverWidth;
		offset.y = (+dir.y - normal.y) * dOverHeight;
		s_Data.LineVertexBufferPtr->ClipCrd = clipI + offset;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->TexCoord = { -d, +d };
		s_Data.LineVertexBufferPtr->Width = 2 * d;
		s_Data.LineVertexBufferPtr->Length = LineLength;
		s_Data.LineVertexBufferPtr++;

		offset.x = (+dir.x + normal.x) * dOverWidth;
		offset.y = (-dir.y - normal.y) * dOverHeight;
		s_Data.LineVertexBufferPtr->ClipCrd = clipJ + offset;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->TexCoord = { LineLength + d, +d };
		s_Data.LineVertexBufferPtr->Width = 2 * d;
		s_Data.LineVertexBufferPtr->Length = LineLength;
		s_Data.LineVertexBufferPtr++;

		offset.x = (+dir.x - normal.x) * dOverWidth;
		offset.y = (-dir.y + normal.y) * dOverHeight;
		s_Data.LineVertexBufferPtr->ClipCrd = clipJ + offset;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->TexCoord = { LineLength + d, -d };
		s_Data.LineVertexBufferPtr->Width = 2 * d;
		s_Data.LineVertexBufferPtr->Length = LineLength;
		s_Data.LineVertexBufferPtr++;

		offset.x = (-dir.x - normal.x) * dOverWidth;
		offset.y = (+dir.y + normal.y) * dOverHeight;
		s_Data.LineVertexBufferPtr->ClipCrd = clipI + offset;
		s_Data.LineVertexBufferPtr->Color = color;
		s_Data.LineVertexBufferPtr->TexCoord = { -d, -d };
		s_Data.LineVertexBufferPtr->Width = 2 * d;
		s_Data.LineVertexBufferPtr->Length = LineLength;
		s_Data.LineVertexBufferPtr++;

		s_Data.LineIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawTriangle(const glm::mat4& transform, const glm::vec4& color)
	{
		SPIRIT_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 3;
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 0.5f }};
		const float tilingFactor = 1.0f;

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawLine(const glm::mat4& transform, const glm::vec4& color)
	{
		SPIRIT_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 2;
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 1.0f } };
		const float tilingFactor = 1.0f;

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		SPIRIT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		
		DrawQuad(transform, color);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		SPIRIT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		SPIRIT_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float tilingFactor = 1.0f;

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			NextBatch();

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = color;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		SPIRIT_PROFILE_FUNCTION();

		constexpr size_t quadVertexCount = 4;
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
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
			if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
				NextBatch();

			textureIndex = (float)s_Data.TextureSlotIndex;
			s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
			s_Data.TextureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
			s_Data.QuadVertexBufferPtr->Color = tintColor;
			s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
			s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
			s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
			s_Data.QuadVertexBufferPtr++;
		}

		s_Data.QuadIndexCount += 6;

		s_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		SPIRIT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		SPIRIT_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		DrawQuad(transform, texture, tilingFactor, tintColor);
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}

}
