#pragma once

#include <iostream>
//#include <GL/glew.h>
//#include <GLFW/glfw3.h>

namespace Spirit
{
    namespace Graphics
    {

#define MAX_KEYS 1024
#define MAX_BUTTONS 32

        __declspec(dllexport) class WindowsWindow
        {
        private:
            friend struct GLFWwindow;
        private:
            const char* m_Title;
            int m_Width, m_Height;
            GLFWwindow* m_Window;
            bool m_closed;

            bool m_Keys[MAX_KEYS];
            bool m_MouseButtons[MAX_BUTTONS];
            double mx, my;
        public:
            WindowsWindow(const char* title, int width, int height);
            ~WindowsWindow();
            void clear() const;
            void update();
            bool closed() const;

            inline int getWidth() const { return m_Width; }
            inline int getHeight() const { return m_Height; }

            bool isKeyPressed(unsigned int keyCode) const;
            bool isMouseButtonPressed(unsigned int button) const;
            void getMousePosition(double& x, double& y) const;
        private:
            bool init();
            friend static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
            friend static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
            friend static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
        };

    }
}