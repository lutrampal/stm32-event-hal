
/*******************************************************************************
 * Implementation file of generic TimerDevice class
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "timer_device.hpp"

#include <hardware/mcu.hpp>

using namespace std;
using namespace hal::device;


/*******************************************************************************
 * CONSTRUCTORS & DESTRUCTOR
 ******************************************************************************/

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

void TimerDevice::setWaitCompleteCallback(
    const std::function<void(ErrorStatus&&)>&& callback)
{
    this->callback = callback;
}
