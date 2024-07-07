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

enum class InitSection : int
{
    Sdl,
    Gui,
    Image,
    Connector,
    Config,

    _COUNT_,
};

// private
std::bitset<static_cast<int>(InitSection::_COUNT_)> init_flag_;

void initialize()
{
    Logger::initialize();
    Logger::debug("<beginning of application>");

    initState();

    try
    {
        Logger::debug("start init SDL");
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
        {
            throw UncontinuableException("SDL_Init error", ERROR_WHEN_INIT, ERROR_CAUSE_INIT_SDL);
        }
        init_flag_.set(static_cast<int>(InitSection::Sdl));

        try
        {
            Logger::debug("start init GUI");
            Gui::initialize();
            init_flag_.set(static_cast<int>(InitSection::Gui));
        }
        catch (std::exception& e)
        {
            throw UncontinuableException(e.what(), ERROR_WHEN_INIT, ERROR_CAUSE_INIT_GUI);
        }

        try
        {
            Logger::debug("start init Image");
            Image::initialize();
            init_flag_.set(static_cast<int>(InitSection::Image));
        }
        catch (std::exception& e)
        {
            throw UncontinuableException(e.what(), ERROR_WHEN_INIT, ERROR_CAUSE_INIT_IMAGE);
        }

        try
        {
            Logger::debug("start init Connector");
            Connector::initialize();
            init_flag_.set(static_cast<int>(InitSection::Connector));
        }
        catch (RtMidiError& e)
        {
            throw UncontinuableException(e.getMessage().c_str(), ERROR_WHEN_INIT, ERROR_CAUSE_INIT_CONN);
        }

        try
        {
            Logger::debug("start init Config");
            Config::initialize();
            init_flag_.set(static_cast<int>(InitSection::Config));
        }
        catch (std::exception& e)
        {
            throw UncontinuableException(e.what(), ERROR_WHEN_INIT, ERROR_CAUSE_INIT_CONFIG);
        }
    }
    catch (UncontinuableException& uce)
    {
        Logger::error(uce);
        Gui::showMessageBox(SDL_MESSAGEBOX_ERROR, "Error", uce.getErrorMessage().c_str());
        throw std::runtime_error("");
    }
}

void finalize() noexcept
{
    if (init_flag_[static_cast<int>(InitSection::Config)])
    {
        Config::save();
    }

    if (init_flag_[static_cast<int>(InitSection::Connector)])
    {
        Connector::finalize();
    }

    if (init_flag_[static_cast<int>(InitSection::Image)])
    {
        Image::finalize();
    }

    if (init_flag_[static_cast<int>(InitSection::Gui)])
    {
        Gui::finalize();
    }

    if (init_flag_[static_cast<int>(InitSection::Sdl)])
    {
        SDL_Quit();
    }

    Logger::debug("<end of application>");
}

void loop()
{
    SDL_Event event;
    auto running = true;

    while (running)
    {
        // pick up all SDL events that occured in this loop
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
            {
                setNextState(State::PrepareToExit);
            }
        }

        // processing branches depending on current state
        try
        {
            running = processForCurrentState();
        }
        catch (ContinuableException& ce)
        {
            Logger::debug(ce);
            setAppError(ce.getErrorMessage().c_str());
            setNextState(ce.getNextState());
        }
        catch (UncontinuableException& uce)
        {
            Logger::error(uce);
            Gui::showMessageBox(SDL_MESSAGEBOX_ERROR, "Error", uce.getErrorMessage().c_str());
            throw std::runtime_error("");
        }
        catch (std::exception& error)
        {
            UncontinuableException uce(error.what(), ERROR_WHEN_STATE_PROCESS);
            Logger::error(uce);
            Gui::showMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error by unexpected cause");
            throw std::runtime_error("");
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
                UncontinuableException uce(error.what(), ERROR_WHEN_RESERVED_FUNC);
                Logger::error(uce);
                Gui::showMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error by unexpected cause");
                throw std::runtime_error("");
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