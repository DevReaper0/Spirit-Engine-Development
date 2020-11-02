#include "spiritpch.h"
#include "SpiritEngine/Core/Input.h"

#include "SpiritEngine/Core/Application.h"
#include <GLFW/glfw3.h>

namespace SpiritEngine
{
#pragma region Codes
		static std::unordered_map<KeyCode, bool> s_KeyStateMap(0);
		static std::unordered_map<MouseCode, bool> s_MouseButtonStateMap(0);

		static std::vector<KeyCode> s_AllKeys =
		{
			Key::Space,
			Key::Apostrophe,
			Key::Comma,
			Key::Minus,
			Key::Period,
			Key::Slash,
			Key::D0,
			Key::D1,
			Key::D2,
			Key::D3,
			Key::D4,
			Key::D5,
			Key::D6,
			Key::D7,
			Key::D8,
			Key::D9,
			Key::Semicolon,
			Key::Equal,
			Key::A,
			Key::B,
			Key::C,
			Key::D,
			Key::E,
			Key::F,
			Key::G,
			Key::H,
			Key::I,
			Key::J,
			Key::K,
			Key::L,
			Key::M,
			Key::N,
			Key::O,
			Key::P,
			Key::Q,
			Key::R,
			Key::S,
			Key::T,
			Key::U,
			Key::V,
			Key::W,
			Key::X,
			Key::Y,
			Key::Z,
			Key::LeftBracket,
			Key::Backslash,
			Key::RightBracket,
			Key::GraveAccent,
			Key::World1,
			Key::World2,
			Key::Escape,
			Key::Enter,
			Key::Tab,
			Key::Backspace,
			Key::Insert,
			Key::Delete,
			Key::Right,
			Key::Left,
			Key::Down,
			Key::Up,
			Key::PageUp,
			Key::PageDown,
			Key::Home,
			Key::End,
			Key::CapsLock,
			Key::ScrollLock,
			Key::NumLock,
			Key::PrintScreen,
			Key::Pause,
			Key::F1,
			Key::F2,
			Key::F3,
			Key::F4,
			Key::F5,
			Key::F6,
			Key::F7,
			Key::F8,
			Key::F9,
			Key::F10,
			Key::F11,
			Key::F12,
			Key::F13,
			Key::F14,
			Key::F15,
			Key::F16,
			Key::F17,
			Key::F18,
			Key::F19,
			Key::F20,
			Key::F21,
			Key::F22,
			Key::F23,
			Key::F24,
			Key::F25,
			Key::KP0,
			Key::KP1,
			Key::KP2,
			Key::KP3,
			Key::KP4,
			Key::KP5,
			Key::KP6,
			Key::KP7,
			Key::KP8,
			Key::KP9,
			Key::KPDecimal,
			Key::KPDivide,
			Key::KPMultiply,
			Key::KPSubtract,
			Key::KPAdd,
			Key::KPEnter,
			Key::KPEqual,
			Key::LeftShift,
			Key::LeftControl,
			Key::LeftAlt,
			Key::LeftSuper,
			Key::RightShift,
			Key::RightControl,
			Key::RightAlt,
			Key::RightSuper,
			Key::Menu
		};

		static std::vector<MouseCode> s_AllMouseButtons =
		{
			Mouse::Button0,
			Mouse::Button1,
			Mouse::Button2,
			Mouse::Button3,
			Mouse::Button4,
			Mouse::Button5,
			Mouse::Button6,
			Mouse::Button7
		};
#pragma endregion

	bool Input::IsKeyPressed(KeyCode key)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

#ifdef Non-QWERTY
		key = Input::GetCrossLayoutKey(key);
#endif

		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsKeyDown(const KeyCode keycode)
	{
		return GetKey(keycode) && !s_KeyStateMap[keycode];
	}

	bool Input::IsKeyUp(const KeyCode keycode)
	{
		return !GetKey(keycode) && s_KeyStateMap[keycode];
	}

#ifdef Non-QWERTY
	KeyCode Input::GetCrossLayoutKeyImpl(KeyCode key)
	{
		const char* keyName = glfwGetKeyName(int(key), glfwGetKeyScancode(int(key)));

		//Check if the key is concerned by keyboards layout (letters)
		if (key >= Key::A && key <= Key::Z)
		{
			return KeyCode(int(Key::A) + (std::toupper(keyName[0]) - 'A')); //Way to ensure that no matter the code that Hazel uses, the keycode will be correct
		}

		//Digit
		else if ((key >= Key::D0 && key <= Key::D9) || (key >= Key::KP0 && key <= Key::KP9))
		{
			return key; //We treat digit as they are (regardless of the layout...)
		}
		//Other keys (mostly punctuation)
		else if (key > Key::Space && key <= Key::World2)
		{

			//Safety check for eventual gaps in Key codes
			if (keyName != NULL)
			{
				return KeyCode(int(Key::Space) + (std::toupper(keyName[0]) - ' '));
			}

		}

		//the other keys are independant of the layout (like 'Space' 'Shift' etc.)
		return key;
	}
#endif

	bool Input::IsMouseButtonPressed(const MouseCode button)
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonDown(const MouseCode button)
	{
		return GetMouseButton(button) && !s_MouseButtonStateMap[button];
	}

	bool Input::IsMouseButtonUp(const MouseCode button)
	{
		return !GetMouseButton(button) && s_MouseButtonStateMap[button];
	}

	glm::vec2 Input::GetMousePosition()
	{
		auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	bool Input::GetKey(const KeyCode keycode)
	{
		return glfwGetKey(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()),
			static_cast<int32_t>(keycode)) == GLFW_PRESS;
	}

	bool Input::GetMouseButton(const MouseCode mousecode)
	{
		return glfwGetMouseButton(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()),
			static_cast<int32_t>(mousecode)) == GLFW_PRESS;
	}

	void Input::OnUpdate()
	{
		for (KeyCode key : s_AllKeys)
		{
			s_KeyStateMap[key] = GetKey(key);
		}

		for (MouseCode mouseButton : s_AllMouseButtons)
		{
			s_MouseButtonStateMap[mouseButton] = GetMouseButton(mouseButton);
		}
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}

}