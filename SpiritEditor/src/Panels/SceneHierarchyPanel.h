#pragma once

#include "SpiritEngine/Core/Base.h"
#include "SpiritEngine/Scene/Scene.h"
#include "SpiritEngine/Scene/Entity.h"

using namespace std;

namespace SpiritEngine {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		Entity GetSelectionContext();

		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};

}
