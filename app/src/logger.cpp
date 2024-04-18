#include "common.hpp"
#include "logger.hpp"

namespace StreichfettSse
{
namespace Logger
{

#ifdef _DEBUG
// public
std::list<DisplayFormattedDebugLog> logs;
const size_t MAX_DISPLAY_LOGS = 100;

// private
int DisplayFormattedDebugLog::_next_log_id = 0;
const std::string DEBUG_FILE_NAME = StringUtil::format("%s.debug.log", APP_NAME.c_str());
#endif
const std::string ERROR_FILE_NAME = StringUtil::format("%s.error.log", APP_NAME.c_str());

void initialize() noexcept
{
    enum LogId
    {
        Error = 1,
    };

#ifdef _DEBUG
    static plog::DebugLogAppender<plog::DebugLogFormatter> debugLogAppender;
    plog::init<plog::DebugLogFormatter>(plog::debug, DEBUG_FILE_NAME.c_str()).addAppender(&debugLogAppender);
    LOGD << "<beginning of application>";
#endif
    plog::init<plog::ErrorLogFormatter, LogId::Error>(plog::error, ERROR_FILE_NAME.c_str());
}

} // Logger
} // StreichfettSse