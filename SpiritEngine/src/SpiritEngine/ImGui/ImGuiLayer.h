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
		
		//static bool ColorEdit4(string label, float* col);

		static void SetDarkThemeColors();
		static void SetLightThemeColors();
		static void SetPurpurThemeColors();
		static void SetOrangeThemeColors();
		static void SetCobaltThemeColors();
		static void SetDandelionThemeColors();
		static void SetRubyThemeColors();
		static void SetGoldenThemeColors();
		static void SetCharcoalThemeColors();
		static void SetCorporateGreyThemeColors();
		static void SetCherryThemeColors();
		static void SetSapphireThemeColors();
		static void SetCustomThemeColors(bool popup);
	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};

}
