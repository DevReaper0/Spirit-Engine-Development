#pragma once

#include "SpiritEngine/Core/Layer.h"

#include "SpiritEngine/Events/ApplicationEvent.h"
#include "SpiritEngine/Events/KeyEvent.h"
#include "SpiritEngine/Events/MouseEvent.h"

namespace SpiritEngine {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }

		static void SetDarkThemeColors();
		static void SetLightThemeColors();
		static void SetCorporateGreyThemeColors();
		static void SetClassicDarkThemeColors();
		static void SetClassicLightThemeColors();
		static void SetClassicThemeColors();

		static std::string GetTheme() { return m_Theme; }
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;

		static std::string m_Theme;
	};

}
