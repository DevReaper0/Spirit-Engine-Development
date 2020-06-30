#include "spiritpch.h"

#include "Window.h"

namespace Spirit
{
    WindowsWindow* Window::CreateWindowsWindow(const char* title, int width, int height)
    {
        windowTitle = title;
        windowWidth = width;
        windowHeight = height;

        return new WindowsWindow(title, width, height);
    }

    int main()
    {
        Spirit::Window win;
        //win.CreateWindowsWindow(windowTitle, windowWidth, windowHeight);
        win.PLATFORM(windowTitle, windowWidth, windowHeight);

        return 0;
    }
}