
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
    /** Cancel the currently running write operation.
     * If no write op is running then @ref CancelAsyncOpFailure will be raised.
     * The callback given when calling @ref asyncWrite previously will be called
     * with the Aborted status code. */
    void cancelAsyncWrite();

    /** Start an asynchronous read operation on the character device
     * @param buf
     *  A buffer where read characters will be copied
     * @param nb_elem
     *  The number of elements to read at most (= size of the given @ref buf)
     * @param stop_char
     *  Read will stop if this char is encountered, even if the buffer isn't
     * full
     * @param event_callback
     *  The event which will be published to the queue once the read operation
     * is complete/canceled. The callback will receive the number of bytes
     * read and an error status as parameters.
     * The given callback std::function may be empty, in which case no event
     * will be published to the queue. */
    void asyncRead(
        T* buf,
        size_t nb_elem,
        std::function<void(size_t, device::ErrorStatus&)>&& event_callback =
            std::function<void(size_t, device::ErrorStatus&)>{},
        std::optional<T> stop_char = std::nullopt);
    /** Cancel the currently running read operation.
     * If no read op is running then @ref CancelAsyncOpFailure will be raised.
     * The callback given when calling @ref asyncRead previously will be called
     * with the Aborted status code. */
    void cancelAsyncRead();

  private:
    bool busy_w = false;
    std::function<void(size_t, device::ErrorStatus&)> write_callback;
    bool busy_r = false;
    std::function<void(size_t, device::ErrorStatus&)> read_callback;

    void completeWrite(size_t nb_written, hal::device::ErrorStatus&& status);
    void completeRead(size_t nb_read, hal::device::ErrorStatus&& status);

    EventLoop& event_loop;
    device::CharacterDevice<T>& device;
};

}  // namespace driver
}  // namespace hal

#include "character_driver_impl.hpp"

#endif
