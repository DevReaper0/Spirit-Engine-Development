#pragma once

#include "SpiritEngine/Core/KeyCodes.h"
#include "SpiritEngine/Core/MouseCodes.h"

#include <glm/glm.hpp>

namespace SpiritEngine {

	// TODO: If this enum class grows too big, we will move it to another .h file;
	/* Hold the name for an input to be mapped. */
	enum class InputName : uint16_t
	{
		CameraMoveLeft = 1,
		CameraMoveRight = 2,
		CameraMoveUp = 3,
		CameraMoveDown = 4,
		CameraRotateClockwise = 5,
		CameraRotateAntiClockwise = 6,
	};

	/* Holds a single input, either a KeyCode or a MouseCode. */
	struct InputKey
	{
		std::variant<KeyCode, MouseCode> value;

		InputKey() = default;
		InputKey(KeyCode key) { value = key; }
		InputKey(MouseCode button) { value = button; }
	};

	class Input
	{
	public:
		template <typename T>
		static bool IsInputPressed(T name)
		{
			if (m_Mapping.count(static_cast<uint16_t>(name)) != 0)
				return IsInputPressed(m_Mapping[static_cast<uint16_t>(name)]);

			SPIRIT_CORE_WARN("The input {0} is not mapped!", static_cast<uint16_t>(name));
			return false;
		}

		inline static bool IsInputPressed(InputKey inputKey)
		{
			if (std::holds_alternative<KeyCode>(inputKey.value))
				return IsKeyPressed(std::get<KeyCode>(inputKey.value));

			if (std::holds_alternative<MouseCode>(inputKey.value))
				return IsMouseButtonPressed(std::get<MouseCode>(inputKey.value));

			// control should never fall down here;
			SPIRIT_CORE_WARN("Unknown Input type!");
			return false;
		}

		static bool IsKeyPressed(KeyCode key);
		static bool IsKeyDown(KeyCode keycode);
		static bool IsKeyUp(KeyCode keycode);

		static bool IsMouseButtonPressed(MouseCode button);
		static bool IsMouseButtonDown(MouseCode button);
		static bool IsMouseButtonUp(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();

		/* Map a Key/Mouse Button to an InputKey, and overwrite the old value if existed. */
		template <typename T>
		static void Remap(T name, InputKey inputKey) { s_Instance->m_Mapping[static_cast<uint16_t>(name)] = inputKey; }
	protected:
		static bool GetKey(KeyCode keycode);
		static bool GetMouseButton(MouseCode mousecode);
	private:
		friend class Application;

		static void OnUpdate();
		std::unordered_map<uint16_t, InputKey> m_Mapping;
	};
}
