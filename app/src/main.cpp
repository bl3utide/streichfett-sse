/*
    Streichfett SSE
    Copyright (C) 2024 bl3utide <bl3utide@gmail.com>
    1.2.0
*/
#include "common.hpp"
#include "error.hpp"
#include "image.hpp"
#include "state.hpp"
#include "config/config.hpp"
#include "gui/gui.hpp"
#include "midi/connector.hpp"
#include "logger.hpp"

namespace StreichfettSse
{

void initialize()
{
    Logger::initialize();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        throw std::runtime_error("SDL_Init error");
    }
    Logger::debug("<beginning of application>");

    try
    {
        Gui::initialize();
        Image::initialize();
        Connector::initialize();
        Config::initialize();
    }
    catch (RtMidiError& error)
    {
#ifdef _DEBUG
        LOGD << error.getMessage();
#endif
        throw;
    }
}

void finalize() noexcept
{
    Config::save();
    Connector::finalize();
    Image::finalize();
    Gui::finalize();

    SDL_Quit();

    Logger::debug("<end of application>");
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
                setNextState(State::PrepareToExit);
        }

        try
        {
            running = processForCurrentState();
        }
        catch (RtMidiError& error)
        {
#ifdef _DEBUG
            LOGD << error.getMessage();
#endif
            setAppError(StringUtil::format("MIDI error: %s", error.getMessage().c_str()));
        }
        catch (std::exception& error)
        {
#ifdef _DEBUG
            LOGD << error.what();
#endif
            setAppError(StringUtil::format("General error: %s", error.what()));
        }

        // check if state changes in the next loop
        if (getNextState() == State::None)
        {
            Gui::drawGui();
            try
            {
                Gui::doReservedFuncs();
            }
            catch (ContinuableException& ce)
            {
                Logger::debug(ce.what());
                setAppError(ce.getErrorMessage().c_str());
                setNextState(ce.getNextState());
            }
            catch (std::exception& error)
            {
                UncontinuableException uce(error.what(), ERROR_WHEN_RESFUNC_ANY);
                Logger::error(uce);
                Gui::showMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error by unexpected cause");
                throw std::runtime_error("");
//#ifdef _DEBUG
//                LOGD << error.what();
//#endif

                //setAppError(StringUtil::format("Gui error: %s", error.what()));
            }
            Gui::clearReservedFuncs();
        }
        else
        {
            transitionState();
        }

        SDL_Delay(5);
    }
}

} // StreichfettSse

int main(int, char**)
{
    try
    {
        StreichfettSse::initialize();
        StreichfettSse::loop();
    }
    catch (std::runtime_error&)
    {
        StreichfettSse::finalize();
        exit(EXIT_FAILURE);
    }

    StreichfettSse::finalize();

    return 0;
}