
/*******************************************************************************
 * A class responsible for receiving new driver events and processing them
 ******************************************************************************/

#ifndef _HAL_EVENT_LOOP_HPP
#define _HAL_EVENT_LOOP_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <functional>
#include <list>


namespace hal
{
/*******************************************************************************
 * CLASS DEFINITION
 ******************************************************************************/

class EventLoop
{
  public:
    typedef std::function<void()>&& EventCallback;

    void run();
    void pushEvent(EventCallback event_handler);

  private:
    std::list<std::function<void()>> event_queue;
};

}  // namespace hal

#endif
