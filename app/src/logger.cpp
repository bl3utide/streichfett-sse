#include "common.hpp"
#include "logger.hpp"

namespace StreichfettSse
{
namespace Logger
{

#ifdef _DEBUG
// public
std::vector<DisplayFormattedDebugLog> dlog;
int dlog_selected_index = -1;
DisplayFormattedDebugLog dlog_selected;
const size_t MAX_SIZE_DISPLAY_DLOG = 1000;
std::mutex dlog_mutex;

// private
int DisplayFormattedDebugLog::_next_log_id = 0;
std::string _debug_file_name;
#endif
std::string _error_file_name;

void initialize() noexcept
{
    enum LogId
    {
        Error = 1,
    };

#ifdef _DEBUG
    _debug_file_name = StringUtil::format("%s.debug.log", APP_NAME.c_str());
    static plog::DebugLogAppender<plog::DebugLogFormatter> debugLogAppender;
    plog::init<plog::DebugLogFormatter>(plog::debug, _debug_file_name.c_str()).addAppender(&debugLogAppender);
    LOGD << "<beginning of application>";
#endif
    _error_file_name = StringUtil::format("%s.error.log", APP_NAME.c_str());
    plog::init<plog::ErrorLogFormatter, LogId::Error>(plog::error, _error_file_name.c_str());
}

void error(const UncontinuableException& uce, const bool no_debug) noexcept
{
    if (!no_debug)
    {
        debug(uce.what());
    }

    LERROR << uce.getErrorMessage();
    LERROR << "<< " << uce.what();
}

void debug(const std::string& message) noexcept
{
#ifdef _DEBUG
    LDEBUG << message;
#endif
}

} // Logger
} // StreichfettSse