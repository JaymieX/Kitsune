#pragma once

#include <memory>
#include <string>
#include <vector>
#include <cassert>

#include <spdlog/async.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/bundled/color.h>

#define LOG_ENGINE            "Engine"
#define LOG_LOW_LEVEL_GRAPHIC "LowLevelGraphic"
#define LOG_IO                "IO"

namespace Kitsune
{
    enum class KitLogLevel
    {
        LOG_TRACE,
        LOG_INFO,
        LOG_WARNING,
        LOG_ERROR,
    };
    
    class KitLog
    {
        inline static std::vector<spdlog::sink_ptr> logger_sinks_;

        inline static std::shared_ptr<spdlog::logger> low_level_graphic_logger_;
        inline static std::shared_ptr<spdlog::logger> engine_logger_;
        
    public:
        static void InitLoggers();

        template <typename... Args>
        static void Log(const char* category, KitLogLevel level, const fmt::format_string<Args...>& fmt, Args &&...args)
        {
            std::shared_ptr<spdlog::logger> logger = spdlog::get(category);

            if (logger != nullptr)
            {
                switch (level)
                {
                case KitLogLevel::LOG_TRACE:
                    logger->trace(fmt, std::forward<Args>(args)...);
                    break;
                case KitLogLevel::LOG_INFO:
                    logger->info(fmt, std::forward<Args>(args)...);
                    break;
                case KitLogLevel::LOG_WARNING:
                    logger->warn(fmt, std::forward<Args>(args)...);
                    break;
                case KitLogLevel::LOG_ERROR:
                    logger->error(fmt, std::forward<Args>(args)...);
                    break;
                }
            }
        }
        
        static void Log(const char* category, KitLogLevel level, const char* message)
        {
            std::shared_ptr<spdlog::logger> logger = spdlog::get(category);

            if (logger != nullptr)
            {
                switch (level)
                {
                case KitLogLevel::LOG_TRACE:
                    logger->trace(message);
                    break;
                case KitLogLevel::LOG_INFO:
                    logger->info(message);
                    break;
                case KitLogLevel::LOG_WARNING:
                    logger->warn(message);
                    break;
                case KitLogLevel::LOG_ERROR:
                    logger->error(message);
                    break;
                }
            }
        }

        template <typename... Args>
        static void Assert(const char* category, const bool condition, const fmt::format_string<Args...>& fmt, Args &&...args)
        {
            if (condition)
                return;
        
            spdlog::get(category)->critical(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void Assert(const char* category, const bool condition, const char* message)
        {
            if (condition)
                return;
        
            spdlog::get(category)->critical(message);
        }
    };

#define KIT_LOG(category, level, ...) Kitsune::KitLog::Log(category, level, __VA_ARGS__)

#define KIT_ASSERT(category, condition, ...) \
Kitsune::KitLog::Assert(category, condition, __VA_ARGS__); \
assert(condition) \

    inline void KitLog::InitLoggers()
    {
        logger_sinks_.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        logger_sinks_.emplace_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/log.txt", 1024*1024*10, 3));
            
        engine_logger_ = std::make_shared<spdlog::logger>(LOG_ENGINE, logger_sinks_.begin(), logger_sinks_.end());
        spdlog::register_logger(engine_logger_);

        low_level_graphic_logger_ = std::make_shared<spdlog::logger>(LOG_LOW_LEVEL_GRAPHIC, logger_sinks_.begin(), logger_sinks_.end());
        spdlog::register_logger(low_level_graphic_logger_);

        KIT_LOG(LOG_ENGINE, KitLogLevel::LOG_INFO, "Logger initialized...");
    }
}