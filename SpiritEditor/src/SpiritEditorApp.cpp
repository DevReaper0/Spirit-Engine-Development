#define enttinclude

#include <SpiritEngine.h>
#include <SpiritEngine/Core/EntryPoint.h>

#include "EditorLayer.h"
#include "entt.hpp"

namespace SpiritEngine {

	class SpiritEditor : public Application
	{
	public:
		SpiritEditor(ApplicationCommandLineArgs args, std::string prefix = "", std::string name = "Spirit Engine", std::string suffix = "")
			: Application(prefix + name + suffix, args)
		{
			PushLayer(new EditorLayer());
		}

		~SpiritEditor()
		{
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		std::string appPrefix = "";
		std::string appName = "Spirit Engine";
		std::string appSuffix = "";

		std::string prefix = "";
		// Always set a seed value.
		srand((unsigned int)time(NULL));
		int RandomPrefixValue = rand() % 7;

		if (RandomPrefixValue == 0)
			prefix = "Dream, Create, Release";
		else if (RandomPrefixValue == 1)
			prefix = "Dream!";
		else if (RandomPrefixValue == 2)
			prefix = "Create!";
		else if (RandomPrefixValue == 3)
			prefix = "Release!";
		else if (RandomPrefixValue == 4)
			prefix = "Home Of The Spirits!";
		else if (RandomPrefixValue == 5)
			prefix = "Try Spiritual";
		else if (RandomPrefixValue == 6)
			prefix = "";

		if (prefix != "")
			prefix = prefix + " - ";
		else
		{
			prefix = appPrefix;
			if (prefix != "")
				prefix = ": " + prefix;
			else
				prefix = "";
		}

		std::string suffix = "";
		// Always set a seed value.
		srand((unsigned int)time(NULL));
		int RandomSuffixValue = rand() % 7;

		if (RandomSuffixValue == 0)
			suffix = "Dream, Create, Release";
		else if (RandomSuffixValue == 1)
			suffix = "Dream!";
		else if (RandomSuffixValue == 2)
			suffix = "Create!";
		else if (RandomSuffixValue == 3)
			suffix = "Release!";
		else if (RandomSuffixValue == 4)
			suffix = "Home Of The Spirits!";
		else if (RandomSuffixValue == 5)
			suffix = "Try Spiritual";
		else if (RandomSuffixValue == 6)
			suffix = "";

		if (suffix != "")
			suffix = ": " + suffix;
		else
		{
			suffix = appSuffix;
			if (suffix != "")
				suffix = ": " + suffix;
			else
				suffix = "";
		}

		// For if there is anything important or an easter egg
		std::string name = "";
		// Always set a seed value.
		srand((unsigned int)time(NULL));
		int RandomNameValue = rand() % 5;

		int a = 1;
		if (RandomNameValue == 0)
			name = "Spirit Editor";
		else
			name = appName;

		// PREFIX: Spirit Engine: SUFFIX
		// EXAMPLE:
		// Dream! - Spirit Engine: Create!
		return new SpiritEditor(args, "", appName, suffix);
	}

}