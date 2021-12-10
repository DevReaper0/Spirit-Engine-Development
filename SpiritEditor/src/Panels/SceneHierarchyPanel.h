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

		void SetSelectedEntity(Entity entity ) { m_SelectionContext = entity; };
		Entity GetSelectedEntity() const { return m_SelectionContext; }

		void EntityCreation();
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};

}
