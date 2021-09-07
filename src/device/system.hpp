
/*******************************************************************************
 * Interface file for the system singleton that is responsible for bringing up
 * and down all hardware ressources. It also stores references to these
 * resources for use by other components.
 ******************************************************************************/

#ifndef _HAL_DEVICE_SYSTEM_HPP
#define _HAL_DEVICE_SYSTEM_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "character_device.hpp"
#include "timer_device.hpp"

#include <array>
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

  private:
    System();
    ~System();

    std::array<std::unique_ptr<TimerDevice>, nb_timers> timers;
    std::array<std::unique_ptr<CharacterDevice<char>>, nb_uarts> uarts;
};

}  // namespace device
}  // namespace hal

#endif
