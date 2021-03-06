
/*******************************************************************************
 * Basic logging utility
 ******************************************************************************/

#ifndef _HAL_COMPONENT_LOGGER_HPP
#define _HAL_COMPONENT_LOGGER_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <component/character_stream_buffer.hpp>
#include <driver/character_driver.hpp>
#include <ostream>

/*******************************************************************************
 * MACRO DEFINITION
 ******************************************************************************/

#define LOG_LEVEL_TRACE   0
#define LOG_LEVEL_DEBUG   1
#define LOG_LEVEL_INFO    2
#define LOG_LEVEL_WARNING 3
#define LOG_LEVEL_ERROR   4
#define LOG_LEVEL_FATAL   5
#define LOG_LEVEL_NONE    6

#ifndef LOG_LEVEL
    #define LOG_LEVEL LOG_LEVEL_NONE
#endif

#if LOG_LEVEL <= LOG_LEVEL_TRACE
    #define LOG_TRACE(module, msg) LOG("TRACE", module, msg)
#else
    #define LOG_TRACE(module, msg)
#endif

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
    #define LOG_DEBUG(module, msg) LOG("DEBUG", module, msg)
#else
    #define LOG_DEBUG(module, msg)
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
    #define LOG_INFO(module, msg) LOG("INFO", module, msg)
#else
    #define LOG_INFO(module, msg)
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARNING
    #define LOG_WARNING(module, msg) LOG("WARNING", module, msg)
#else
    #define LOG_WARNING(module, msg)
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
    #define LOG_ERROR(module, msg) LOG("ERROR", module, msg)
#else
    #define LOG_ERROR(module, msg)
#endif

#if LOG_LEVEL <= LOG_LEVEL_FATAL
    #define LOG_FATAL(module, msg) LOG("FATAL", module, msg)
#else
    #define LOG_FATAL(module, msg)
#endif

// TODO: Add timestamp?
#define LOG(level, module, msg)                                                \
    do {                                                                       \
        hal::component::Logger::getInstance().getOutputStream()                \
            << level << " - " << module << " - " << msg << "\r\n"              \
            << std::flush;                                                     \
    } while (false)

namespace hal
{
namespace component
{
/*******************************************************************************
 * CLASS DEFINITION
 ******************************************************************************/

class Logger
{
  public:
    Logger(Logger const&) = delete;
    Logger& operator=(Logger const&) = delete;

    static Logger& getInstance();
    std::basic_ostream<char>& getOutputStream();

  private:
    Logger();
    ~Logger();

    hal::driver::CharacterDriver<char> driver;
    hal::component::CharacterStreamBuffer<char> buffer;
    std::basic_ostream<char> os;
};

}  // namespace component
}  // namespace hal

#endif
