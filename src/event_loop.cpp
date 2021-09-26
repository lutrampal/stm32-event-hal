
/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "event_loop.hpp"

#include "device/irqs.hpp"

using namespace std;
using namespace hal;
using namespace device;

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

void EventLoop::run()
{
    while (true) {
        while (event_queue.empty()) { __WFI(); }

        /* Disable interrupts while accessing event_queue to avoid race
         * conditions with interrupt handlers that may add events to the loop */
        disableInterrupts();
        auto event_handler = event_queue.front();
        event_queue.pop_front();
        event_handler();
        enableInterrupts();
    }
}

void EventLoop::pushEvent(std::function<void()>&& event_handler)
{
    event_queue.push_back(event_handler);
}
