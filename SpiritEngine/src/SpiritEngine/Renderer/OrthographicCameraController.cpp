#include "spiritpch.h"
#include "SpiritEngine/Renderer/OrthographicCameraController.h"

#include "SpiritEngine/Core/Input.h"
#include "SpiritEngine/Core/KeyCodes.h"

namespace SpiritEngine {

	OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
		: m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_EnableRotation(rotation)
	{
		/* Those remaps can be done (and should be done) at user-level layers or
		 * apps). However, since this CameraController is using those Key Inputs
		 * initially, if user forgot to map those key, camera will not react to
		 * inputs, so we pre-map at least those keys here.
		 */
		 //Input::Remap("camera_move_left", SpiritEngine::InputKey(SPIRIT_KEY_A)); // this also works;
		Input::Remap(InputName::CameraMoveLeft, SpiritEngine::InputKey(SpiritEngine::Key::A));
		Input::Remap(InputName::CameraMoveRight, SpiritEngine::InputKey(SpiritEngine::Key::D));
		Input::Remap(InputName::CameraMoveUp, SpiritEngine::InputKey(SpiritEngine::Key::W));
		Input::Remap(InputName::CameraMoveDown, SpiritEngine::InputKey(SpiritEngine::Key::S));

		Input::Remap(InputName::CameraRotateClockwise, SpiritEngine::InputKey(SpiritEngine::Key::E));
		Input::Remap(InputName::CameraRotateAntiClockwise, SpiritEngine::InputKey(SpiritEngine::Key::Q));
	}

	void OrthographicCameraController::OnUpdate(Timestep ts)
	{
		SPIRIT_PROFILE_FUNCTION();

		if (Input::IsInputPressed(InputName::CameraMoveLeft))
		{
			m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}
		else if (Input::IsInputPressed(InputName::CameraMoveRight))
		{
			m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}

		if (Input::IsInputPressed(InputName::CameraMoveUp))
		{
			m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}
		else if (Input::IsInputPressed(InputName::CameraMoveDown))
		{
			m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
			m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		}

		if (m_EnableRotation)
		{
			if (Input::IsInputPressed(InputName::CameraRotateClockwise))
				m_CameraRotation += m_CameraRotationSpeed * ts;
			if (Input::IsInputPressed(InputName::CameraRotateAntiClockwise))
				m_CameraRotation -= m_CameraRotationSpeed * ts;

			if (m_CameraRotation > 180.0f)
				m_CameraRotation -= 360.0f;
			else if (m_CameraRotation <= -180.0f)
				m_CameraRotation += 360.0f;

			m_Camera.SetRotation(m_CameraRotation);
		}

		m_Camera.SetPosition(m_CameraPosition);

		//m_CameraTranslationSpeed = m_ZoomLevel;
	}

	void OrthographicCameraController::SetZoomLevel(float level)
	{
		SPIRIT_CORE_ASSERT(level > 0.0f, "Zoom Level has to be higher than 0.0f!");

		// translation speed should scale relative to the zoom level changes;
		float diff = level / m_ZoomLevel; // zoom in -> fast translation, zoom out -> slow translation;
		m_CameraTranslationSpeed *= diff;
		m_ZoomLevel = level;
		UpdateProjectionMatrix();
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		SPIRIT_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(SPIRIT_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(SPIRIT_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	void OrthographicCameraController::OnResize(float width, float height)
	{
		m_AspectRatio = width / height;
		UpdateProjectionMatrix();
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		SPIRIT_PROFILE_FUNCTION();

		float zoomLevel = m_ZoomLevel - e.GetYOffset() * m_ZoomSpeed;
		/* Spirit Engine's current "Zoom Level" definition makes std::clamp confuses:
		 *   - 0.25f Zoom Level -> 4x magnification
		 *   - 2.0f Zoom Level -> 0.5x magnification
		 * so the min and max value of "Zoom Level" and "Magnification Level" is the invertion of each other.
		 * If we set MaxZoomLevel to 0.25f (4x) and MinZoomLevel to 2.0f (0.5x) then the user will confuses
		 * "why Max is smaller than Min??".
		 *
		 * Suggestion: Changing the definition to "Magnification Level".
		 */
		zoomLevel = std::clamp(zoomLevel, m_MinZoomLevel, m_MaxZoomLevel);
		SetZoomLevel(zoomLevel);
		UpdateProjectionMatrix();
		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		SPIRIT_PROFILE_FUNCTION();

		OnResize((float)e.GetWidth(), (float)e.GetHeight());
		return false;
	}

}