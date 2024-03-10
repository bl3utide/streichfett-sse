#pragma once

namespace StreichfettSse
{
namespace Connector
{

extern SDL_TimerID _waiting_timer;
extern bool _is_waiting_store_delay;

bool isWaitingStoreDelay() noexcept;

} // Connector
} // StreichfettSse
