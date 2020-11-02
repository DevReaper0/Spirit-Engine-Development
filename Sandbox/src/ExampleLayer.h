#pragma once

#include "SpiritEngine.h"

enum CustomEventTypes
{
	TestEventType
};

class TestEvent : public SpiritEngine::CustomEvent
{
public:
	TestEvent() {}

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "TestEvent" << std::endl;
		return ss.str();
	}

	EVENT_CLASS_CUSTOM_TYPE(TestEventType)
};

class ExampleLayer : public SpiritEngine::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(SpiritEngine::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(SpiritEngine::Event& e) override;

	bool OnTestEvent(TestEvent& e);
	void OnCustomEvent(SpiritEngine::CustomEvent& e);
private:
	SpiritEngine::ShaderLibrary m_ShaderLibrary;
	SpiritEngine::Ref<SpiritEngine::Shader> m_Shader;
	SpiritEngine::Ref<SpiritEngine::VertexArray> m_VertexArray;

	SpiritEngine::Ref<SpiritEngine::Shader> m_FlatColorShader;
	SpiritEngine::Ref<SpiritEngine::VertexArray> m_SquareVA;

	SpiritEngine::Ref<SpiritEngine::Texture2D> m_Texture, m_ChernoLogoTexture;

	SpiritEngine::OrthographicCameraController m_CameraController;
	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};

