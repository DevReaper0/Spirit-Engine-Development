#pragma once

#include <Spirit.h>
#include "Spirit/Platform/Windows/WindowsWindow.h"

using namespace Spirit;
using namespace Graphics;

const char* windowTitle = "Spirit Engine";
unsigned int windowWidth = 1280;
unsigned int windowHeight = 720;

#ifdef SPIRIT_PLATFORM_WINDOWS
    #define PLATFORM CreateWindowsWindow
#endif

namespace Spirit
{
    class SPIRIT_API Window
    {
        struct WindowProps
        {
            const char* Title;
            unsigned int Width;
            unsigned int Height;

            WindowProps(const char* title = "Spirit Engine",
                unsigned int width = 1280,
                unsigned int height = 720)
                : Title(title), Width(width), Height(height)
            {
                windowTitle = title;
                windowWidth = width;
                windowHeight = height;
            }
        };

    public:
        WindowsWindow* CreateWindowsWindow(const char* title, int width, int height);
    };
}