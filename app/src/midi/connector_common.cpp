#include "common.hpp"
#include "midi/connector_common.hpp"

namespace StreichfettSse
{
namespace Connector
{

// public
SDL_TimerID _waiting_timer;
bool _is_waiting_store_delay = false;   // store delay check

} // Connector
} // StreichfettSse
