#pragma once

#include <glm/glm.hpp>

#include "SpiritEngine/Core/KeyCodes.h"
#include "SpiritEngine/Core/MouseCodes.h"

namespace SpiritEngine {

	class Input
	{
	public:
#ifdef Non-QWERTY
		static KeyCode GetCrossLayoutKey(KeyCode key) { return s_Instance->GetCrossLayoutKeyImpl(key); }
#endif
		static bool IsKeyPressed(KeyCode key);
		static bool IsKeyDown(KeyCode keycode);
		static bool IsKeyUp(KeyCode keycode);

		static bool IsMouseButtonPressed(MouseCode button);
		static bool IsMouseButtonDown(MouseCode button);
		static bool IsMouseButtonUp(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	protected:
		static bool GetKey(KeyCode keycode);
		static bool GetMouseButton(MouseCode mousecode);

	private:
		friend class Application;

		static void OnUpdate();
#ifdef Non-QWERTY
	protected:
		virtual KeyCode GetCrossLayoutKeyImpl(KeyCode key) = 0;
	private:
		static Scope<Input> s_Instance;
#endif
	};
}
