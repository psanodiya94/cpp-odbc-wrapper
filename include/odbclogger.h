#ifndef ODBC_LOGGER_H
#define ODBC_LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>
#include <string>

namespace ps {
    namespace odbc {
        class OdbcLogger {
        public:
            static void initialize(const std::string& logFilePath) {
                spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
                auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath, true);
                auto logger = std::make_shared<spdlog::logger>("OdbcLogger", fileSink);
                spdlog::set_default_logger(logger);
                spdlog::set_level(spdlog::level::info); // Set default log level
            }

            static void logInfo(const std::string& message) {
                spdlog::info(message);
            }

            static void logError(const std::string& message) {
                spdlog::error(message);
            }
        };
    }
}
#endif // ODBC_LOGGER_H
