
/*******************************************************************************
 * A generic character driver that can be used to read and write data to any
 * device that implements the CharacterDevice interface.
 ******************************************************************************/

#ifndef _HAL_DRIVER_CHARACTER_DRIVER_HPP
#define _HAL_DRIVER_CHARACTER_DRIVER_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include <device/character_device.hpp>
#include <event_loop.hpp>

namespace hal
{
namespace driver
{
/*******************************************************************************
 * CLASS DEFINITION
 ******************************************************************************/

template<typename T>
class CharacterDriver
{
  public:
    CharacterDriver(EventLoop& event_loop, device::CharacterDevice<T>& device);

    /** Start an asynchronous write operation on the character device
     * @param buf
     *  Pointer to the first character to write
     * @param nb_elem
     *  The number of elements to write
     * @param event_callback
     *  The event which will be published to the queue once the write operation
     * is complete/canceled. The callback will receive the number of bytes
     * written and an error status as parameters.
     * The given callback std::function may be empty, in which case no event
     * will be published to the queue. */
    void asyncWrite(
        const T* buf,
        size_t nb_elem,
        std::function<void(size_t, device::ErrorStatus&)>&& event_callback =
            std::function<void(size_t, device::ErrorStatus&)>{});

  private:
    struct WriteOp {
        std::function<void(size_t, device::ErrorStatus&)> callback;
        const T* buf;
        size_t nb_elem;
    };
    std::list<WriteOp> write_queue;

    void completeWrite(size_t nb_written, hal::device::ErrorStatus&& status);

    EventLoop& event_loop;
    device::CharacterDevice<T>& device;
};

}  // namespace driver
}  // namespace hal

#include "character_driver_impl.hpp"

#endif
