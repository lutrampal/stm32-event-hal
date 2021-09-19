
/*******************************************************************************
 * Implementation file of Logger class
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "logger.hpp"

#include "character_stream_buffer.hpp"

#include <device/system.hpp>
#include <driver/character_driver.hpp>

using namespace std;
using namespace hal;
using namespace component;
using namespace driver;
using namespace device;


/*******************************************************************************
 * PRIVATE TYPE DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * STATIC FUNCTION DECLARATIONS
 ******************************************************************************/

/*******************************************************************************
 * STATIC FUNCTION DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 * CONSTRUCTORS & DESTRUCTOR
 ******************************************************************************/

Logger::Logger()
: driver{System::getInstance().getEventLoop(),
         System::getInstance().getUart(logging_uart_id)},
  buffer{driver}, os{&buffer}
{
}

Logger::~Logger()
{
}

/*******************************************************************************
 * OPERATOR IMPLEMENTATIONS
 ******************************************************************************/

/*******************************************************************************
 * PRIVATE METHOD IMPLEMENTATIONS
 ******************************************************************************/

/*******************************************************************************
 * PROTECTED METHOD IMPLEMENTATIONS
 ******************************************************************************/

/*******************************************************************************
 * PUBLIC METHOD IMPLEMENTATIONS
 ******************************************************************************/

std::basic_ostream<char>& Logger::getOutputStream()
{
    return os;
}

Logger& Logger::getInstance()
{
    static Logger s;

    return s;
}