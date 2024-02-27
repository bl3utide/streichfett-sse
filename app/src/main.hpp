/*
    ImGui Project
    0.2.2
*/
/*
    (Project Name)
    Copyright (C) 20xx bl3utide <bl3utide@gmail.com>
    1.0.0
*/
#pragma once

namespace StreichfettSse
{

#define DEF_APP_NAME        "ImGuiProject"      // TODO fix app name
#define DEF_APP_VERSION     "1.0"
#define DEF_APP_DEV_BY      "bl3utide"
#define DEF_APP_DEV_YR      2024                // TODO fix dev year
#define DEF_APP_TITLE       "ImGui Project"     // TODO fix app title


std::string getAppVersion() noexcept;
std::string getAppCopyright() noexcept;
std::string getAppTitle() noexcept;

} // StreichfettSse
