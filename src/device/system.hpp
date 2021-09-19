
/*******************************************************************************
 * Interface file for the system singleton that is responsible for bringing up
 * and down all hardware ressources upon request. It also stores references to
 * these resources for use by other components.
 ******************************************************************************/

#ifndef _HAL_DEVICE_SYSTEM_HPP
#define _HAL_DEVICE_SYSTEM_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "character_device.hpp"
#include "dma_device.hpp"
#include "timer_device.hpp"

#include <array>
#include <event_loop.hpp>
#include <hardware/mcu.hpp>
#include <memory>

namespace hal
{
namespace device
{
/*******************************************************************************
 * CLASS DEFINITIONS
 ******************************************************************************/

class System
{
  public:
    System(System const&) = delete;
    System& operator=(System const&) = delete;

    static System& getInstance();
    /* Conforming to MCU component naming, timer IDs start at 1 i.e. ID 2 is
     * TIM2 */
    TimerDevice& getTimer(unsigned id);
    CharacterDevice<char>& getUart(unsigned id);
    DmaDevice& getDma(unsigned id);

    EventLoop& getEventLoop();

  private:
    System();
    ~System();

    std::array<std::unique_ptr<TimerDevice>, nb_timers> timers;
    std::array<std::unique_ptr<CharacterDevice<char>>, nb_uarts> uarts;
    std::array<std::unique_ptr<DmaDevice>, nb_dmas> dmas;

    EventLoop event_loop;
};

}  // namespace device
}  // namespace hal

#endif
