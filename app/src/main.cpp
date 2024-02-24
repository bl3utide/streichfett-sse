#include "common.hpp"
#include "error.hpp"
#include "main.hpp"
#include "state.hpp"
#include "gui/gui.hpp"
#include "util/string_util.hpp"
#ifdef _DEBUG
#include "logger.hpp"
#endif

namespace StreichfettSse
{

// private
const char* APP_NAME = DEF_APP_NAME;
const char* APP_VERSION = DEF_APP_VERSION;
const std::string APP_COPYRIGHT = format("Copyright (C) %d %s", DEF_APP_DEV_YR, DEF_APP_DEV_BY);
const char* APP_TITLE = DEF_APP_TITLE;
#ifdef _DEBUG
const std::string DEBUG_FILE_NAME = format("%s.debug.log", APP_NAME);
#endif

void initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        throw std::runtime_error("SDL_Init error");
    }

    Gui::initialize(APP_TITLE);
}

void finalize() noexcept
{
    Gui::finalize();

    SDL_Quit();
}

void loop()
{
    SDL_Event event;
    bool running = true;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                running = false;
        }

        try
        {
            switch (getState())
            {
                case State::InitInternalData:
                    setNextState(State::Idle);
                    break;
                case State::Idle:
                    break;
                default:
                    break;
            }
        }
        catch (std::exception& error)
        {
#ifdef _DEBUG
            LOGD << error.what();
#endif
            setAppError(format("General error: %s", error.what()));
        }

        if (getNextState() == State::None)
        {
            try
            {
                Gui::drawGui();
            }
            catch (std::exception& error)
            {
#ifdef _DEBUG
                LOGD << error.what();
#endif
                setAppError(format("Gui error: %s", error.what()));
            }
        }
        else
        {
            transitionState();
        }

        SDL_Delay(5);
    }
}

std::string getAppVersion() noexcept
{
    return APP_VERSION;
}

std::string getAppCopyright() noexcept
{
    return APP_COPYRIGHT;
}

std::string getAppTitle() noexcept
{
    return APP_TITLE;
}

} // StreichfettSse

int main(int, char**)
{
#ifdef _DEBUG
    static plog::DebugLogAppender<plog::LogFormatter> debugLogAppender;
    plog::init<plog::LogFormatter>(plog::debug, StreichfettSse::DEBUG_FILE_NAME.c_str()).addAppender(&debugLogAppender);
    LOGD << "<beginning of application>";
#endif
    try
    {
        StreichfettSse::initialize();
    }
    catch (std::exception& e)
    {
#ifdef _DEBUG
        LOGD << e.what();
#endif
        printf("%s", e.what());
        StreichfettSse::finalize();
        exit(EXIT_FAILURE);
    }

    StreichfettSse::loop();
    StreichfettSse::finalize();

#ifdef _DEBUG
    LOGD << "<end of application>";
#endif
    return 0;
}