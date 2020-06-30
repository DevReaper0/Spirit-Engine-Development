#include "spiritpch.h"
#include <GLFW/glfw3.h>
#include "WindowsWindow.h"

namespace Spirit
{ 
    namespace Graphics
    {

        void windowResize(GLFWwindow* window, int width, int height);
        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

        WindowsWindow::WindowsWindow(const char* title, int width, int height)
        {
            m_Title = title;
            m_Width = width;
            m_Height = height;
            if (!init())
                glfwTerminate();

            for (int i = 0; i < MAX_KEYS; i++)
            {
                m_Keys[i] = false;
            }

            for (int i = 0; i < MAX_BUTTONS; i++)
            {
                m_MouseButtons[i] = false;
            }
        }

        WindowsWindow::~WindowsWindow()
        {
            glfwTerminate();
        }

        bool WindowsWindow::init()
        {
            if (!glfwInit())
            {
                std::cout << "Failed to initialize GLFW!" << std::endl;
                return false;
            }
            m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, NULL, NULL);
            if (!m_Window)
            {
                std::cout << "Failed to create GLFW window!" << std::endl;
                return false;
            }
            glfwMakeContextCurrent(m_Window);
            glfwSetWindowUserPointer(m_Window, this);
            glfwSetWindowSizeCallback(m_Window, windowResize);
            glfwSetKeyCallback(m_Window, key_callback);
            glfwSetMouseButtonCallback(m_Window, mouse_button_callback);
            glfwSetCursorPosCallback(m_Window, cursor_position_callback);

            /*if (glewInit() != GLEW_OK)
            {
                std::cout << "Could not initialize GLEW!" << std::endl;
                return false;
            }*/

            std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;

            return true;
        }

        bool WindowsWindow::isKeyPressed(unsigned int keyCode) const
        {
            // TODO: Log this!
            if (keyCode >= MAX_KEYS)
                return false;
            return m_Keys[keyCode];
        }

        bool WindowsWindow::isMouseButtonPressed(unsigned int button) const
        {
            // TODO: Log this!
            if (button >= MAX_BUTTONS)
                return false;
            return m_MouseButtons[button];
        }

        void WindowsWindow::getMousePosition(double& x, double& y) const
        {
            x = mx;
            y = my;
        }

        void WindowsWindow::clear() const
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        void WindowsWindow::update()
        {
            glfwSwapBuffers(m_Window);
            //glfwGetFramebufferSize(m_Window, &m_Width, &m_Height);
            glfwPollEvents();
        }

        bool WindowsWindow::closed() const
        {
            return glfwWindowShouldClose(m_Window) == 1;
        }

        void windowResize(GLFWwindow* window, int width, int height)
        {
            glViewport(0, 0, width, height);
        }

        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            WindowsWindow* win = (WindowsWindow*)glfwGetWindowUserPointer(window);
            win->m_Keys[key] = action != GLFW_RELEASE;
        }

        void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
        {
            WindowsWindow* win = (WindowsWindow*)glfwGetWindowUserPointer(window);
            win->m_MouseButtons[button] = action != GLFW_RELEASE;
        }

        void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
        {
            WindowsWindow* win = (WindowsWindow*)glfwGetWindowUserPointer(window);
            win->mx = xpos;
            win->my = ypos;
        }

    }
}