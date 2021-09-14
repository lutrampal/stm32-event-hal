
/*******************************************************************************
 * Interface file for character-like devices
 ******************************************************************************/

#ifndef _HAL_DEVICE_CHARACTER_DEVICE_HPP
#define _HAL_DEVICE_CHARACTER_DEVICE_HPP

/*******************************************************************************
 * INCLUDE DIRECTIVES
 ******************************************************************************/

#include "error_status.hpp"

#include <cstddef>
#include <functional>
#include <optional>

namespace hal
{
namespace device
{
/*******************************************************************************
 * CLASS DEFINITION
 ******************************************************************************/

template<typename T>
class CharacterDevice
{
  public:
    /** Set the callback function that will be called once the write operation
     * is complete (i.e. the specified number of bytes was written or the
     * operation failed). This will be called from an interrupt context.
     * @param callback
     *  The new callback function. It will receive as paramater the number of
     * bytes written and an error status indicating if the write operation was
     * succesfully executed or not. */
    void setWriteCompleteCallback(
        std::function<void(size_t, ErrorStatus&&)>&& callback)
    {
        write_complete_callback = callback;
    }
    /** Set the callback function that will be called once the read operation
     * is complete (i.e. the specified number of bytes was read, the stop char
     * was encountered or the operation failed). This will be called from an
     * interrupt context.
     * @param callback
     *  The new callback function. It will receive as paramater the number of
     * bytes read and an error status indicating if the write operation was
     * succesfully executed or not. */
    void setReadCompleteCallback(
        std::function<void(size_t, ErrorStatus&&)>&& callback)
    {
        read_complete_callback = callback;
    }

    /** Begin a write operation
     * @param buf
     *  Pointer to the first character to write
     * @param buf_size
     *  The number of characters to write */
    virtual void startWrite(const T* buf, size_t buf_size) = 0;
    /** Cancel a running write operation.
     *  When cancelling a write operation, the device must still make the call
     * to write_complete_callback but it must pass the Aborted StatusCode as
     * parameter. */
    virtual bool cancelWrite() = 0;

    /** Begin a write operation
     * @param buf
     *  Pointer to start of the
     * @param buf_size
     *  The number of characters to read
     * @param stop_char
     *  An optional stop character, reading will stop before the buffer is full
     * if this character is encountered. */
    virtual void startRead(T* buf,
                           size_t buf_size,
                           std::optional<T> stop_char = std::nullopt) = 0;
    virtual bool cancelRead()                                         = 0;

  protected:
    /* Read buffer */
    T* buf_in;
    /* Write buffer */
    const T* buf_out;
    std::optional<T> read_stop_char;
    size_t nb_to_write = 0;
    size_t nb_written  = 0;
    size_t nb_to_read  = 0;
    size_t nb_read     = 0;
    std::function<void(size_t, ErrorStatus&&)> write_complete_callback;
    std::function<void(size_t, ErrorStatus&&)> read_complete_callback;
};

}  // namespace device
}  // namespace hal

#endif
