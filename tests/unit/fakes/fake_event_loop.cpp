
/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <event_loop.hpp>
#include "CppUTestExt/MockSupport.h"

using namespace std;
using namespace hal;

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

/** Obviously, the fake EventLoop doesn't run forever as we need the tests to
 * stop somehow.
 * It will instead pop all the events and call the event handlers.
 * Unit tests should make sure that all the expected events are pushed BEFORE
 * calling run, in order for the test to succeed. */
void EventLoop::run()
{
    while (!event_queue.empty()) {
        auto event_handler = event_queue.front();
        event_queue.pop_front();
        event_handler();
    }
}

void EventLoop::pushEvent(std::function<void()>&& event_handler)
{
    event_queue.push_back(event_handler);
}
