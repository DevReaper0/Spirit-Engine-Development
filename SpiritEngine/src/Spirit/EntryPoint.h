#pragma once

//#include "Spirit/Log.h"

#ifdef SPIRIT_PLATFORM_WINDOWS

extern Spirit::Application* Spirit::CreateApplication();

int main(int argc, char** argv)
{
    //Spirit::Log::Init();
    //Spirit::Log::GetCoreLogger()->warn("Inialized Log!");
    //Spirit::Log::GetClientLogger()->info("Hello!");

    auto app = Spirit::CreateApplication();
    app->Run();
    delete app;
}

#endif
