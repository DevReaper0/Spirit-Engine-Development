#pragma once

#include "SpiritEngine/Core/Base.h"
#include "SpiritEngine/Core/Log.h"
#include "SpiritEngine/Scene/Scene.h"
#include "SpiritEngine/Scene/Entity.h"

namespace SpiritEngine
{

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender();
	private:
		void DrawEntityNode(Entity entity);
	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;
	};

}