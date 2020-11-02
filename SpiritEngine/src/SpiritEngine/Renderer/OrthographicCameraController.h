#pragma once

#include "SpiritEngine/Renderer/OrthographicCamera.h"
#include "SpiritEngine/Core/Timestep.h"

#include "SpiritEngine/Events/ApplicationEvent.h"
#include "SpiritEngine/Events/MouseEvent.h"

namespace SpiritEngine {

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = false);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		void OnResize(float width, float height);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }

		float GetZoomLevel() const { return m_ZoomLevel; }
		/* Setting zoom level using this function will override min/max zoom level. */
		void SetZoomLevel(float level);

		float GetMagnificationZoomLevel() const { return 1.0f / m_ZoomLevel; }
		void SetMagnificationZoomLevel(float magnificationLevel) { if (magnificationLevel != 0.0f) SetZoomLevel(1.0f / magnificationLevel); }

		bool IsRotationEnabled() const { return m_Rotation; }
		void SetEnableRotation(bool enabled) { m_Rotation = enabled; }
	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);
		void UpdateProjectionMatrix() { m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel); }
	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		
		bool m_Rotation;

		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		float m_CameraRotation = 0.0f; //In degrees, in the anti-clockwise direction

		OrthographicCamera m_Camera;

		// TODO: add helper function to get/set those values, or public them;
		float m_CameraTranslationSpeed = 2.0f;
		float m_CameraRotationSpeed = 180.0f; //Also in degrees, in the anti-clockwise direction
		float m_ZoomSpeed = 0.25f;
		float m_MaxZoomLevel = 2.0f;
		float m_MinZoomLevel = 0.25f;
	};

}