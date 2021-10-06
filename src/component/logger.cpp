
/*******************************************************************************
 * Implementation file of logger functions
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "logger.hpp"

#include "character_stream_buffer.hpp"

#include <device/system.hpp>

using namespace std;
using namespace hal;
using namespace component;
using namespace driver;
using namespace device;

/*******************************************************************************
 * STATIC OBJECTS
 ******************************************************************************/

static CharacterDriver<char> log_driver{
    System::getInstance().getEventLoop(),
    System::getInstance().getUartWithDma(logging_uart_id)};
static CharacterStreamBuffer log_buffer{log_driver};

/*******************************************************************************
 * PUBLIC FUNCTION IMPLEMENTATIONS
 ******************************************************************************/

void logger_init_stdios()
{
    cout.rdbuf(&log_buffer);
    cerr.rdbuf(&log_buffer);
    cin.rdbuf(&log_buffer);
    clog.rdbuf(&log_buffer);
}
