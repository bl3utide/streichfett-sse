/*
    Streichfett SSE
    Copyright (C) 2024 bl3utide <bl3utide@gmail.com>
    1.2.0
*/
#include "common.hpp"
#include "error.hpp"
#include "image.hpp"
#include "operation.hpp"
#include "state.hpp"
#include "config/config.hpp"
#include "data/internal_patch.hpp"
#include "data/internal_setting.hpp"
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
        throw error;
    }
}

void finalize() noexcept
{
    Config::save();
    Connector::finalize();
    Image::finalize();
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
                setNextState(State::PrepareToExit);
        }

        try
        {
            switch (getState())
            {
                case State::InitInternalData:
                    InternalPatch::initData();
                    InternalSetting::initData();
                    Connector::resetAllConnections();
                    setNextState(State::ApplyConfig);
                    break;
                case State::ApplyConfig:
                    Config::load();
                    Connector::applyConfig();
                    setNextState(State::Idle);
                    break;
                case State::Idle:
                    Connector::sendOneTaskMessage();
                    break;
                case State::RequestInquiry:
                    Connector::requestInquiry();
                    break;
                case State::RequestGlobal:
                    Connector::requestGlobalData();
                    break;
                case State::SendBankProgChange:
                    Connector::sendProgChange();
                    break;
                case State::RequestSound:
                    Connector::requestSoundData();
                    break;
                case State::EnterSoundMode:
                    setOperation(Operation::Sound);
                    if (Connector::isSynthConnected()) setNextState(State::RequestGlobal);
                    else setNextState(State::Idle);
                    break;
                case State::EnterOptionMode:
                    setOperation(Operation::Option);
                    if (Connector::isSynthConnected()) setNextState(State::RequestGlobal);
                    else setNextState(State::Idle);
                    break;
                case State::PrepareToExit:
                    Connector::updateConfig();
                    running = false;
                    break;
                default:
                    break;
            }
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
                setAppError(StringUtil::format("Gui error: %s", error.what()));
            }
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
    }
    catch (std::exception& e)
    {
#ifdef _DEBUG
        LOGD << e.what();
#endif
        LERROR << e.what();
        StreichfettSse::Gui::showMessageBox(SDL_MESSAGEBOX_ERROR, "Error", e.what());
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