#include "common.hpp"
#include "logger.hpp"

namespace StreichfettSse
{
namespace Logger
{

#ifdef _DEBUG
// public
std::vector<DisplayFormattedDebugLog> dlog;
int dlog_selected_index;
DisplayFormattedDebugLog dlog_selected;
const size_t MAX_SIZE_DISPLAY_DLOG = 1000;
std::mutex dlog_mutex;

// private
int DisplayFormattedDebugLog::next_log_id_ = 0;
std::string debug_file_name_;
#endif
std::string error_file_name_;

void initialize() noexcept
{
    enum LogId
    {
        Error = 1,
    };

#ifdef _DEBUG
    dlog_selected_index = -1;
    debug_file_name_ = StringUtil::format("%s.debug.log", APP_NAME.c_str());
    static plog::DebugLogAppender<plog::DebugLogFormatter> debugLogAppender;
    plog::init<plog::DebugLogFormatter>(plog::debug, debug_file_name_.c_str()).addAppender(&debugLogAppender);
#endif
    error_file_name_ = StringUtil::format("%s.error.log", APP_NAME.c_str());
    plog::init<plog::ErrorLogFormatter, LogId::Error>(plog::error, error_file_name_.c_str());
}

void error(const UncontinuableException& uce) noexcept
{
#ifdef _DEBUG
    LDEBUG << uce.getErrorMessage().c_str();
    LDEBUG << "<< " << uce.what();
#endif

    LERROR << uce.getErrorMessage().c_str();
    LERROR << "<< " << uce.what();
}

void debug(const ContinuableException& ce) noexcept
{
#ifdef _DEBUG
    LDEBUG << ce.getErrorMessage().c_str();
    LDEBUG << "<< " << ce.what();
#endif
}

void debug(const char* message) noexcept
{
#ifdef _DEBUG
    LDEBUG << message;
#endif
}

void debug(const std::string& message) noexcept
{
    debug(message.c_str());
}

} // Logger
} // StreichfettSse