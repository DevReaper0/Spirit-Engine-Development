#include <SpiritEngine.h>
#include <SpiritEngine/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace SpiritEngine {

	class Spirit : public Application
	{
	public:
		Spirit()
			: Application("Spirit Engine")
		{
			PushLayer(new EditorLayer());
		}

		~Spirit()
		{
		}
	};

	Application* CreateApplication()
	{
		return new Spirit();
	}

}